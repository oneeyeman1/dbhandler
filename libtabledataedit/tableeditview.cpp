/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * tableeditview.cc
 * Copyright (C) 2020 Igor Korot <igor@IgorReinCloud>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include <map>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/artprov.h"
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/mstream.h"
#include "wx/grid.h"
#include "typecombobox.h"
#include "database.h"
#include "configuration.h"                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          
#include "tableattributes.h"
#include "tableeditdocument.h"
#include "tableeditview.h"
#include "tableeditcanvas.h"

#include "res/queryexec.xpm"
#include "res/querycancel.xpm"

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::map<wxString, std::vector<TableDefinition> > &, std::vector<std::wstring> &, bool, const int, bool);

wxIMPLEMENT_DYNAMIC_CLASS(TableEditView, wxView);

wxBEGIN_EVENT_TABLE(TableEditView, wxView)
    EVT_MENU(wxID_CLOSE, TableEditView::OnClose)
wxEND_EVENT_TABLE()

bool TableEditView::OnCreate(wxDocument *doc, long flags)
{
    m_currentRow = 0;
    m_processed = 0;
    m_queryexecuting = true;
    m_tb = nullptr;
    if( !wxView::OnCreate( doc, flags ) )
        return false;
    m_parent = wxStaticCast( wxTheApp->GetTopWindow(), wxDocMDIParentFrame );
    wxRect clientRect = m_parent->GetClientRect();
    wxWindowList children = m_parent->GetChildren();
    for( wxWindowList::iterator it = children.begin(); it != children.end(); it++ )
    {
        auto tb = wxDynamicCast( *it, wxToolBar );
        if( tb && tb->GetName() == "ViewBar" )
        {
            m_tb = tb;
        }
        if( tb && tb->GetName() == "StyleBar" )
            m_styleBar = tb;
    }
    auto stdPath = wxStandardPaths::Get();
#ifdef __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    m_libPath = fn.GetPathWithSep() + "Frameworks/";
#elif __WXGTK__ || __WXQT__
    m_libPath = stdPath.GetInstallPrefix() + "/lib/";
#elif __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    m_libPath = fn.GetPathWithSep();
#endif
    const wxString tableName = dynamic_cast<TableEditDocument *>( doc )->GetTableName();
    wxString title = _( "Create/Alter Table" );
    m_frame = new wxDocMDIChildFrame( doc, this, m_parent, wxID_ANY, title, wxDefaultPosition, wxSize( clientRect.GetWidth(), clientRect.GetHeight() ) );
    sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    m_panel = new wxPanel( m_frame );
    sizer_1->Add( m_panel, 1, wxEXPAND, 0 );
    wxPoint ptCanvas;
    CreateMenuAndToolbar();
#ifdef __WXOSX__
    wxRect parentRect = m_parent->GetRect();
    wxSize parentClientSize = m_parent->GetClientSize();
    m_frame->Move( 0, parentRect.y - parentClientSize.y );
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), parentClientSize.GetHeight() );
    int offset = m_tb->GetSize().y;
    ptCanvas.x = 0;
    ptCanvas.y = offset;
#else
    ptCanvas = wxDefaultPosition;
#endif
    wxASSERT( m_frame == GetFrame() );
/*#ifndef __WXOSX__
    wxSize size = m_parent->GetClientSize();
    m_tb->ClearTools();
    m_tb->AddTool( wxID_QUERYCANCEL, _( "Cancel" ), wxBitmap( querycancel ), _( "Cancel the query" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_TOP, _( "Top" ), wxArtProvider::GetBitmap( wxART_GOTO_FIRST ), _( "Top" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_BACK, _( "Back" ), wxArtProvider::GetBitmap( wxART_GO_BACK ), _( "Back" ), wxITEM_NORMAL );
    m_tb->Realize();
    wxMDIClientWindow *frame = (wxMDIClientWindow *) m_parent->GetClientWindow();
    m_tb->SetSize( 0, 0, size.x, wxDefaultCoord );
    int offset = m_tb->GetSize().y;
    frame->SetSize( 0, offset, size.x, size.y - offset );
    m_frame->SetSize( 0, 0, size.x, size.y - offset - 2 );
#endif*/
    m_frame->Layout();
    m_frame->Show();
    return true;
}

