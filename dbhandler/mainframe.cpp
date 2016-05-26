/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#include <vector>
#include <map>
#include "wx/docview.h"
#include "wx/config.h"
#include "wx/dynlib.h"
#include "database.h"
#include "docview.h"
#include "mainframe.h"

#include "res/odbc1.xpm"
#include "res/database.xpm"

typedef void (*ODBCSETUP)(wxWindow *);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &);
typedef void (*DISCONNECTFROMDB)(void *, const wxString &);

BEGIN_EVENT_TABLE(MainFrame, wxDocParentFrame)
    EVT_MENU(wxID_CONFIGUREODBC, MainFrame::OnConfigureODBC)
    EVT_MENU(wxID_DATABASEWINDOW, MainFrame::OnDatabaseProfile)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager) : wxDocParentFrame(manager, NULL, wxID_ANY, "DB Handler" )
{
    m_db = NULL;
    m_lib = NULL;
    wxMenu *menuFile = new wxMenu;
    menuFile->Append( wxID_NEW );
    menuFile->Append( wxID_OPEN );
    menuFile->AppendSeparator();
    // A nice touch: a history of files visited. Use this menu.
    manager->FileHistoryUseMenu( menuFile );
#if wxUSE_CONFIG
    manager->FileHistoryLoad( *wxConfig::Get() );
#endif // wxUSE_CONFIG
    menuFile->AppendSeparator();
    menuFile->Append( wxID_EXIT );
    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append( menuFile, wxGetStockLabel( wxID_FILE ) );
    wxMenu *help= new wxMenu;
    help->Append( wxID_ABOUT );
    menubar->Append( help, wxGetStockLabel( wxID_HELP ) );
    SetMenuBar( menubar );
#if wxUSE_STATUSBAR
    CreateStatusBar();
#endif // wxUSE_STATUSBAR
#if wxUSE_TOOLBAR
    CreateToolBar( wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL );
    InitToolBar( GetToolBar() );
#endif // wxUSE_TOOLBAR
}

MainFrame::~MainFrame()
{
    delete m_db;
    m_db = NULL;
}

#if wxUSE_TOOLBAR
void MainFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap bitmaps[9];
    bitmaps[0] = wxBitmap( odbc1 );
    bitmaps[1] = wxBitmap( database );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), bitmaps[0], bitmaps[0], wxITEM_NORMAL, _( "Configure ODBC" ), _( "Configure ODBC data source" ) );
    toolBar->AddTool( wxID_DATABASEWINDOW, _( "Database" ), bitmaps[1], bitmaps[1], wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
    toolBar->Realize();
}
#endif

void MainFrame::OnConfigureODBC(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary lib;
#ifdef __WXMSW__
	lib.Load( "dialogs" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        ODBCSETUP func = (ODBCSETUP) lib.GetSymbol( "ODBCSetup" );
        func( this );
    }
}

void MainFrame::OnDatabaseProfile(wxCommandEvent &WXUNUSED(event))
{
    Database *db = NULL;
    if( !m_lib )
    {
        m_lib = new wxDynamicLibrary();
#ifdef __WXMSW__
        m_lib->Load( "dbloader" );
#else
        m_lib->Load( "libdbloader" );
#endif
    }
    if( m_lib->IsLoaded() )
    {
        DBPROFILE func = (DBPROFILE) m_lib->GetSymbol( "ConnectToDb" );
        wxString name = wxGetApp().GetDBName();
        wxString engine = wxGetApp().GetDBEngine();
        db = func( this, name, engine );
        if( db )
        {
            delete m_db;
            m_db = NULL;
            wxGetApp().SetDBEngine( engine );
			wxGetApp().SetDBName( name );
        }
        m_db = db;
    }
}
