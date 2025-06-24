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
#include "mytabledefgrid.h"
#include "mycombocellrenderer.h"
#include "mytabletypeeditor.h"
#include "mycombocelleditor.h"
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
    EVT_MENU(wxID_INSERTCOLUMN, TableEditView::OnInsertColumn)
    EVT_MENU(wxID_DELETECOLUMN, TableEditView::OnDeleteColumn)
    EVT_MENU(wxID_TABLEPROPERTIES, TableEditView::OnTableProperties)
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
    wxString query, documentName = "", m_dbType = db->GetTableVector().m_type, m_dbSubtype = db->GetTableVector().m_subtype;
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
    m_grid = new MyTableDefGrid( m_panel, wxID_ANY );
    auto rows = 0;
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
    int count;
    if( m_dbType == L"SQLite" )
    {
        choices.push_back( "NULL" );
        choices.push_back( "Text" );
        choices.push_back( "Integer" );
        choices.push_back( "Real" );
        choices.push_back( "BLOB" );
        count = 5;
        selString = "Integer";
    }
    if( ( m_dbType == L"ODBC" && m_dbSubtype == L"Microsoft SQL Server" ) || m_dbType == L"Microsoft SQL Server" )
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
        count = 33;
        selString = "numerc";
    }
    if( ( m_dbType == L"ODBC" && m_dbSubtype == L"PostgreSQL" ) || m_dbType == L"PostgreSQL" )
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
        count = 46;
        selString = "numerc";
    }
    m_grid->Freeze();
    if( table && !table->GetFields().empty() )
    {
        m_frame->SetTitle( "Alter Table - " + table->GetSchemaName() + "." + table->GetTableName() );
        for( std::vector<TableField *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); ++it )
        {
//            if( it < table->GetFields().end() - 1 )
            {
                AppendOrInsertField( *it );
                rows++;
            }
        }
    }
    m_grid->Thaw();
    sizer_2->Add( m_grid, 1, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 1, wxEXPAND, 0 );
    auto sizer = new wxBoxSizer( wxHORIZONTAL );
    sizer_2->Add( sizer, 0, wxEXPAND, 0 );
    attributes = new TableSettngs( m_panel, wxID_ANY, db );
    sizer->Add( attributes, 0, wxEXPAND, 0 );
    sizer->AddStretchSpacer();
    m_panel->SetSizer( sizer_2 );
    m_frame->SetSizer( sizer_1 );
    m_frame->Layout();
    auto width = m_frame->GetSize().GetWidth();
    auto columns = m_grid->GetColSize( 1 ) + m_grid->GetColSize( 2 ) + m_grid->GetColSize( 3 ) + m_grid->GetColSize( 4 ) + m_grid->GetRowLabelSize();
    auto halfSize = ( width - columns ) / 2;
    m_grid->SetColSize( 0, halfSize );
    m_grid->SetColSize( 5, halfSize );
    m_grid->SetFocus();
    m_grid->Bind( wxEVT_KEY_DOWN, &TableEditView::OnKeyDown, this );
    m_grid->Bind( wxEVT_GRID_SELECT_CELL, &TableEditView::OnCellClicked, this );
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
    wxBitmapBundle save, properties;
#ifdef __WXMSW__
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "tabledataedit", &gs_wxMainThread );
    const void *data1 = nullptr, *data2 = nullptr;
    size_t sizeSave = 0, sizeProperties = 0;
    if( !wxLoadUserResource( &data1, &sizeSave, "save", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        save = wxBitmapBundle::FromSVG( (const char *) data1, wxSize( 16, 16 ) );
    }
    if( !wxLoadUserResource( &data2, &sizeProperties, "properties", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
}
    else
    {
        properties = wxBitmapBundle::FromSVG( (const char *) data2, wxSize( 16, 16 ) );
    }
#elif __WXOSX__
    save = wxBitmapBundle::FromSVGResource( "save", wxSize( 16, 16 ) );
    properties = wxBitmapBundle::FromSVGResource( "properties", wxSize( 16, 16 ) );
#else
    save = wxArtProvider::GetBitmapBundle( wxART_FLOPPY, wxART_TOOLBAR );
    properties = wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) );