TableEditView::~TableEditView()
{
    wxMenuBar *bar = m_parent->GetMenuBar();
    for( auto i = bar->GetMenuCount() - 2; i > 0; i-- )
    {
        auto menu = bar->Remove( i );
        delete menu;
    }
}

void TableEditView::OnClose(wxCommandEvent &WXUNUSED(event))
{
    m_frame->Close();
}

void TableEditView::OnDraw(wxDC *dc)
{
}

bool TableEditView::OnClose(bool deletedwndow)
{
    wxDocMDIParentFrame *mainWin = (wxDocMDIParentFrame *) m_frame->GetMDIParent();
    wxMDIClientWindow *frame = (wxMDIClientWindow *) mainWin->GetClientWindow();
    if( GetDocument()->GetViewsVector().size() == 1 )
    {
        //        int y = m_tb->GetHeight();
        frame->SetPosition( wxPoint( 0, 0 ) );
        delete m_tb;
        m_tb = nullptr;
    }
    else
        m_tb->ClearTools();
    if( !wxView::OnClose( deletedwndow ) )
        return false;

    //    Activate( false );

    return true;
}

void TableEditView::GetTablesForView(Database *db, bool init)
{
    std::map<wxString, std::vector<TableDefinition> > tables;
    std::vector<std::wstring> tableNames;
    int res = -1;
    wxString query, documentName = "", type = db->GetTableVector().m_type, subtype = db->GetTableVector().m_subtype;
    wxString libName;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    libName = m_libPath + "dialogs";
#elif __WXMAC__
    libName = m_libPath + "liblibdialogs.dylib";
#else
    libName = m_libPath + "libdialogs";
#endif
    lib.Load( libName );
    if( lib.IsLoaded() )
    {
        TABLESELECTION func2 = (TABLESELECTION) lib.GetSymbol( "SelectTablesForView" );
        res = func2( m_frame, db, tables, tableNames, true, 1, true );
        if( res == wxID_CANCEL )
        {
            m_frame->Close();
            return;
        }
    }
    auto found = false;
    DatabaseTable *table = nullptr;
    for( std::map<wxString, std::vector<TableDefinition> >::iterator it = tables.begin(); it != tables.end() && !found; ++it )
    {
        for( auto it1 = db->GetTableVector().m_tables.begin(); it1 != db->GetTableVector().m_tables.end() && !found; ++ it1 )
            for( auto it2 = ( *it1 ).second.begin(); it2 < ( *it1 ).second.end() && !found; ++it2 )
            {
                if( (*it).first == (*it1).first &&
                    (*it).second.at( 0 ).schemaName == (*it2)->GetSchemaName() &&
                    (*it).second.at( 0 ).tableName == (*it2)->GetTableName() )
                {
                    dynamic_cast<TableEditDocument *>( GetDocument() )->SetTable( (*it2) );
                    table = (*it2);
                    found = true;
                }
            }
    }
    auto sizer_2 = new wxBoxSizer( wxVERTICAL );
    m_grid = new wxGrid( m_panel, wxID_ANY );
    auto rows = 1;
    m_grid->CreateGrid( rows, 6 );
    m_grid->SetColLabelValue( 0, _( "Name" ) );
    m_grid->SetColLabelValue( 1, _( "Data Type" ) );
    m_grid->SetColLabelValue( 2, _( "Width" ) );
    m_grid->SetColLabelValue( 3, _( "Dec" ) );
    m_grid->SetColLabelValue( 4, _( "Null" ) );
    m_grid->SetColLabelValue( 5, _( "Default" ) );
    m_grid->SetRowLabelValue( 0, "" );
    wxString selString;
    wxArrayString choices;
    if( type == L"SQLite" )
    {
        choices.push_back( "NULL" );
        choices.push_back( "Text" );
        choices.push_back( "Integer" );
        choices.push_back( "Real" );
        choices.push_back( "BLOB" );
/*        if( argType == L"" )
            selString = "Integer";
        else
            selString = argType;*/
    }
    if( ( type == L"ODBC" && subtype == L"Microsoft SQL Server" ) || type == L"Microsoft SQL Server" )
    {
        choices.push_back( "bigint" );
        choices.push_back( "binary" );
        choices.push_back( "bit" );
        choices.push_back( "char" );
        choices.push_back( "date" );
        choices.push_back( "datetime" );
        choices.push_back( "datetime2" );
        choices.push_back( "datetimeoffset" );
        choices.push_back( "decimal" );
        choices.push_back( "float" );
        choices.push_back( "geography" );
        choices.push_back( "geometry" );
        choices.push_back( "hierarchyid" );
        choices.push_back( "image" );
        choices.push_back( "int" );
        choices.push_back( "money" );
        choices.push_back( "nchar" );
        choices.push_back( "ntextr" );
        choices.push_back( "numeric" );
        choices.push_back( "nvarchar" );
        choices.push_back( "real" );
        choices.push_back( "smalldatetime" );
        choices.push_back( "smallint" );
        choices.push_back( "smallmoney" );
        choices.push_back( "sql_variant" );
        choices.push_back( "text" );
        choices.push_back( "time" );
        choices.push_back( "timestamp" );
        choices.push_back( "tinyint" );
        choices.push_back( "uniqueidentifier" );
        choices.push_back( "varbinary" );
        choices.push_back( "varchar" );
        choices.push_back( "xml" );
/*        if( argType == L"" )
            selString = "numerc";
        else
            selString = argType;*/
    }
    if( ( type == L"ODBC" && subtype == L"PostgreSQL" ) || type == L"PostgreSQL" )
    {
        choices.push_back( "JSON" );
        choices.push_back( "UUID" );
        choices.push_back( "XML" );
        choices.push_back( "bigint" );
        choices.push_back( "bigserial" );
        choices.push_back( "bit" );
        choices.push_back( "boolean" );
        choices.push_back( "box" );
        choices.push_back( "bytea" );
        choices.push_back( "char" );
        choices.push_back( "character varying" );
        choices.push_back( "character" );
        choices.push_back( "cidr" );
        choices.push_back( "circle" );
        choices.push_back( "date" );
        choices.push_back( "daterange" );
        choices.push_back( "decimal" );
        choices.push_back( "double precison" );
        choices.push_back( "enum" );
        choices.push_back( "inet" );
        choices.push_back( "int4range" );
        choices.push_back( "int8range" );
        choices.push_back( "integer" );
        choices.push_back( "interval" );
        choices.push_back( "line" );
        choices.push_back( "lseg" );
        choices.push_back( "macaddr" );
        choices.push_back( "macaddr8" );
        choices.push_back( "money" );
        choices.push_back( "numeric" );
        choices.push_back( "numrange" );
        choices.push_back( "path" );
        choices.push_back( "point" );
        choices.push_back( "polygon" );
        choices.push_back( "real" );
        choices.push_back( "serial" );
        choices.push_back( "smallint" );
        choices.push_back( "smallserial" );
        choices.push_back( "text" );
        choices.push_back( "time" );
        choices.push_back( "timestamp" );
        choices.push_back( "tsrange" );
        choices.push_back( "tstzrange " );
        choices.push_back( "tsvector" );
        choices.push_back( "tsquery" );
        choices.push_back( "varchar" );
/*        if( argType == L"" )
            selString = "numerc";
        else
            selString = argType;*/
    }
    if( table && !table->GetFields().empty() )
    {
        for( std::vector<TableField *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); ++it )
        {
            m_grid->SetCellValue( rows - 1, 0, (*it)->GetFieldName() );
            m_grid->SetCellValue( rows - 1, 1, (*it)->GetFieldType() );
            m_grid->SetCellRenderer( rows - 1, 1, new wxGridCellChoiceRenderer( (*it)->GetFieldType() ) );
            m_grid->SetCellEditor( rows - 1, 1, new wxGridCellChoiceEditor( choices ) );
            auto width = (*it)->GetFieldSize();
            auto precision = (*it)->GetPrecision();
            if( width > 0 )
                m_grid->SetCellValue( rows - 1, 2, wxString::Format( "%d", width ) );
            else
                m_grid->SetCellValue( rows - 1, 2, "" );
            if( precision > 0 )
                m_grid->SetCellValue( rows - 1, 3, wxString::Format( "%d", precision ) );
            m_grid->SetCellRenderer( rows - 1, 4, new wxGridCellChoiceRenderer( "Yes,No" ) );
            m_grid->SetCellValue( rows - 1, 4, (*it)->IsNullAllowed() ? "Yes" : "No" );
            rows++;
            m_grid->AppendRows();
            m_grid->SetRowLabelValue( rows - 1, "" );
        }
    }
    sizer_2->Add( m_grid, 1, wxEXPAND, 0 );
