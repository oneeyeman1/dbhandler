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
#include <wx/artprov.h>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/config.h"
#include "wx/dynlib.h"
#include "wx/fswatcher.h"
#include "database.h"
#include "docview.h"
#include "newtablehandler.h"

#ifdef __WXGTK__
#include "query.h"
#include "odbc.h"
#include "res/profile.c"
#include "res/database.c"
#include "table.h"
#endif

typedef void (*ODBCSETUP)(wxWindow *);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, wxString &, std::vector<Profile> &);
typedef void (*DATABASE)(wxWindow *, wxDocManager *, Database *, ViewType, std::map<wxString, wxDynamicLibrary *> &, const std::vector<Profile> &);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef void (*DISCONNECTFROMDB)(void *, const wxString &);
typedef int (*ATTACHDATABASE)(wxWindow *, Database *);
typedef int (*DETACHDATABASE)(wxWindow *);

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_MENU(wxID_CONFIGUREODBC, MainFrame::OnConfigureODBC)
    EVT_MENU(wxID_DATABASEWINDOW, MainFrame::OnDatabaseProfile)
    EVT_MENU(wxID_TABLE, MainFrame::OnTable)
    EVT_MENU(wxID_DATABASE, MainFrame::OnDatabase)
    EVT_MENU(wxID_QUERY, MainFrame::OnQuery)
    EVT_MENU(wxID_ATTACHDATABASE, MainFrame::OnAttachDatabase)
    EVT_MENU(wxID_DETACHDATABASE, MainFrame::OnDetachDatabase)
    EVT_UPDATE_UI(wxID_DETACHDATABASE, MainFrame::OnUpdateUIDetachDB)
    EVT_SIZE(MainFrame::OnSize)
    EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager) : wxDocMDIParentFrame(manager, NULL, wxID_ANY, "DB Handler" )
{
    m_db = NULL;
    m_countAttached = 0;
    m_handler = NULL;
#if !( defined( __sun ) && defined( __SVR4 ) )
    m_oldPGWatcher = NULL;
#endif
#if defined __WXMSW__ || defined __WXGTK__
    m_tb = NULL;
#endif
    wxConfigBase *config = wxConfigBase::Get( "DBManager" );
    wxString path = config->GetPath();
    config->SetPath( "CurrentDB" );
    m_pgLogfile = config->Read( "Logfile", "" );
    config->SetPath( path );
    config->SetPath( "Profiles" );
    long counter;
    wxString profile;
    auto currentProfile = config->Read( "CurrentProfile", "" );
    auto res = config->GetFirstEntry( profile, counter );
    auto found = false;
    while( res )
    {
        auto prof = config->Read( profile, "" );
        if( prof == currentProfile || currentProfile.IsEmpty() )
        {
            m_profiles.push_back( Profile( prof, true ) );
            found = true;
        }
        else
            m_profiles.push_back( Profile( prof, false ) );
        res = config->GetNextEntry( profile, counter );
    }
    if( !found && !currentProfile.IsEmpty() )
        m_profiles.push_back( Profile( currentProfile, true ) );
    config->SetPath( path );
    m_manager = manager;
    auto menuFile = new wxMenu;
    menuFile->Append( wxID_NEW );
    menuFile->Append( wxID_OPEN );
    menuFile->AppendSeparator();
    menuFile->Append( wxID_EXIT );
    wxMenuBar *menubar = new wxMenuBar;
    menubar->Append( menuFile, wxGetStockLabel( wxID_FILE ) );
    wxMenu *help= new wxMenu;
    help->Append( wxID_ABOUT );
    menubar->Append( help, wxGetStockLabel( wxID_HELP ) );
    SetMenuBar( menubar );
    CreateStatusBar();
    CreateToolBar( wxNO_BORDER | wxTB_FLAT | wxTB_HORIZONTAL );
    InitToolBar( GetToolBar() );
}