#endif
    m_tb->AddTool( wxID_NEW, _( "New" ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_NEW, wxART_TOOLBAR ), wxITEM_NORMAL, _( "New" ), _( "New Query" ) );
    m_tb->AddTool( wxID_OPEN, _( "Open" ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxArtProvider::GetBitmapBundle( wxART_FILE_OPEN, wxART_TOOLBAR ), wxITEM_NORMAL, _( "Open" ), _( "Open Query" ) );
    m_tb->AddTool( wxID_SAVEQUERY, _( "Save" ), save, save, wxITEM_NORMAL, _( "Save" ), _( "Save Query" ) );
    m_tb->AddSeparator();
    m_tb->AddTool( wxID_CUTCOLUMN, _( "Cut" ), wxArtProvider::GetBitmapBundle( wxART_CUT ), wxArtProvider::GetBitmapBundle( wxART_CUT ), wxITEM_NORMAL, _( "Cut" ), _( "Cut Column" ) );
    m_tb->AddTool( wxID_COPYCOLUMN, _( "Copy" ), wxArtProvider::GetBitmapBundle( wxART_COPY ), wxArtProvider::GetBitmapBundle( wxART_COPY ), wxITEM_NORMAL, _( "Copy" ), _( "Copy Column" ) );
    m_tb->AddTool( wxID_PASTECOLUMN, _( "Paste" ), wxArtProvider::GetBitmapBundle( wxART_PASTE ), wxArtProvider::GetBitmapBundle( wxART_PASTE ), wxITEM_NORMAL, _( "Paste" ), _( "Paste Column" ) );
    m_tb->AddTool( wxID_DELETECOLUMN, _( "Delete Column" ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxArtProvider::GetBitmapBundle( wxART_DELETE ), wxITEM_NORMAL, _( "Delete Column" ), _( "Delete Column" ) );
    m_tb->AddSeparator();
    m_tb->AddTool( wxID_TABLEPROPERTIES, _( "Properties" ), properties, properties, wxITEM_NORMAL, _( "Properties" ), _( "Properties" ) );
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
    if( event.GetKeyCode() == WXK_DOWN )
    {
        if( m_grid->GetGridCursorRow() + 1 == m_grid->GetNumberRows() )
        {
        // range check happens inside method anyway
            AppendOrInsertField( nullptr );
        }
    }
    event.Skip();
}

void TableEditView::OnCellClicked(wxGridEvent &event)
{
    auto row = event.GetRow();
    if( row != m_grid->GetNewRow() )
    {
        m_grid->SetOldRow( m_grid->GetNewRow() );
        m_grid->SetNewRow( row );
        m_grid->GetGridRowLabelWindow()->Refresh();
    }
}

void TableEditView::AppendOrInsertField(TableField *it)
{
    std::wstring size, tempWidth, tempPrecision;
    wxString name = "", fieldType = "", nullAllowed = "No";
    size_t pos1, pos2;
    int width = 0, precision = 0;
    if( it )
    {
        name = it->GetFieldName();
        fieldType = it->GetFieldType();
        pos1 = fieldType.find( '(' ); 
        if( pos1 != std::wstring::npos )
        {
            auto gridtype = fieldType.substr( 0, pos1 );
            size = fieldType.substr( pos1 + 1 );
            pos2 = size.find( ',' );
            if( pos2 != std::wstring::npos )
            {
                tempWidth = size.substr( 0, pos2 );
                tempPrecision = size.substr( pos2 + 1, size.length() - 1 );
            }
            else
            {
                tempWidth = size.substr( 0, size.length() - 1 );
                tempPrecision = L"";
            }
            fieldType = gridtype;
        }
        else
        {
            pos1 = fieldType.find( L' ' );
            if( pos1 != std::string::npos )
                fieldType = fieldType.substr( 0, pos1 );
        }
        width = it->GetFieldSize();
        precision = it->GetPrecision();
        nullAllowed = it->IsNullAllowed() ? "Yes" : "No";
    }
    m_grid->AppendRows();
    int rows = m_grid->GetNumberRows();
    m_grid->SetCellValue( rows - 1, 0, name );
    m_grid->SetCellValue( rows - 1, 1, fieldType );
    m_grid->SetCellRenderer( rows - 1, 1, new MyComboCellRenderer );
    m_grid->SetCellEditor( rows - 1, 1, new MyTableTypeEditor( m_dbType, m_dbSubtype, fieldType ) );
    if( width > 0 )
        m_grid->SetCellValue( rows - 1, 2, wxString::Format( "%d", width ) );
    else if( !tempWidth.empty() )
        m_grid->SetCellValue( rows - 1, 2, tempWidth );                
    if( precision > 0 )
        m_grid->SetCellValue( rows - 1, 3, wxString::Format( "%d", precision ) );
    else if( !tempPrecision.empty() )
        m_grid->SetCellValue( rows - 1, 2, tempPrecision );
    const wxString nullChoices[] =
    {
        "Yes",
        "No"
    };
    m_grid->SetCellEditor( rows - 1, 4, new MyComboCellEditor( 2, nullChoices ) );
    m_grid->SetCellRenderer( rows - 1, 4, new MyComboCellRenderer );
    m_grid->SetCellValue( rows - 1, 4, nullAllowed );
    const wxString defValues[] = 
    {
        "[None]",
        "autoincrement"
    };
    m_grid->SetCellEditor( rows - 1, 5, new MyComboCellEditor( 2, defValues ) );
    m_grid->SetCellRenderer( rows - 1, 5, new MyComboCellRenderer );
    m_grid->SetCellValue( rows - 1, 5, "[None]" );
    m_grid->SetRowLabelValue( rows - 1, "" );
}

void TableEditView::OnInsertColumn(wxCommandEvent &event)
{
    AppendOrInsertField( nullptr );
    m_grid->GoToCell( m_grid->GetNumberRows() - 1, 0 );
    GetDocument()->Modify( true );
}

void TableEditView::OnDeleteColumn(wxCommandEvent &event)
{
    auto value = m_grid->GetCellValue( m_grid->GetGridCursorRow(), 0 );
    int answer = wxMessageBox( _( "You are about to drop column " ) + value + _( ". OK to continue?" ), _( "Create/Alter Table" ), wxOK | wxCANCEL | wxCANCEL_DEFAULT | wxICON_INFORMATION );
    if( answer == wxID_OK )
    {
        GetDocument()->Modify( true );
    }
}

void TableEditView::OnTableProperties(wxCommandEvent &event)
{
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
}
