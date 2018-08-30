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
#include "wx/fswatcher.h"
#include "database.h"
#include "docview.h"
#include "newtablehandler.h"

#include "res/odbc1.xpm"
#include "res/database_profile.xpm"
#include "res/database.xpm"
#include "res/table.xpm"
#include "res/properties.xpm"
#include "key-f1.xpm"
#include "res/query.xpm"
#include "quit.xpm"

typedef void (*ODBCSETUP)(wxWindow *);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, wxString &);
#if defined __WXMSW__ && _MSC_VER < 1900
typedef void (*DATABASE)(wxWindow *, wxDocManager *, Database *, ViewType, wxCriticalSection &);
#else
typedef void (*DATABASE)(wxWindow *, wxDocManager *, Database *, ViewType);
#endif
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, DatabaseTable *, const wxString &);
typedef void (*DISCONNECTFROMDB)(void *, const wxString &);

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_MENU(wxID_CONFIGUREODBC, MainFrame::OnConfigureODBC)
    EVT_MENU(wxID_DATABASEWINDOW, MainFrame::OnDatabaseProfile)
    EVT_MENU(wxID_TABLE, MainFrame::OnTable)
    EVT_MENU(wxID_DATABASE, MainFrame::OnDatabase)
    EVT_MENU(wxID_QUERY, MainFrame::OnQuery)
    EVT_SIZE(MainFrame::OnSize)
    EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager) : wxDocMDIParentFrame(manager, NULL, wxID_ANY, "DB Handler" )
{
    m_db = NULL;
    m_handler = NULL;
    m_oldPGWatcher = NULL;
#if defined __WXMSW__ || defined __WXGTK__
    m_tb = NULL;
#endif
    m_lib = NULL;
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
        wxString temp( m_db->GetTableVector().m_type );
        config->Write( "Engine", temp );
        temp = m_db->GetTableVector().m_subtype;
        config->Write( "Subtype", temp );
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker enter( m_threadCS );
#else
        std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
#endif
        result = m_db->Disconnect( errorMsg );
    }
    if( result )
    {
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
    }
    for( std::map<wxString, wxDynamicLibrary *>::iterator it = m_painters.begin(); it != m_painters.end(); it++ )
    {
        delete m_painters[(*it).first];
        m_painters[(*it).first] = NULL;
    }
    delete m_db;
    m_db = NULL;
    delete m_lib;
    m_lib = NULL;
}

void MainFrame::OnClose(wxCloseEvent &event)
{
    {
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker enter( m_threadCS );
#else
        std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
#endif
        if( m_handler )
        {
            if( m_handler->Delete() != wxTHREAD_NO_ERROR )
            {
            }
            m_handler = NULL;
        }
    }
    while( 1 )
    {
        {
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker enter( m_threadCS );
#else
        std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
#endif
            if( !m_handler )
                break;
        }
        wxThread::This()->Sleep( 1 );
    }
    Destroy();
}

void MainFrame::InitToolBar(wxToolBar* toolBar)
{
    wxBitmap bitmaps[9];
    bitmaps[0] = wxBitmap( query );
    bitmaps[1] = wxBitmap( odbc1 );
    bitmaps[2] = wxBitmap( database_profile );
    bitmaps[3] = wxBitmap( table );
    bitmaps[4] = wxBitmap( database );
    bitmaps[5] = wxBitmap( quit_xpm );
    toolBar->AddTool( wxID_QUERY, _( "Query" ), bitmaps[0], bitmaps[0], wxITEM_NORMAL, _( "Run Query" ), _( "Run Query Wizard" ) );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), bitmaps[1], bitmaps[1], wxITEM_NORMAL, _( "Configure ODBC" ), _( "Configure ODBC data source" ) );
    toolBar->AddTool( wxID_DATABASEWINDOW, _( "Database Profile" ), bitmaps[2], bitmaps[2], wxITEM_NORMAL, _( "DB Profile" ), _( "Select database profile" ) );
    toolBar->AddTool( wxID_TABLE, _( "Table" ), bitmaps[3], bitmaps[3], wxITEM_NORMAL, _( "Table" ), _( "Run Table View" ) );
    toolBar->AddTool( wxID_DATABASE, _( "Database" ), bitmaps[4], bitmaps[4], wxITEM_NORMAL, _( "Database" ), _( "Database" ) );
    toolBar->AddTool( wxID_EXIT, _( "Exit the application" ), bitmaps[5], bitmaps[5], wxITEM_NORMAL, _( "Quit" ), _( "Quit the application" ) );
    toolBar->SetName( "PowerBar" );
    toolBar->Realize();
}