MainFrame::~MainFrame()
{
    std::vector<std::wstring> errorMsg;
    int result = 0;
    wxConfigBase *config = wxConfigBase::Get( "DBManager" );
    wxString path = config->GetPath();
    config->SetPath( "CurrentDB" );
    if( m_db )
    {
        wxString temp1( m_db->GetTableVector().m_type );
        config->Write( "Engine", temp1 );
        wxString temp2 = m_db->GetTableVector().m_subtype;
        config->Write( "Subtype", temp2 );
        if( temp2 == "PostgreSQL" )
        {
            wxString temp3( m_db->GetTableVector().GetPostgreLogFile() );
            config->Write( "Logfile", temp3 );
        }
        wxString currentProfile;
        temp1 = m_db->GetTableVector().m_dbName;
        config->Write( "DatabaseName", temp1 );
        if( m_db->GetTableVector().m_type == "SQLite" )
#ifdef __WXMSW__
            currentProfile = temp1.Mid( temp1.find_last_of( '\\' ) + 1 );
#else
            currentProfile = temp1.Mid( temp1.find_last_of( '/' ) + 1 );
#endif
        config->SetPath( path );
        config->SetPath( "Profiles" );
        auto found = false;
        long counter;
        wxString profile;
        auto res = config->GetFirstEntry( profile, counter );
        while( res && !found )
        {
            auto prof = config->Read( profile, "" );
            if( prof == currentProfile )
                found = true;
            res = config->GetNextEntry( profile, counter );
        }
        if( !found )
            config->Write( wxString::Format( "Profile%ld", counter ), currentProfile );
        config->SetPath( path );
        std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
        result = m_db->Disconnect( errorMsg );
    }
    if( result )
    {
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
    }
    delete m_db;
    m_db = NULL;
    for( std::map<wxString, wxDynamicLibrary *>::iterator it = m_painters.begin(); it != m_painters.end(); ++it )
    {
        delete (*it).second;
        (*it).second = NULL;
    }
#if !( defined( __sun ) && defined( __SVR4 ) )
    delete m_oldPGWatcher;
    m_oldPGWatcher = NULL;
#endif
}

void MainFrame::OnClose(wxCloseEvent &WXUNUSED(event))
{
    if( m_db )
    {
        {
            std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
            if( m_handler )
            {
                if( m_handler->Delete() != wxTHREAD_NO_ERROR )
                {
                }
            }
        }
        while( 1 )
        {
            {
                std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
                if( !m_handler )
                    break;
            }
            wxThread::This()->Sleep( 1 );
        }
    }
    Destroy();
}

void MainFrame::InitToolBar(wxToolBar* toolBar)
{
    wxVector<wxBitmap> bitmaps[9];

    bitmaps[2].push_back( wxBITMAP_PNG( profile_16x16 ) );
    bitmaps[2].push_back( wxBITMAP_PNG( profile_32x32 ) );
    bitmaps[2].push_back( wxBITMAP_PNG( profile_64x64 ) );

    bitmaps[4].push_back( wxBITMAP_PNG( database_16x16 ) );
    bitmaps[4].push_back( wxBITMAP_PNG( database_32x32 ) );
    bitmaps[4].push_back( wxBITMAP_PNG( database_64x64 ) );
#ifdef __WXGTK__
    toolBar->AddTool( wxID_QUERY, _( "Query" ), wxBitmapBundle::FromSVG( query, wxSize( 16, 16 ) ) );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), wxBitmapBundle::FromSVG( odbc, wxSize( 16, 16 ) ) );
#else
    toolBar->AddTool( wxID_QUERY, _( "Query" ), wxBitmapBundle::FromSVGResource( "query", wxSize( 16, 16 ) ) );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), wxBitmapBundle::FromSVGResource( "odbc", wxSize( 16, 16 ) ) );
#endif
    toolBar->AddTool( wxID_DATABASEWINDOW, _( "Profile" ), wxBitmapBundle::FromBitmaps( bitmaps[2] ) );