/*    auto gridsizer = new wxFlexGridSizer( 6, 0, 0 );
//    auto gridsizer = new wxGridSizer( 6, 0, 0 );
    gridsizer->AddGrowableRow( 0, 1 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Field Name" ) ), 1, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Field Type" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Width" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Dec" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Null" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Default Value" ) ), 1, wxEXPAND, 0 );
    const wxString nullChoices[] =
    {
        "Yes",
        "No"
    };
    const wxString defaultChoices[] = 
    {
        "None",
        "autoincrement",
        "current date",
        "current time",
        "current tmestamp",
        "timestamp",
        "null",
        "user"
    };
    m_grid->Freeze();
    if( table && !table->GetFields().empty() )
    {
        for( std::vector<TableField *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); ++it )
        {
            m_currentRow++;
            auto name = new wxTextCtrl( m_grid, wxID_ANY, (*it)->GetFieldName() );
            name->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            gridsizer->Add( name, 1, wxEXPAND, 0 );
            name->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            name->Bind( wxEVT_KEY_DOWN, &TableEditView::OnKeyDown, this );
            auto type = new TypeComboBox( m_grid, db->GetTableVector().m_type, db->GetTableVector().m_type, (*it)->GetFieldType() );
            type->Disable();
            type->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            type->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            gridsizer->Add( type, 0, wxEXPAND, 0 );
            auto size = new wxTextCtrl( m_grid, wxID_ANY, wxString::Format( "%d", (*it)->GetFieldSize() ) );
            size->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            size->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            gridsizer->Add( size, 0, wxEXPAND, 0 );
            auto precision = new wxTextCtrl( m_grid, wxID_ANY, wxString::Format( "%d", (*it)->GetPrecision() ) );
            precision->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            precision->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            gridsizer->Add( precision, 0, wxEXPAND, 0 );
            auto nullAllowed = new wxComboBox( m_grid, wxID_ANY, (*it)->IsNullAllowed() ? "Yes" : "No" );
            nullAllowed->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            nullAllowed->Disable();
            nullAllowed->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            gridsizer->Add( nullAllowed, 0, wxEXPAND, 0 );
            wxString defValue;
            if( (*it)->GetDefaultValue() == L"" )
                defValue = "(None)";
            else if( (*it)->IsAutoIncrement() )
                defValue = "autoincrement";
            else
                defValue = (*it)->GetDefaultValue();
            auto defaultValue = new wxComboBox( m_grid, wxID_ANY, defValue, wxDefaultPosition, wxDefaultSize, 8, defaultChoices  );
            defaultValue->SetClientObject( (wxClientData *) &(*it)->GetFieldProperties() );
            defaultValue->Bind( wxEVT_SET_FOCUS, &TableEditView::OnFieldSetFocus, this );
            gridsizer->Add( defaultValue, 1, wxEXPAND, 0 );
            auto dbType = type->GetValue();
            m_lines.push_back( TableDefinitionLine( name, type, size, precision, nullAllowed, defaultValue ) );
            if( db->GetTableVector().m_type == L"PostgreSQL" || db->GetTableVector().m_subtype == L"PostgreSQL" )
            {
                if( dbType == L"bigint" ||
                    dbType == L"bigserial" || 
                    dbType == L"boolean" ||
                    dbType == L"box" ||
                    dbType == L"bytes" ||
                    dbType == L"cidr" ||
                    dbType == L"circle" ||
                    dbType == L"date" ||
                    dbType == L"double precision" ||
                    dbType == L"inet" ||
                    dbType == L"integer" ||
                    dbType == L"json" ||
                    dbType == L"jsonb" ||
                    dbType == L"line" ||
                    dbType == L"lseg" ||
                    dbType == L"macaddr" ||
                    dbType == L"macaddr8" ||
                    dbType == L"money" ||
                    dbType == L"path" ||
                    dbType == L"pg_lsn" ||
                    dbType == L"pg_snapshot" ||
                    dbType == L"point" ||
                    dbType == L"polygon" ||
                    dbType == L"real" ||
                    dbType == L"smallint" ||
                    dbType == L"smallserial" ||
                    dbType == L"serial" ||
                    dbType == L"text" ||
                    dbType == L"tsqery" ||
                    dbType == L"txid_snapshot" ||
                    dbType == L"uuid" ||
                    dbType == L"xml" )
                {
                    size->SetValue( "" );
                    precision->SetValue( "" );
                    size->Disable();
                    precision->Disable();
                }
                else if( dbType == L"numeric" ||
                    dbType == L"decimal" )
                {

                }
                else
                {
                    precision->SetValue( "" );
                    precision->Disable();
                }
            }
            else if( db->GetTableVector().m_type == L"Microsoft SQL Server" || db->GetTableVector().m_subtype == L"Microsoft SQL Server" )
            {
                if( dbType == L"bigint" ||
                    dbType == L"bit" ||
                    dbType == L"date" ||
                    dbType == L"datetime" ||
                    dbType == L"float" ||
                    dbType == L"geoography" ||
                    dbType == L"geometry" ||
                    dbType == L"herarchyid" ||
                    dbType == L"image" ||
                    dbType == L"int" ||
                    dbType == L"money" ||
                    dbType == L"ntext" ||
                    dbType == L"real" ||
                    dbType == L"smalldatetime" ||
                    dbType == L"smallint" ||
                    dbType == L"smallmoney" ||
                    dbType == L"sql_variant" ||
                    dbType == L"text" ||
                    dbType == L"timestamp" ||
                    dbType == L"tinyint" ||
                    dbType == L"uniqueidentifier" ||
                    dbType == L"xml"
                    )
                {
                    size->SetValue( "" );
                    precision->SetValue( "" );
                    size->Disable();
                    precision->Disable();
                }
                else if( dbType == L"decimal" ||
                    dbType == L"numeric" )
                {

                }
                else
                {
                    precision->SetValue( "" );
                    precision->Disable();
                }
            }
        }
    }
    else
    {
        gridsizer->Add( new wxTextCtrl( m_grid, wxID_ANY, "" ), 1, wxEXPAND, 0 );
        gridsizer->Add( new TypeComboBox( m_grid, db->GetTableVector().m_type, db->GetTableVector().m_type, L"" ), 0, wxEXPAND, 0 );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ), 1, wxEXPAND, 0 );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ), 1, wxEXPAND, 0 );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "Yes" ), 1, wxEXPAND, 0 );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ), 1, wxEXPAND, 0 );
    }
    m_grid->Thaw();*/
    sizer_2->Add( 5, 5, 1, wxEXPAND, 0 );
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( sizer, 0, wxEXPAND, 0 );
    attributes = new TableSettngs( m_panel, wxID_ANY, db );
    sizer->Add( attributes, 0, wxEXPAND, 0 );
    sizer->AddStretchSpacer();