void MainFrame::InitMenuBar(int id)
{
    wxPoint pt;
    wxSize size;
    wxMenuBar *mbar = GetMenuBar();
    for( size_t i = 1; i < mbar->GetMenuCount() - 1; i++ )
        mbar->Remove( i );
    if( m_menuFile->FindItem( wxID_NEW ) )
        m_menuFile->Delete( wxID_NEW );
    if( m_menuFile->FindItem( wxID_OPEN ) )
        m_menuFile->Delete( wxID_OPEN );
    m_menuFile->Insert( 0, wxID_CLOSE, _( "&Close\tCtrl+W" ), _( "Close Database Window" ) );
    switch( id )
    {
        case wxID_DATABASE:
            DatabaseMenu();
            break;
        case wxID_TABLE:
            TableMenu();
            break;
        case wxID_QUERY:
            QueryMenu();
            break;
    }
}

void MainFrame::DatabaseMenu()
{
    m_menuFile->Insert( 2, wxID_CREATEDATABASE, _( "Create Database..." ), _( "Create Database" ) );
    m_menuFile->Insert( 3, wxID_DELETEDATABASE, _( "Delete Database..." ), _( "Delete Database" ) );
    m_menuFile->InsertSeparator( 4 );
    wxMenu *menuObject = new wxMenu();
    menuObject->Append( wxID_SELECTTABLE, _( "Select Table..." ), _( "Select tables" ) );
    wxMenu *menuNewObject = new wxMenu();
    menuNewObject->Append( wxID_OBJECTNEWTABLE, _( "Table..." ), _( "New Table" ) );
    menuNewObject->Append( wxID_OBJECTNEWINDEX, _( "Index..." ), _( "New Index" ) );
    menuNewObject->Append( wxID_OBJECTNEWVIEW, _( "View" ), _( "New View" ) );
    menuNewObject->Append( wxID_OBJECTNEWFF, _( "Foreign Key..." ), _( "New Foreign Key" ) );
    menuObject->AppendSubMenu( menuNewObject, _( "New" ), _( "New Object" ) );
    menuObject->Append( wxID_DROPOBJECT, _( "Drop" ), _( "Drop database object" ) );
    menuObject->AppendSeparator();
    menuObject->Append( wxID_PROPERTIES, _( "Properties..." ), _( "Properties" ) );
    GetMenuBar()->Insert( 1, menuObject, _( "&Object" ) );
    wxMenu *menuDesign = new wxMenu();
    menuDesign->Append( wxID_STARTLOG, _( "Start Log" ), _( "Start log" ) );
    menuDesign->Append( wxID_STOPLOG, _( "Stop Log" ), _( "Stop log" ) );
    menuDesign->Append( wxID_SAVELOG, _( "Save Log As..." ), _( "Save log to disk file" ) );
    menuDesign->Append( wxID_CLEARLOG, _( "Clear Log" ), _( "Discard content of the log" ) );
    menuDesign->AppendSeparator();
    GetMenuBar()->Insert( 2, menuDesign, _( "&Design" ) );
}

