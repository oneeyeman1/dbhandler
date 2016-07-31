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
#include "wx/docmdi.h"
#include "wx/config.h"
#include "wx/dynlib.h"
#include "database.h"
#include "docview.h"
#include "mainframe.h"

#include "res/odbc1.xpm"
#include "res/database_profile.xpm"
#include "res/database.xpm"

typedef void (*ODBCSETUP)(wxWindow *);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &);
typedef void (*DATABASE)(wxWindow *, wxDocManager *, Database *);
typedef void (*DISCONNECTFROMDB)(void *, const wxString &);

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_MENU(wxID_CONFIGUREODBC, MainFrame::OnConfigureODBC)
    EVT_MENU(wxID_DATABASEWINDOW, MainFrame::OnDatabaseProfile)
    EVT_MENU(wxID_DATABASE, MainFrame::OnDatabase)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager) : wxDocMDIParentFrame(manager, NULL, wxID_ANY, "DB Handler" )
{
    m_db = NULL;
    m_lib = m_lib1 = NULL;
    m_manager = manager;
    m_menuFile = new wxMenu;
    m_menuFile->Append( wxID_NEW );
    m_menuFile->Append( wxID_OPEN );
    m_menuFile->AppendSeparator();
    m_menuFile->Append( wxID_EXIT );
    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append( m_menuFile, wxGetStockLabel( wxID_FILE ) );
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
    std::vector<std::wstring> errorMsg;
    int result = 0;
    if( m_db )
        result = m_db->Disconnect( errorMsg );
    if( result )
    {
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
    }
    delete m_db;
    m_db = NULL;
    delete m_lib;
    m_lib = NULL;
    delete m_lib1;
    m_lib1 = NULL;
}

#if wxUSE_TOOLBAR
void MainFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap bitmaps[9];
    bitmaps[0] = wxBitmap( odbc1 );
    bitmaps[1] = wxBitmap( database_profile );
    bitmaps[2] = wxBitmap( database );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), bitmaps[0], bitmaps[0], wxITEM_NORMAL, _( "Configure ODBC" ), _( "Configure ODBC data source" ) );
    toolBar->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), bitmaps[1], bitmaps[1], wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
    toolBar->AddTool( wxID_DATABASE, _( "Database" ), bitmaps[2], bitmaps[2], wxITEM_NORMAL, _( "Database" ), _( "Database" ) );
    toolBar->Realize();
}
#endif

void MainFrame::InitMenuBar(int id)
{
    m_menuFile->Delete( wxID_NEW );
    m_menuFile->Delete( wxID_OPEN );
    m_menuFile->Insert( 0, wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    switch( id )
    {
        case wxID_DATABASE:
            DatabaseMenu();
            break;
    }
}

void MainFrame::DatabaseMenu()
{
    m_menuFile->Insert( 2, wxID_CREATEDATABASE, _( "Create Database..." ), _( "Create Database" ) );
    m_menuFile->Insert( 3, wxID_DELETEDATABASE, _( "Delete Database..." ), _( "Delete Database" ) );
    m_menuFile->InsertSeparator( 4 );
}

void MainFrame::Connect()
{
    Database *db = NULL;
    if( !m_lib )
    {
        m_lib = new wxDynamicLibrary();
#ifdef __WXMSW__
        m_lib->Load( "dbloader" );
#elif __WXMAC__
        m_lib->Load( "liblibdbloader.dylib" );
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

void MainFrame::OnConfigureODBC(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary lib;
#ifdef __WXMSW__
	lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        ODBCSETUP func = (ODBCSETUP) lib.GetSymbol( "ODBCSetup" );
        func( this );
    }
}

void MainFrame::OnDatabase(wxCommandEvent &event)
{
    InitMenuBar( event.GetId() );
    if( !m_db )
        Connect();
    m_lib1 = new wxDynamicLibrary;
#ifdef __WXMSW__
    m_lib1->Load( "dbview" );
#elif __WXOSX__
    m_lib1->Load( "liblibdatabaseview.dylib" );
#else
    m_lib1->Load( "libdatabaseview" );
#endif
    if( m_db && m_lib1->IsLoaded() )
    {
        DATABASE func = (DATABASE) m_lib1->GetSymbol( "CreateDatabaseWindow" );
        func( this, m_manager, m_db );
    }
    else if( m_db )
        wxMessageBox( "Error loading the library. Please re-install the software and try again." );
    else
        wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
}

void MainFrame::OnDatabaseProfile(wxCommandEvent &WXUNUSED(event))
{
    Connect();
}