//    sizer_2->Add( attributes, 0, wxEXPAND, 0 );
//    m_lines.front().m_name->SetFocus();
//    m_lines.front().m_name->SetSelection( -1, -1 );
//    m_grid->SetSizer( gridsizer );
    m_panel->SetSizer( sizer_2 );
    m_frame->SetSizer( sizer_1 );
    m_frame->Layout();
}

void TableEditView::CreateMenuAndToolbar()
{
    int offset = 0;
    long style = wxNO_BORDER | wxTB_FLAT;
    switch( m_tbSettings.m_orientation )
    {
    case 0:
        style |= wxTB_VERTICAL;
        break;
    case 1:
        style |= wxTB_HORIZONTAL;
        break;
    case 2:
        style |= wxTB_RIGHT;
        break;
    case 3:
        style |= wxTB_BOTTOM;
        break;
    }
    if( !m_tbSettings.m_showTooltips )
        style |= wxTB_NO_TOOLTIPS;
    if( m_tbSettings.m_showText )
        style |= wxTB_TEXT ;
    wxWindow *parent = nullptr;
#ifdef __WXOSX__
    parent = m_frame;
#elif __WXMSW__
    parent = m_parent->GetClientWindow();
#elif __WXGTK__
	parent = m_parent;
#endif
    wxSize size = m_parent->GetClientSize();
#ifdef __WXOSX__
    if( !m_tb )
        m_tb = m_frame->CreateToolBar();
    m_tb->SetName( "ViewBar" );
#else
    if( !m_tb )
    {
        m_tb = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "ViewBar" );
    }
    else
        m_tb->ClearTools();