void MainFrame::QueryMenu()
{
    wxMenu *designMenu = new wxMenu;
    designMenu->Append( wxID_UNDOALL, _( "Undo All" ), _( "Undo All" ) );
    designMenu->AppendSeparator();
    designMenu->AppendCheckItem( wxID_DATASOURCE, _( "Data Source" ), _( "Data Source" ) );
    designMenu->Append( wxID_PREVIEW, _( "Preview" ), _( "Preview" ) );
    designMenu->AppendSeparator();
    designMenu->Append( wxID_SELECTTABLE, _( "Select Tables..." ), _( "Select additional tables for query" ) );
    designMenu->Append( wxID_ARRANGETABLES, _( "Arrange Tables" ), _( "Arrange Tables" ) );
    designMenu->Append( wxID_UNIONS, _( "Unions..." ), _( "Unions" ) );
    designMenu->Append( wxID_RETRIEVEARGS, _( "Retrieval Arguments..." ), _( "Define Retrieval Arguments" ) );
    designMenu->Append( wxID_CHECKOPTION, _( "Check Option" ), _( "Check Option" ) );
    designMenu->AppendCheckItem( wxID_DISTINCT, _( "Distinct" ), _( "Use Distinct in query" ) );
    designMenu->AppendSeparator();
    designMenu->Append( wxID_CONVERTTOSYNTAX, _( "Convert to Syntax" ), _( "Convert to Syntax" ) );
    designMenu->AppendSeparator();
    wxMenu *showMenu = new wxMenu;
    designMenu->AppendSubMenu( showMenu, _( "Show" ), _( "Show" ) );
    designMenu->AppendSeparator();
    designMenu->Append( wxID_CUSTOMCOLORS, _( "Custom Colors..." ), _( "Define custom coloring scheme" ) );
    designMenu->Append( wxID_OPTIONS, _( "Options..." ), _( "Query view options" ) );
    GetMenuBar()->Insert( 1, designMenu, _( "Design" ) );
}

void MainFrame::TableMenu()
{
    wxMenu *edit_menu = new wxMenu;
    edit_menu->Append( wxID_UNDO, _( "&Undo\tCtrl+Z" ), _( "Undo operation" ) );
    edit_menu->AppendSeparator();
    edit_menu->Append( wxID_EDITCUTCOLUMN, _( "Cu&t Column" ), _( "Cut Column" ) );
    edit_menu->Append( wxID_EDITCOPYCOLUMN, _( "&Copy Column" ), _( "Copy Column" ) );
    edit_menu->Append( wxID_EDITPASTECOLUMN, _( "&Paste Column" ), _( "Paste Column" ) );
    edit_menu->Append( wxID_EDITINSERTCOLUMN, _( "Insert Column" ), _( "Insert Column" ) );
    edit_menu->Append( wxID_EDITDELETECOLUMN, _( "Delete Column" ), _( "Delete Column" ) );
    edit_menu->AppendSeparator();
    edit_menu->Append( wxID_EDITTABLEPROPERTY, _( "Table Property" ), _( "Table Property" ) );
    GetMenuBar()->Insert( 1, edit_menu, _( "Edit" ) );
    wxMenu *design_menu = new wxMenu;
    design_menu->Append( wxID_DESIGNSYNTAX, _( "Syntax" ), _( "Syntax" ) );
    GetMenuBar()->Insert( 2, design_menu, _( "Design" ) );
}