#ifdef __WXGTK__
    toolBar->AddTool( wxID_TABLE, _( "Table" ), wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) ) );
#else
    toolBar->AddTool( wxID_TABLE, _( "Table" ), wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) ) );
#endif
    toolBar->AddTool( wxID_DATABASE, _( "Database" ), wxBitmapBundle::FromBitmaps( bitmaps[4] ) );
    toolBar->AddTool( wxID_EXIT, _( "Exit the application" ), wxArtProvider::GetBitmapBundle( wxART_QUIT, wxART_TOOLBAR ), wxBitmapBundle(), wxITEM_NORMAL, _( "Quit" ), _( "Quit the application" ) );
    toolBar->SetName( "PowerBar" );
    toolBar->Realize();
}

void MainFrame::Connect()
{
    Database *db = NULL;
    wxString title;
    std::vector<std::wstring> errorMsg;
    wxDynamicLibrary *lib = NULL;
    if( m_painters.find( "DBloader" ) == m_painters.end() )
    {
        bool loaded;
        lib = new wxDynamicLibrary();
#ifdef __WXMSW__
        loaded = lib->Load( "dbloader" );
#elif __WXMAC__
        loaded = lib->Load( "liblibdbloader.dylib" );
#else
        loaded = lib->Load( "libdbloader" );
#endif
        if( loaded )
            m_painters["DBloader"] = lib;
        else
        {
            delete lib;
            lib = nullptr;
        }
    }
    else
        lib = m_painters ["DBloader"];
    if( lib && lib->IsLoaded() )
    {
        DBPROFILE func = (DBPROFILE) lib->GetSymbol( "ConnectToDb" );
        wxString name = wxGetApp().GetDBName();
        wxString engine = wxGetApp().GetDBEngine();
        wxString connectStr = wxGetApp().GetConnectString();
        wxString connectedUser = wxGetApp().GetConnectedUser();
        db = func( this, name, engine, connectStr, connectedUser, m_profiles );
        if( db && m_db )
        {
            {
                std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
                if( m_handler )
                {
                    if( m_handler->Delete() != wxTHREAD_NO_ERROR )
                    {
                    }
                }
            }
            while( 1 )
            {
                {
                    std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
                    if( !m_handler )
                        break;
                }
                wxThread::This()->Sleep( 1 );
            }
#if !( defined( __sun ) && defined( __SVR4 ) )
            delete m_oldPGWatcher;
            m_oldPGWatcher = NULL;
#endif
            delete m_db;
            m_db = NULL;
            wxGetApp().SetDBEngine( engine );
            wxGetApp().SetDBName( name );
            wxGetApp().SetConnectString( connectStr );
            wxGetApp().SetConnectedUser( connectedUser );
        }
        if( db && db != m_db )
        {
            m_db = db;
            auto major = wxString::Format( "%lu", m_db->GetTableVector().m_versionMajor );
            auto minor = wxString::Format( "%lu", m_db->GetTableVector().m_versionMinor );
            auto revision = wxString::Format( "%lu", m_db->GetTableVector().m_versionRevision );
            auto clientMajor = wxString::Format( "%lu", m_db->GetTableVector().m_clientVersionMajor );
            auto clientMinor = wxString::Format( "%lu", m_db->GetTableVector().m_clientVersionMinor );
            auto clientRevision = wxString::Format( "%lu", m_db->GetTableVector().m_clientVersionRevision );
            if( m_db && m_db->GetTableVector().m_type == L"ODBC" )
                title = "Connected to " + m_db->GetTableVector().m_subtype + " version " + m_db->GetTableVector().m_serverVersion + " thru the ODBC";
            else if( m_db && m_db->GetTableVector().m_type == L"SQLite" )
                title = wxString::Format( "Connected to " + m_db->GetTableVector().m_type + " version " + major + "." + minor + "." + revision );
            else if( m_db )
                title = wxString::Format( "Connected to " + m_db->GetTableVector().m_type + " version " + major + "." + minor + "." + revision + " with the client library " + clientMajor + "." + clientMinor + "." + clientRevision );
            SetTitle( title );
            if( m_db )
            {
                m_handler = new NewTableHandler( this, m_db );
                if( m_handler->Run() != wxTHREAD_NO_ERROR )
                {
                    wxMessageBox( _( "Internal error. Try to clean some memory and try again!" ) );
                    delete m_handler;
                    m_handler = NULL;
                    m_db->Disconnect( errorMsg );
                    delete m_db;
                    m_db = NULL;
                }
            }
        }
    }
}

