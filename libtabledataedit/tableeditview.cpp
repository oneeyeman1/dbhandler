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
#include "typecombobox.h"
#include "database.h"
#include "tableattributes.h"
#include "tableeditdocument.h"
#include "tableeditview.h"
#include "tableeditcanvas.h"

#include "res/queryexec.xpm"
#include "res/querycancel.xpm"

typedef int (*TABLESELECTION)(wxDocMDIChildFrame *, Database *, std::map<wxString, std::vector<TableDefinition> > &, std::vector<std::wstring> &, bool, const int, bool);

wxIMPLEMENT_DYNAMIC_CLASS(TableEditView, wxView);

bool TableEditView::OnCreate(wxDocument *doc, long flags)
{
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
#elif __WXGTK__
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
    sizer = new wxBoxSizer( wxVERTICAL );
#ifdef __WXOSX__
    wxRect parentRect = m_parent->GetRect();
    wxSize parentClientSize = m_parent->GetClientSize();
    m_frame->Move( 0, parentRect.y - parentClientSize.y );
    wxPoint pt;
    pt.x = -1;
    pt.y = parentRect.height - parentClientSize.GetHeight();
    m_frame->SetSize( pt.x, pt.y, parentRect.GetWidth(), parentClientSize.GetHeight() );
    m_tb = m_frame->CreateToolBar();
    m_tb->AddTool( wxID_QUERYCANCEL, _( "Cancel" ), wxBitmap( querycancel ), _( "Cancel the query" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_TOP, _( "Top" ), wxArtProvider::GetBitmap( wxART_GOTO_FIRST ), _( "Top" ), wxITEM_NORMAL );
    m_tb->AddTool( wxID_BACK, _( "Back" ), wxArtProvider::GetBitmap( wxART_GO_BACK ), _( "Back" ), wxITEM_NORMAL );
    m_tb->Realize();
    int offset = m_tb->GetSize().y;
    ptCanvas.x = 0;
    ptCanvas.y = offset;
#else
    ptCanvas = wxDefaultPosition;
#endif
    CreateMenuAndToolbar();
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
    sizer->Layout();
    m_frame->Layout();
    m_frame->Show();
    return true;
}

TableEditView::~TableEditView()
{
}

void TableEditView::OnDraw(wxDC *dc)
{
}

void TableEditView::GetTablesForView(Database *db, bool init)
{
    std::map<wxString, std::vector<TableDefinition> > tables;
    std::vector<std::wstring> tableNames;
    int res = -1;
    wxString query, documentName = "";
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
    m_grid = new wxScrolled<wxPanel>( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxALWAYS_SHOW_SB | wxVSCROLL );
    sizer_2->Add( m_grid, 1, wxEXPAND, 0 );
    auto gridsizer = new wxFlexGridSizer( 6, 0, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Field Name" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Field Type" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Width" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Dec" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Null" ) ), 0, wxEXPAND, 0 );
    gridsizer->Add( new wxStaticText( m_grid, wxID_ANY, _( "Default Value" ) ), 0, wxEXPAND, 0 );
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
    if( table && !table->GetFields().empty() )
    {
        for( std::vector<TableField *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); ++it )
        {
            gridsizer->Add( new wxTextCtrl( m_grid, wxID_ANY, (*it)->GetFieldName() ) );
            gridsizer->Add( new TypeComboBox( m_grid, db->GetTableVector().m_type, db->GetTableVector().m_type, (*it)->GetFieldType() ) );
            gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, wxString::Format( "%d", (*it)->GetFieldSize() ) ) );
            gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, wxString::Format( "%d", (*it)->GetPrecision() ) ) );
            gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, (*it)->IsNullAllowed() ? "Yes" : "No" ) );
            gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, (*it)->GetDefaultValue(), wxDefaultPosition, wxDefaultSize, 8, defaultChoices  ) );
        }
    }
    else
    {
        gridsizer->Add( new wxTextCtrl( m_grid, wxID_ANY, "" ) );
        gridsizer->Add( new TypeComboBox( m_grid, db->GetTableVector().m_type, db->GetTableVector().m_type, L"" ) );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ) );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ) );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "Yes" ) );
        gridsizer->Add( new wxComboBox( m_grid, wxID_ANY, "" ) );
    }
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    attributes = new TableSettngs( m_panel, wxID_ANY );
    sizer_2->Add( attributes, 1, wxEXPAND, 0 );

    m_grid->SetSizer( gridsizer );
    m_panel->SetSizer( sizer_2 );
    m_frame->SetSizer( sizer_1 );
    m_frame->Layout();
}

void TableEditView::CreateMenuAndToolbar()
{
    int offset = 0, position = 0;
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
    position = dynamic_cast<wxDocMDIParentFrame *>( m_parent )->GetToolBar()->GetSize().GetHeight();
#else __WXGTK__
	parent = m_parent;
#endif
    wxSize size = m_parent->GetClientSize();
#ifdef __WXOSX__
    m_tb = m_frame->CreateToolBar();
    if( m_type == QueryView )
    {
        m_styleBar = new wxToolBar( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, style, "StyleBar" );
    }
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