void MainFrame::Connect()
{
    Database *db = NULL;
    wxString title;
    std::vector<std::wstring> errorMsg;
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
        wxString connectStr = wxGetApp().GetConnectString();
        wxString connectedUser = wxGetApp().GetConnectedUser();
        db = func( this, name, engine, connectStr, connectedUser );
        if( db )
        {
            {
#if defined __WXMSW__ && _MSC_VER < 1900
                wxCriticalSectionLocker enter( m_threadCS );
#else
                std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
#endif
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
#if defined __WXMSW__ && _MSC_VER < 1900
                    wxCriticalSectionLocker enter( m_threadCS );
#else
                    std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
#endif
                    if( !m_handler )
                        break;
                }
                wxThread::This()->Sleep( 1 );
            }
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
            if( m_db && m_db->GetTableVector().m_type == L"ODBC" )
                title = "Connected to " + m_db->GetTableVector().m_subtype + " version " + m_db->GetTableVector().m_serverVersion + " thru the ODBC";
            else if( m_db )
                title = "Connected to " + m_db->GetTableVector().m_type + " version " + m_db->GetTableVector().m_serverVersion;
            SetTitle( title );
            if( ( ( db->GetTableVector().m_type == L"ODBC" && db->GetTableVector().m_subtype == L"PostgreSQL" ) || db->GetTableVector().m_type == L"PostgreSQL" ) && db->GetTableVector().m_versionMajor <= 9 && db->GetTableVector().m_versionMinor <= 3 )
            {
                m_oldPGWatcher = new wxFileSystemWatcher;
                m_oldPGWatcher->SetOwner( this );
                Bind( wxEVT_FSWATCHER, &MainFrame::OnPGSchemaChanged, this );
            }
            if( m_db )
            {
                if( ( m_db->GetTableVector().m_type != L"PostgreSQL" ) || ( m_db->GetTableVector().m_type == L"ODBC" &&  m_db->GetTableVector().m_subtype != L"PostgreSQL" ) || ( m_db->GetTableVector().m_type == L"PostgreSQL" && m_db->GetTableVector().m_versionMajor >= 9 && m_db->GetTableVector().m_versionMinor >= 3 ) || ( m_db->GetTableVector().m_type == L"ODBC" && m_db->GetTableVector().m_subtype == L"PostgreSQL" && m_db->GetTableVector().m_versionMajor >= 9 && m_db->GetTableVector().m_versionMinor >= 3 ) )
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
    else
        wxMessageBox( _( "Error loading the DLL/so" ) );
}

void MainFrame::OnDatabase(wxCommandEvent &event)
{
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect();
    if( m_db )
    {
        InitMenuBar( event.GetId() );
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
            m_painters["Database"] = lib;
        }
        else
            lib = m_painters["Database"];
        if( m_db && lib->IsLoaded() )
        {
            DATABASE func = (DATABASE) lib->GetSymbol( "CreateDatabaseWindow" );
#if defined __WXMSW__ && _MSC_VER < 1900
            func( this, m_manager, m_db, DatabaseView, m_threadCS );
#else
            func( this, m_manager, m_db, DatabaseView );
#endif
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
    }
}

void MainFrame::OnQuery(wxCommandEvent &event)
{
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect();
    if( m_db )
    {
        InitMenuBar( event.GetId() );
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
            m_painters["Query"] = lib;
        }
        else
            lib = m_painters["Query"];
        if( m_db && lib->IsLoaded() )
        {
            DATABASE func = (DATABASE) lib->GetSymbol( "CreateDatabaseWindow" );
#if defined __WXMSW__ && _MSC_VER < 1900
            func( this, m_manager, m_db, QueryView, m_threadCS );
#else
            func( this, m_manager, m_db, QueryView );
#endif
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

void MainFrame::OnTable(wxCommandEvent &event)
{
    if( !m_db )
        Connect();
    if( m_db )
    {
        InitMenuBar( event.GetId() );
        if( !m_lib )
        {
            m_lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            m_lib->Load( "tablewindow" );
#elif __WXOSX__
            m_lib->Load( "liblibtablewindow.dylib" );
#else
            m_lib->Load( "libtablewindow" );
#endif
        }
        if( m_db && m_lib->IsLoaded() )
        {
            TABLE func = (TABLE) m_lib->GetSymbol( "CreateDatabaseWindow" );
            func( this, m_manager, m_db, NULL, wxEmptyString );                 // create with possible alteration table
        }
        else if( !m_lib->IsLoaded() )
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

void MainFrame::OnPGSchemaChanged(wxFileSystemWatcherEvent& event)
{
}