void MainFrame::OnConfigureODBC(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary *lib;
    if( m_painters.find( "dialogs" ) == m_painters.end() )
    {
        lib = new wxDynamicLibrary();
#ifdef __WXMSW__
        lib->Load( "dialogs" );
#elif __WXMAC__
        lib->Load( "liblibdialogs.dylib" );
#else
        lib->Load( "libdialogs" );
#endif
        if( lib->IsLoaded() )
            m_painters["dialogs"] = lib;
        else
        {
            delete lib;
            lib = nullptr;
        }
    }
    else
        lib = m_painters ["dialogs"];
    if( lib && lib->IsLoaded() )
    {
        ODBCSETUP func = (ODBCSETUP) lib->GetSymbol( "ODBCSetup" );
        func( this );
    }
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void MainFrame::OnDatabase(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary *lib = nullptr, *lib1 = nullptr, *lib2 = nullptr;
    if( !m_db )
        Connect();
    if( m_db )
    {
        if( m_painters.find( "Database" ) == m_painters.end() )
        {
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            lib->Load( "dbwindow" );
#elif __WXOSX__
            lib->Load( "liblibdbwindow.dylib" );
#else
            lib->Load( "libdbwindow" );
#endif
            if( lib->IsLoaded() )
                m_painters["Database"] = lib;
            else
            {
                delete lib;
                lib = nullptr;
            }
        }
        else
            lib = m_painters["Database"];
        if( m_painters.find( "EditData" ) == m_painters.end() )
        {
            lib1 = new wxDynamicLibrary;
#ifdef __WXMSW__
            lib1->Load( "tabledataedit" );
#elif __WXOSX__
            lib1->Load( "liblibtabledataedit.dylib" );
#else
            lib1->Load( "libtableedit" );
#endif
            if( lib1->IsLoaded() )
                m_painters["EditData"] = lib1;
            else
            {
                delete lib1;
                lib1 = nullptr;
            }
        }
        else
            lib1 = m_painters["EditData"];
        if( m_painters.find( "TableView" ) == m_painters.end() )
        {
            lib2 = new wxDynamicLibrary;
#ifdef __WXMSW__
            lib2->Load( "tabledataedit" );
#elif __WXOSX__
            lib2->Load( "liblibtabledataedit.dylib" );
#else
            lib2->Load( "libtabledataedit" );
#endif
            if( lib2->IsLoaded() )
                m_painters["TableView"] = lib2;
            else
            {
                delete lib2;
                lib2 = nullptr;
            }
        }
        else
            lib2 = m_painters["TableView"];
        if( m_db && lib->IsLoaded() )
        {
            DATABASE func = (DATABASE) lib->GetSymbol( "CreateDatabaseWindow" );
            func( this, m_manager, m_db, DatabaseView, m_painters, m_profiles );
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
    }
}

void MainFrame::OnQuery(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect();
    if( m_db )
    {
        if( m_painters.find( "Query" ) == m_painters.end() )
        {
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            lib->Load("dbwindow");
#elif __WXOSX__
            lib->Load("liblibdbwindow.dylib");
#else
            lib->Load("libdbwindow");
#endif
            if( lib->IsLoaded() )
                m_painters["Query"] = lib;
            else
            {
                delete lib;
                lib = nullptr;
            }
        }
        else
            lib = m_painters["Query"];
        if( m_db && lib->IsLoaded() )
        {
            DATABASE func = (DATABASE) lib->GetSymbol( "CreateDatabaseWindow" );
            func( this, m_manager, m_db, QueryView, m_painters, m_profiles );
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
	}
}

void MainFrame::OnDatabaseProfile(wxCommandEvent &WXUNUSED(event))
{
    Connect();
}

void MainFrame::OnTable(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect();
    if( m_db )
    {
        if( m_painters.find( "TableView" ) == m_painters.end() )
        {
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            lib->Load( "tabledataedit" );
#elif __WXOSX__
            lib->Load( "liblibtabledataedit.dylib" );
#else
            lib->Load( "libtabledataedit" );
#endif
            if( lib->IsLoaded() )
                m_painters["TableView"] = lib;
            else
            {
                delete lib;
                lib = nullptr;
            }
        }
        else
            lib = m_painters["TableView"];
        if( m_db && lib->IsLoaded() )
        {
            TABLE func = (TABLE) lib->GetSymbol( "CreateDatabaseWindow" );
            func( this, m_manager, m_db, NULL, wxEmptyString );                 // create with possible alteration table
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
    }
}

void MainFrame::OnSize(wxSizeEvent &event)
{
    wxSize size = GetClientSize();
    int offset = 0;
    bool foundTb = false;
    wxMDIClientWindow *child = NULL;
    wxDocMDIChildFrame *frame = NULL;
    wxToolBar *tb = NULL;
    for( wxWindowList::compatibility_iterator it = GetChildren().GetFirst(); it; it = it->GetNext() )
    {
        wxWindow *current = it->GetData();
        if( !foundTb )
        {
            tb = dynamic_cast<wxToolBar *>( current );
            if( tb && tb->GetName() == "ViewBar" )
                foundTb = true;
        }
    }
    wxView *currentView = wxGetApp().GetDocManager()->GetCurrentView();
    if( currentView )
        frame = (wxDocMDIChildFrame *) currentView->GetFrame();
    if( foundTb )
    {
        child = (wxMDIClientWindow *) GetClientWindow();
        tb->SetSize( 0, 0, size.x, wxDefaultCoord );
        offset = tb->GetSize().y;
        child->SetSize( 0, offset, size.x, size.y - offset );
        if( frame )
            frame->SetSize( 0, 0, size.x, size.y - offset - 2 );
    }
    else if( !tb || tb->GetName() == "PowerBar" )
        event.Skip();
}

void MainFrame::OnAttachDatabase(wxCommandEvent &WXUNUSED(event))
{
    auto lib = new wxDynamicLibrary;
#ifdef __WXMSW__
    lib->Load( "dialogs" );
#elif __WXOSX__
    lib->Load( "liblibdialogs.dylib" );
#else
    lib->Load( "libdialogs" );
#endif
    ATTACHDATABASE func = (ATTACHDATABASE) lib->GetSymbol( "AttachToDatabase" );
    int result = func( this, m_db );
    if( result == wxID_OK )
        m_countAttached++;
    delete lib;
    lib = nullptr;
}

void MainFrame::OnDetachDatabase(wxCommandEvent &WXUNUSED(event))
{
    auto lib = new wxDynamicLibrary;
#ifdef __WXMSW__
    lib->Load( "dialogs" );
#elif __WXOSX__
    lib->Load( "liblibdialogs.dylib" );
#else
    lib->Load( "libdialogs" );
#endif
    DETACHDATABASE func = (DETACHDATABASE) lib->GetSymbol( "DetachDatabase" );
    int result = func( this );
    if( result == wxID_OK )
        m_countAttached--;
    delete lib;
    lib = nullptr;
}

void MainFrame::OnUpdateUIDetachDB(wxUpdateUIEvent &event)
{
    if( m_countAttached > 0 )
        event.Enable( true );
    else
        event.Enable( false );
}