#endif
    auto mbar = new wxMenuBar;
    auto fileMenu = new wxMenu;
    fileMenu->Append( wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    fileMenu->AppendSeparator();
    fileMenu->AppendSeparator();
    fileMenu->Append( wxID_EXIT );
    mbar->Insert( 0, fileMenu, _( "File" ) );
    auto menuEdit = new wxMenu();
    menuEdit->Append( wxID_UNDO, _( "Undo\tCtrl+Z" ), _( "Undo the action" ) );
    menuEdit->AppendSeparator();
    menuEdit->Append( wxID_CUTCOLUMN, _( "Cut column" ), _( "Cut Column" ) );
    menuEdit->Append( wxID_COPYCOLUMN, _( "Copy column" ), _( "Copy Column" ) );
    menuEdit->Append( wxID_PASTECOLUMN, _( "Paste Column" ), _( "Paste Column" ) );
    menuEdit->Append( wxID_INSERTCOLUMN, _( "Insert Column" ), _( "Insert Column" ) );
    menuEdit->Append( wxID_DELETECOLUMN, _( "Delete Column" ), _( "Delete Column" ) );
    menuEdit->AppendSeparator();
    menuEdit->Append( wxID_TABLEPROPERTIES, _( "Table Properties..." ), _( "Table Properties" ) );
    mbar->Insert( 1, menuEdit, _( "&Edit" ) );
    auto menuDesign = new wxMenu();
    menuDesign->Append( wxID_DESIGNSYNTAX, _( "Syntax" ), _( "SQL Syntax View" ) );
    mbar->Insert( 2, menuDesign, _( "&Design" ) );
    auto helpMenu = new wxMenu;
    helpMenu->Append( wxID_HELP, _( "Help" ), _( "Help" ) );
    mbar->Append( helpMenu, _( "Help" ) );
    m_frame->SetMenuBar( mbar );
    wxBitmapBundle save;
#ifdef __WXMSW__
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "tabledataedit", &gs_wxMainThread );
    const void* data = nullptr;
    size_t sizeSave = 0;
    if( !wxLoadUserResource( &data, &sizeSave, "save", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        save = wxBitmapBundle::FromSVG( (const char *) data, wxSize( 16, 16 ) );
    }
#elif __WXOSX__
    save = wxBitmapBundle::FromSVGResource( "save", wxSize( 16, 16 ) );
#else
    save = wxArtProvider::GetBitmapBundle( wxART_FLOPPY, wxART_TOOLBAR );
#endif
    m_tb->AddTool( wxID_NEW, _( "New" ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
    m_tb->AddTool( wxID_OPEN, _( "Open" ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
    m_tb->AddTool( wxID_SAVEQUERY, _( "Save" ), save, save, wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
    m_tb->AddSeparator();
    m_tb->AddTool( wxID_CUTCOLUMN, _( "Cut" ), wxArtProvider::GetBitmapBundle( wxART_CUT ), wxArtProvider::GetBitmapBundle( wxART_CUT ), wxITEM_NORMAL, _( "Cut" ), _( "Cut Column" ) );
    m_tb->AddTool( wxID_COPYCOLUMN, _( "Copy" ), wxArtProvider::GetBitmapBundle( wxART_COPY ), wxArtProvider::GetBitmapBundle( wxART_COPY ), wxITEM_NORMAL, _( "Copy" ), _( "Copy Column" ) );
    m_tb->AddTool( wxID_CUTCOLUMN, _( "Paste" ), wxArtProvider::GetBitmapBundle( wxART_PASTE ), wxArtProvider::GetBitmapBundle( wxART_PASTE ), wxITEM_NORMAL, _( "Paste" ), _( "Paste Column" ) );
    m_tb->AddTool( wxID_DELETECOLUMN, _( "Delete Column" ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxITEM_NORMAL, _( "Delete Column" ), _( "Delete Column" ) );
    m_tb->Realize();
    switch( m_tbSettings.m_orientation )
    {
    case 0:
        m_tb->SetSize( 0, 0,  wxDefaultCoord, size.y );
        offset = m_tb->GetSize().x;
#if defined( __WXMSW__ ) || defined( __WXGTK__ )
        m_frame->SetSize( offset, 0, size.x - offset, size.y );
#endif
        break;
    case 1:
        m_tb->SetSize( 0, 0,  size.x, wxDefaultCoord );
        offset = m_tb->GetSize().y;
#if defined( __WXMSW__ ) || defined( __WXGTK__ )
        m_frame->SetSize( 0, offset, size.x, size.y - offset );
#endif
        break;
    case 2:
        offset = m_tb->GetSize().x;
        m_tb->SetSize( size.x - offset, 0,  offset, size.y );
#if defined( __WXMSW__ ) || defined( __WXGTK__ )
        m_frame->SetSize( 0, 0, size.x - offset, size.y );
#endif
        break;
    case 3:
        offset = m_tb->GetSize().y;
#if defined( __WXMSW__ ) || defined( __WXGTK__ )
        m_frame->SetSize( 0, 0, size.x, size.y - offset );
#endif
        m_tb->SetSize( 0, size.y - offset, size.x, wxDefaultCoord );
        break;
    }
}

void TableEditView::SetToolbarOption(Configuration *conf)
{
    m_tbSettings.m_hideShow = conf->m_tbSettings["ViewBar"].m_hideShow;
    m_tbSettings.m_showTooltips = conf->m_tbSettings["ViewBar"].m_showTooltips;
    m_tbSettings.m_showText = conf->m_tbSettings["ViewBar"].m_showText;
    m_tbSettings.m_orientation = conf->m_tbSettings["ViewBar"].m_orientation;
}

void TableEditView::OnFieldSetFocus(wxFocusEvent &event)
{
    auto field = dynamic_cast<wxWindow *>( event.GetEventObject() );
    FieldProperties *data = reinterpret_cast<FieldProperties *>( field->GetClientObject() );
    attributes->GetCommentCtrl()->SetValue( data->m_comment );
    attributes->GetHeaderCtrl()->SetValue( data->m_heading.m_heading );
    attributes->GetLabelCtrl()->SetValue( data->m_heading.m_label );
    event.Skip();
}

void TableEditView::OnKeyDown(wxKeyEvent &event)
{
    if( event.GetKeyCode() == WXK_UP )
    {
        // range check happens inside method anyway
        SetActiveLine( m_currentRow - 1 );
    }
    else if( event.GetKeyCode() == WXK_DOWN )
    {
        // range check happens inside method anyway
        SetActiveLine( m_currentRow + 1 );
    }
    event.Skip();
}

void TableEditView::SetActiveLine(int line)
{
/*    size_t index;
    if( line == 1 )
        index = 9;
    else
        index = line - 1;
    if( index >= m_lines.size() )
        return;

    // just set focus to the first control in that line
    // focus handler will do the rest
    std::list<TableDefinitionLine>::iterator it = std::next( m_lines.begin(), index );
    (*it).m_name->SetFocus();
*/}
