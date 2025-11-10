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
#include <memory>
#include <wx/artprov.h>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/config.h"
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/xml/xml.h"
#include "wx/fswatcher.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "painterobjects.h"
#include "docview.h"
#include "newtablehandler.h"

#if defined( __WXGTK__ ) || defined( __WXQT__ )
#include "query.h"
#include "odbc.h"
#include "library.h"
#include "res/profile.c"
#include "res/database.c"
#include "table.h"
#endif

typedef void (*ODBCSETUP)(wxWindow *);
typedef Database *(*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, wxString &, std::vector<Profile> &);
typedef void (*DATABASE)(wxWindow *, wxDocManager *, Database *, ViewType, std::map<wxString, wxDynamicLibrary *> &, const std::vector<Profile> &, const std::vector<QueryInfo> &, const std::vector<LibrariesInfo> &, Configuration *);
typedef void (*TABLE)(wxWindow *, wxDocManager *, Database *, ViewType, Configuration *);
typedef void (*DISCONNECTFROMDB)(void *, const wxString &);
typedef int (*ATTACHDATABASE)(wxWindow *, Database *);
typedef int (*DETACHDATABASE)(wxWindow *);
typedef int (*CHOOSEOBJECT)(wxWindow *, int);
typedef void (*LIBRARYPAINTER)(wxWindow *, wxDocManager *, ViewType , std::map<wxString, wxDynamicLibrary *> &, Configuration *, LibraryObject *);

BEGIN_EVENT_TABLE(MainFrame, wxDocMDIParentFrame)
    EVT_MENU(wxID_CONFIGUREODBC, MainFrame::OnConfigureODBC)
    EVT_MENU(wxID_DATABASEWINDOWPROFILE, MainFrame::OnDatabaseProfile)
    EVT_MENU(wxID_TABLE, MainFrame::OnTable)
    EVT_MENU(wxID_DATABASE, MainFrame::OnDatabase)
    EVT_MENU(wxID_QUERY, MainFrame::OnQuery)
    EVT_MENU(wxID_ATTACHDATABASE, MainFrame::OnAttachDatabase)
    EVT_MENU(wxID_DETACHDATABASE, MainFrame::OnDetachDatabase)
    EVT_MENU(wxID_LIBRARY, MainFrame::OnLibrary)
    EVT_UPDATE_UI(wxID_DETACHDATABASE, MainFrame::OnUpdateUIDetachDB)
    EVT_SIZE(MainFrame::OnSize)
    EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(wxDocManager *manager) : wxDocMDIParentFrame(manager, NULL, wxID_ANY, "DB Handler" )
{
    const wxString managementLogFile = wxStandardPaths::Get().GetAppDocumentsDir() + wxPATH_SEP + "absms.log";
    m_libraryLoaded = false;
    m_conf = new Configuration;
    m_db = NULL;
    m_countAttached = 0;
    m_handler = NULL;
#if !( defined( __sun ) && defined( __SVR4 ) )
    m_oldPGWatcher = NULL;
#endif
#if defined __WXMSW__ || defined __WXGTK__
    m_tb = NULL;
#endif
    m_libraryPath = wxStandardPaths::Get().GetSharedLibrariesDir() + wxPATH_SEP;
    wxTheColourDatabase->AddColour( "SILVER", wxColour( 0xC0C0C0 ) );
    m_config = wxConfigBase::Get( "DBManager" );
    wxString path = m_config->GetPath();
    m_config->SetPath( "CurrentDB" );
    m_pgLogfile = m_config->Read( "Logfile", "" );
    m_config->SetPath( path );
    m_config->SetPath( "Profiles" );
    long counter;
    wxString profile;
    auto currentProfile = m_config->Read( "CurrentProfile", "" );
    auto res = m_config->GetFirstEntry( profile, counter );
    auto found = false;
    while( res )
    {
        auto prof = m_config->Read( profile, "" );
        if( prof == currentProfile || currentProfile.IsEmpty() )
        {
            m_profiles.push_back( Profile( prof, true ) );
            found = true;
        }
        else
            m_profiles.push_back( Profile( prof, false ) );
        res = m_config->GetNextEntry( profile, counter );
    }
    if( !found && !currentProfile.IsEmpty() )
        m_profiles.push_back( Profile( currentProfile, true ) );
    m_config->SetPath( path );
    m_config->SetPath( "CurrentLibraries" );
    auto libpath = m_config->Read( "Active", "" );
    if( !libpath.IsEmpty() )
    {
        m_conf->m_currentLibrary = libpath;
        m_path.push_back( LibrariesInfo( libpath, true ) );
        libpath = m_config->Read( "Dependency", "" );
    }
    m_config->SetPath( path );
    m_config->SetPath( "MainToolbar" );
    m_conf->m_tbSettings["PowerBar"].m_hideShow = m_config->ReadBool( "Show", true );
    m_conf->m_tbSettings["PowerBar"].m_showTooltips = m_config->ReadBool( "ShowTooltip", true );
    m_conf->m_tbSettings["PowerBar"].m_showText = m_config->ReadBool( "ShowText", false );
    m_conf->m_tbSettings["PowerBar"].m_orientation = m_config->ReadLong( "Orientation", 1 );
    m_config->SetPath( path );
    m_config->SetPath( "ViewBar" );
    m_conf->m_tbSettings["ViewBar"].m_hideShow = m_config->ReadBool( "Show", true );
    m_conf->m_tbSettings["ViewBar"].m_showTooltips = m_config->ReadBool( "ShowTooltip", true );
    m_conf->m_tbSettings["ViewBar"].m_showText = m_config->ReadBool( "ShowText", false );
    m_conf->m_tbSettings["ViewBar"].m_orientation = m_config->ReadLong( "Orientation", 1 );
    m_config->SetPath( path );
    m_config->SetPath( "StyleBar" );
    m_conf->m_tbSettings["StyleBar"].m_hideShow = m_config->ReadBool( "Show", true );
    m_conf->m_tbSettings["StyleBar"].m_showTooltips = m_config->ReadBool( "ShowTooltip", true );
    m_conf->m_tbSettings["StyleBar"].m_showText = m_config->ReadBool( "ShowText", false );
    m_conf->m_tbSettings["StyleBar"].m_orientation = m_config->ReadLong( "Orientation", 1 );
    m_config->SetPath( path );
    m_config->SetPath( "Query" );
    m_config->Read( "QuerySource", &m_conf->m_querySource, 2 );
    m_config->Read( "QueryPresentation", &m_conf->m_queryPresentation, 4 );
    m_config->SetPath( path );
    m_config->SetPath( "Database/General" );
    m_config->Read( "Shared Profiles", &m_conf->m_dbOptions.m_general.m_sharedProfile, "" );
    m_config->Read( "Display Table List", &m_conf->m_dbOptions.m_general.m_tableLst, 1 );
    m_config->Read( "Use Repository", &m_conf->m_dbOptions.m_general.m_useRepo, 1 );
    m_config->Read( "Read Only", &m_conf->m_dbOptions.m_general.m_readOnly, 0 );
    m_config->Read( "Keep Alive", &m_conf->m_dbOptions.m_general.m_keepAlive, 1 );
    m_config->Read( "SQL Terminator Character", &m_conf->m_dbOptions.m_general.m_sqlTerminator, ';' );
    m_config->Read( "Refresh Table List", &m_conf->m_dbOptions.m_general.m_tableRefresh, "1800" );
    m_config->Read( "Columns in Table", &m_conf->m_dbOptions.m_general.m_tableColumns, "8" );
    m_config->SetPath( path );
    m_config->SetPath( "Database/Colors" );
    long defaultVal = 0xFFFFFF;
    long temp = 0x000000;
    m_config->Read( "Background", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_background = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0xC0C0C0;
    m_config->Read( "TableColumns", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_tableCol = wxColor( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0xFF0000;
    m_config->Read( "IndexKeyLiine", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_indexLine = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x808080;
    m_config->Read( "TableHeader", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_tableHeader = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x000000;
    m_config->Read( "TableColumnText", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_tableColText = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x00FF00;
    m_config->Read( "PrimaryKeyLine", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_primaryKeyLine = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x000000;
    m_config->Read( "TableHeaderText", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_tableHeaderText = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x000080;
    m_config->Read( "TableCommentText", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_tableCommentText = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    defaultVal = 0x0000FF;
    m_config->Read( "ForeignKeyLine", &temp, defaultVal );
    m_conf->m_dbOptions.m_colors.m_foreignKeyLine = wxColour( ( temp & 0xFF0000 ) >> 16, ( temp & 0x00FF00 ) >> 8, ( temp & 0x0000FF ) );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPanter/General" );
    m_config->Read( "ShowCheckedOut", &m_conf->m_libPainterOptions.m_general.m_showCheckedOut, 1 );
    m_config->Read( "ShowModification", &m_conf->m_libPainterOptions.m_general.m_showModification, 1 );
    m_config->Read( "ShowCompilation", &m_conf->m_libPainterOptions.m_general.m_showCompilation, 1 );
    m_config->Read( "ShowSizes", &m_conf->m_libPainterOptions.m_general.m_showSizes, 1 );
    m_config->Read( "ShowComments", &m_conf->m_libPainterOptions.m_general.m_showComments, 1 );
    m_config->Read( "Information", &m_conf->m_libPainterOptions.m_general.m_compInformation, 1 );
    m_config->Read( "Obsolete", &m_conf->m_libPainterOptions.m_general.m_compObsolete, 1 );
    m_config->Read( "SaveBackups", &m_conf->m_libPainterOptions.m_general.m_saveBackups, 1 );
    m_config->Read( "ConfirmDelete", &m_conf->m_libPainterOptions.m_general.m_confirmDelete, 1 );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPainter/Include" );
    m_config->Read( "Name", &m_conf->m_libPainterOptions.m_include.m_name, "*" );
    m_config->Read( "Applcation", &m_conf->m_libPainterOptions.m_include.m_app, 1 );
    m_config->Read( "DataPipeline", &m_conf->m_libPainterOptions.m_include.m_pipeline, 1 );
    m_config->Read( "DataWindow", &m_conf->m_libPainterOptions.m_include.m_datawin, 1 );
    m_config->Read( "Function", &m_conf->m_libPainterOptions.m_include.m_function, 1 );
    m_config->Read( "Menu", &m_conf->m_libPainterOptions.m_include.m_menu, 1 );
    m_config->Read( "Project", &m_conf->m_libPainterOptions.m_include.m_project, 1 );
    m_config->Read( "Proxy", &m_conf->m_libPainterOptions.m_include.m_proxy, 1 );
    m_config->Read( "Query", &m_conf->m_libPainterOptions.m_include.m_query, 1 );
    m_config->Read( "Structure", &m_conf->m_libPainterOptions.m_include.m_structure, 1 );
    m_config->Read( "UserObject", &m_conf->m_libPainterOptions.m_include.m_userobject, 1 );
    m_config->Read( "Window", &m_conf->m_libPainterOptions.m_include.m_wndow, 1 );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPainter/SourceManagement" );
    m_config->Read( "LogEverything", &m_conf->m_libPainterOptions.m_management.m_logEverything, 0 );
    m_config->Read( "FileName", &m_conf->m_libPainterOptions.m_management.m_logFile, managementLogFile );
    m_config->Read( "LogOptions", &m_conf->m_libPainterOptions.m_management.m_options, 1 );
    m_config->Read( "RequireCommentOnCheckin", &m_conf->m_libPainterOptions.m_management.m_requireComment, 0 );
    m_config->SetPath( path );
    m_conf->m_textDirection = wxGetApp().GetLayoutDirection();
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
    long style = wxNO_BORDER | wxTB_FLAT;
    switch( m_conf->m_tbSettings["PowerBar"].m_orientation )
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
    if( !m_conf->m_tbSettings["PowerBar"].m_showTooltips )
        style |= wxTB_NO_TOOLTIPS;
    if( m_conf->m_tbSettings["PowerBar"].m_showText )
        style |= wxTB_TEXT ;
    CreateToolBar( style );
    if( !m_conf->m_tbSettings["PowerBar"].m_hideShow )
        GetToolBar()->Hide();
    InitToolBar( GetToolBar() );
}

MainFrame::~MainFrame()
{
    std::vector<std::wstring> errorMsg;
    int result = 0;
    wxString path = m_config->GetPath();
    m_config->SetPath( "CurrentDB" );
    if( m_db )
    {
        wxString temp1( m_db->GetTableVector().m_type );
        m_config->Write( "Engine", temp1 );
        wxString temp2 = m_db->GetTableVector().m_subtype;
        m_config->Write( "Subtype", temp2 );
        if( temp1 == "PostgreSQL" || temp2 == "PostgreSQL" )
        {
            wxString temp3( m_db->GetTableVector().GetPostgreLogFile() );
            m_config->Write( "Logfile", temp3 );
        }
        wxString currentProfile;
        temp1 = m_db->GetTableVector().m_connectString;
        m_config->Write( "DatabaseName", temp1 );
        if( m_db->GetTableVector().m_type == L"SQLite" )
#ifdef __WXMSW__
            currentProfile = temp1.Mid( temp1.find_last_of( '\\' ) + 1 );
#else
            currentProfile = temp1.Mid( temp1.find_last_of( '/' ) + 1 );
#endif
        m_config->SetPath( path );
        m_config->SetPath( "Profiles" );
        auto found = false;
        long counter;
        wxString profile;
        auto res = m_config->GetFirstEntry( profile, counter );
        while( res && !found )
        {
            auto prof = m_config->Read( profile, "" );
            if( prof == currentProfile )
                found = true;
            res = m_config->GetNextEntry( profile, counter );
        }
        if( !found )
            m_config->Write( wxString::Format( "Profile%ld", counter ), currentProfile );
        m_config->SetPath( path );
        std::lock_guard<std::mutex>( m_db->GetTableVector().my_mutex );
        result = m_db->Disconnect( errorMsg );
    }
    m_config->SetPath( "CurrentLibraries" );
    m_config->Write( "Active", m_conf->m_currentLibrary );
    m_config->SetPath( path );
    m_config->SetPath( "MainToolbar" );
    m_config->Write( "Show", m_conf->m_tbSettings["PowerBar"].m_hideShow );
    m_config->Write( "ShowTooltip", m_conf->m_tbSettings["PowerBar"].m_showTooltips );
    m_config->Write( "ShowText", m_conf->m_tbSettings["PowerBar"].m_showText );
    m_config->Write( "Orientation", m_conf->m_tbSettings["PowerBar"].m_orientation );
    m_config->SetPath( path );
    m_config->SetPath( "ViewBar" );
    m_config->Write( "Show", m_conf->m_tbSettings["ViewBar"].m_hideShow );
    m_config->Write( "ShowTooltip", m_conf->m_tbSettings["ViewBar"].m_showTooltips );
    m_config->Write( "ShowText", m_conf->m_tbSettings["ViewBar"].m_showText );
    m_config->Write( "Orientation", m_conf->m_tbSettings["ViewBar"].m_orientation );
    m_config->SetPath( path );
    m_config->SetPath( "StyleBar" );
    m_config->Write( "Show", m_conf->m_tbSettings["StyleBar"].m_hideShow );
    m_config->Write( "ShowTooltip", m_conf->m_tbSettings["StyleBar"].m_showTooltips );
    m_config->Write( "ShowText", m_conf->m_tbSettings["StyleBar"].m_showText );
    m_config->Write( "Orientation", m_conf->m_tbSettings["StyleBar"].m_orientation );
    m_config->SetPath( path );
    m_config->SetPath( "Database/General" );
    m_config->Write( "Shared Profiles", m_conf->m_dbOptions.m_general.m_sharedProfile );
    m_config->Write( "Display Table List", m_conf->m_dbOptions.m_general.m_tableLst );
    m_config->Write( "Use Repository", m_conf->m_dbOptions.m_general.m_useRepo );
    m_config->Write( "Read Only", m_conf->m_dbOptions.m_general.m_readOnly );
    m_config->Write( "Keep Alive", m_conf->m_dbOptions.m_general.m_keepAlive );
    m_config->Write( "SQL Terminator Character", m_conf->m_dbOptions.m_general.m_sqlTerminator );
    m_config->Write( "Refresh Table List", m_conf->m_dbOptions.m_general.m_tableRefresh );
    m_config->Write( "Columns in Table", m_conf->m_dbOptions.m_general.m_tableColumns );
    m_config->SetPath( path );
    m_config->SetPath( "Database/Colors" );
    m_config->Write( "Background", m_conf->m_dbOptions.m_colors.m_background.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_background.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_background.GetBlue() );
    m_config->Write( "TableColumns", m_conf->m_dbOptions.m_colors.m_tableCol.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_tableCol.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_tableCol.GetBlue() );
    m_config->Write( "IndexKeyLiine", m_conf->m_dbOptions.m_colors.m_indexLine.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_indexLine.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_indexLine.GetBlue() );
    m_config->Write( "PrimaryKeyLine", m_conf->m_dbOptions.m_colors.m_primaryKeyLine.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_primaryKeyLine.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_primaryKeyLine.GetBlue() );
    m_config->Write( "TableHeader", m_conf->m_dbOptions.m_colors.m_tableHeader.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_tableHeader.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_tableHeader.GetBlue() );
    m_config->Write( "TableColumnText", m_conf->m_dbOptions.m_colors.m_tableColText.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_tableColText.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_tableColText.GetBlue() );
    m_config->Write( "TableHeaderText", m_conf->m_dbOptions.m_colors.m_tableHeaderText.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_tableHeaderText.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_tableHeaderText.GetBlue() );
    m_config->Write( "TableCommentText", m_conf->m_dbOptions.m_colors.m_tableCommentText.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_tableCommentText.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_tableCommentText.GetBlue() );
    m_config->Write( "ForeignKeyLine", m_conf->m_dbOptions.m_colors.m_foreignKeyLine.GetRed() * 65536 + m_conf->m_dbOptions.m_colors.m_foreignKeyLine.GetGreen() * 256 + m_conf->m_dbOptions.m_colors.m_foreignKeyLine.GetBlue() );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPanter/General" );
    m_config->Write( "ShowCheckedOut", m_conf->m_libPainterOptions.m_general.m_showCheckedOut );
    m_config->Write( "ShowModification", m_conf->m_libPainterOptions.m_general.m_showModification );
    m_config->Write( "ShowCompilation", m_conf->m_libPainterOptions.m_general.m_showCompilation );
    m_config->Write( "ShowSizes", m_conf->m_libPainterOptions.m_general.m_showSizes );
    m_config->Write( "ShowComments", m_conf->m_libPainterOptions.m_general.m_showComments );
    m_config->Write( "Information", m_conf->m_libPainterOptions.m_general.m_compInformation );
    m_config->Write( "Obsolete", m_conf->m_libPainterOptions.m_general.m_compObsolete );
    m_config->Write( "SaveBackups", m_conf->m_libPainterOptions.m_general.m_saveBackups );
    m_config->Write( "ConfirmDelete", m_conf->m_libPainterOptions.m_general.m_confirmDelete );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPainter/Include" );
    m_config->Write( "Name", m_conf->m_libPainterOptions.m_include.m_name );
    m_config->Write( "Applcation", m_conf->m_libPainterOptions.m_include.m_app );
    m_config->Write( "DataPipeline", m_conf->m_libPainterOptions.m_include.m_pipeline );
    m_config->Write( "DataWindow", m_conf->m_libPainterOptions.m_include.m_datawin );
    m_config->Write( "Function", m_conf->m_libPainterOptions.m_include.m_function );
    m_config->Write( "Menu", m_conf->m_libPainterOptions.m_include.m_menu );
    m_config->Write( "Project", m_conf->m_libPainterOptions.m_include.m_project );
    m_config->Write( "Proxy", m_conf->m_libPainterOptions.m_include.m_proxy );
    m_config->Write( "Query", m_conf->m_libPainterOptions.m_include.m_query );
    m_config->Write( "Structure", m_conf->m_libPainterOptions.m_include.m_structure );
    m_config->Write( "UserObject", m_conf->m_libPainterOptions.m_include.m_userobject );
    m_config->Write( "Window", m_conf->m_libPainterOptions.m_include.m_wndow );
    m_config->SetPath( path );
    m_config->SetPath( "LibraryPainter/SourceManagement" );
    m_config->Write( "LogEverything", m_conf->m_libPainterOptions.m_management.m_logEverything );
    m_config->Write( "FileName", m_conf->m_libPainterOptions.m_management.m_logFile );
    m_config->Write( "LogOptions", m_conf->m_libPainterOptions.m_management.m_options );
    m_config->Write( "RequireCommentOnCheckin", m_conf->m_libPainterOptions.m_management.m_requireComment );
    m_config->SetPath( path );
    if( result )
    {
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
    }
    delete m_db;
    m_db = nullptr;
    delete m_conf;
    m_conf = nullptr;
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
#if defined( __WXGTK__ ) || defined( __WXQT__ )
    toolBar->AddTool( wxID_QUERY, _( "Query" ), wxBitmapBundle::FromSVG( query, wxSize( 16, 16 ) ),  wxBitmapBundle::FromSVG( query, wxSize( 16, 16 ) ), wxITEM_NORMAL,_( "Query" ), _( "Run the query" ) );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), wxBitmapBundle::FromSVG( odbc, wxSize( 16, 16 ) ), wxBitmapBundle::FromSVG( odbc, wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Configre ODBC" ), _( "Configre ODBC" ));
#else
    toolBar->AddTool( wxID_QUERY, _( "Query" ), wxBitmapBundle::FromSVGResource( "query", wxSize( 16, 16 ) ), wxBitmapBundle::FromSVGResource( "query", wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Query" ), _( "Run uqery" ) );
    toolBar->AddTool( wxID_CONFIGUREODBC, _( "ODBC" ), wxBitmapBundle::FromSVGResource( "odbc", wxSize( 16, 16 ) ), wxBitmapBundle::FromSVGResource( "odbc", wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Configure ODBC" ), _( "Configure ODBC" ) );
#endif
    toolBar->AddTool( wxID_DATABASEWINDOWPROFILE, _( "Profile" ), wxBitmapBundle::FromBitmaps( bitmaps[2] ), wxBitmapBundle::FromBitmaps( bitmaps[2] ), wxITEM_NORMAL, _( "DB Profile" ), _( "DB Profile" ) );
#if defined( __WXGTK__ ) || defined( __WXQT__ )
    toolBar->AddTool( wxID_TABLE, _( "Table" ), wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) ), wxBitmapBundle::FromSVG( table, wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Table" ), _( "Add/Modify Table definition" ) );
#else
    toolBar->AddTool( wxID_TABLE, _( "Table" ), wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) ), wxBitmapBundle::FromSVGResource( "table", wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Table" ), _( "Add/Modify Table definition" ) );
#endif
    toolBar->AddTool( wxID_DATABASE, _( "Database" ), wxBitmapBundle::FromBitmaps( bitmaps[4] ), wxBitmapBundle::FromBitmaps( bitmaps[4] ), wxITEM_NORMAL, _( "Database" ), _( "Perform database operations") );
#if defined( __WXGTK__ ) || defined( __WXQT__ )
    toolBar->AddTool( wxID_LIBRARY, _( "Library" ), wxBitmapBundle::FromSVG( library, wxSize( 16, 16 ) ), wxBitmapBundle::FromSVG( library, wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Library selector" ) );
#else
    toolBar->AddTool( wxID_LIBRARY, _( "Library" ), wxBitmapBundle::FromSVGResource( "library", wxSize( 16, 16 ) ), wxBitmapBundle::FromSVGResource( "library", wxSize( 16, 16 ) ), wxITEM_NORMAL, _( "Library selector" ) );
#endif
    toolBar->AddTool( wxID_EXIT, _( "Exit the application" ), wxArtProvider::GetBitmapBundle( wxART_QUIT, wxART_TOOLBAR ), wxBitmapBundle(), wxITEM_NORMAL, _( "Quit" ), _( "Quit the application" ) );
    toolBar->SetName( "PowerBar" );
    toolBar->Realize();
//    toolBar->Bind( wxEVT_LEFT_DOWN, &MainFrame::OnCustomize, this );
}

void MainFrame::Connect(bool atomatic)
{
    Database *db = NULL;
    wxString title;
    std::vector<std::wstring> errorMsg;
    wxDynamicLibrary *lib = NULL;
    wxString name = "";
    wxString engine = "";
    wxString connectStr = "";
    wxString connectedUser = "";
    if( m_painters.find( "DBloader" ) == m_painters.end() )
    {
        bool loaded;
        lib = new wxDynamicLibrary();
        wxString libName;
#ifdef __WXMSW__
        libName = m_libraryPath + "dbloader";
#elif __WXMAC__
        libName = m_libraryPath + "liblibdbloader.dylib";
#else
        libName = m_libraryPath + "libdbloader";
#endif
        loaded = lib->Load( libName );
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
        if( atomatic )
        {
            name = wxGetApp().GetDBName();
            engine = wxGetApp().GetDBEngine();
            connectStr = wxGetApp().GetConnectString();
            connectedUser = wxGetApp().GetConnectedUser();
        }
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
                title = L"Connected to " + m_db->GetTableVector().m_subtype + L" version " + m_db->GetTableVector().m_serverVersion + L" thru the ODBC";
            else if( m_db && m_db->GetTableVector().m_type == L"SQLite" )
                title = wxString::Format( L"Connected to " + m_db->GetTableVector().m_type + L" version " + major + L"." + minor + L"." + revision );
            else if( m_db )
                title = wxString::Format( L"Connected to " + m_db->GetTableVector().m_type + L" version " + major + L"." + minor + L"." + revision + L" with the client library " + clientMajor + L"." + clientMinor + L"." + clientRevision );
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
    wxString libName;
    if( m_painters.find( "dialogs" ) == m_painters.end() )
    {
        lib = new wxDynamicLibrary();
#ifdef __WXMSW__
        libName = m_libraryPath + "dialogs";
#elif __WXMAC__
        libName = m_libraryPath + "liblibdialogs.dylib";
#else
        libName = m_libraryPath + "libdialogs";
#endif
        lib->Load( libName );
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
        Connect( true );
    if( m_db )
    {
        if( m_painters.find( "Database" ) == m_painters.end() )
        {
            wxString libName;
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            libName = m_libraryPath + "dbwindow";
#elif __WXOSX__
            libName = m_libraryPath + "liblibdbwindow.dylib";
#else
            libName = m_libraryPath + "libdbwindow";
#endif
            lib->Load( libName);
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
            wxString libName;
            lib1 = new wxDynamicLibrary;
#ifdef __WXMSW__
            libName = m_libraryPath + "tabledataedit";
#elif __WXOSX__
            libName = m_libraryPath + "liblibtabledataedit.dylib";
#else
            libName = m_libraryPath + "libtableedit";
#endif
            lib1->Load( libName );
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
            wxString libName;
            lib2 = new wxDynamicLibrary;
#ifdef __WXMSW__
            libName = m_libraryPath + "tabledataedit";
#elif __WXOSX__
            libName = m_libraryPath + "liblibtabledataedit.dylib";
#else
            libName = m_libraryPath + "libtabledataedit";
#endif
            lib2->Load( libName );
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
            func( this, m_manager, m_db, DatabaseView, m_painters, m_profiles, queries, m_path, m_conf );
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
    }
}

void MainFrame::OnQuery(wxCommandEvent &WXUNUSED(event))
{
    wxString libName;
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect( true );
    if( m_db )
    {
        if( m_painters.find( "Query" ) == m_painters.end() )
        {
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            libName = m_libraryPath + "dbwindow";
#elif __WXOSX__
            libName = m_libraryPath + "liblibdbwindow.dylib";
#else
            libName = m_libraryPath + "libdbwindow";
#endif
            lib->Load( libName );
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
        if( !m_libraryLoaded && LoadApplication( m_path ) )
            return;
        if( m_db && lib->IsLoaded() )
        {
            DATABASE func = (DATABASE) lib->GetSymbol( "CreateDatabaseWindow" );
            func( this, m_manager, m_db, QueryView, m_painters, m_profiles, queries, m_path, m_conf );
        }
        else if( !lib->IsLoaded() )
            wxMessageBox( "Error loading the library. Please re-install the software and try again." );
        else
            wxMessageBox( "Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again." );
	}
}

void MainFrame::OnDatabaseProfile(wxCommandEvent &WXUNUSED(event))
{
    Connect( false );
}

void MainFrame::OnTable(wxCommandEvent &WXUNUSED(event))
{
    wxString libName;
    wxDynamicLibrary *lib = NULL;
    if( !m_db )
        Connect( true );
    if( m_db )
    {
        if( m_painters.find( "TableView" ) == m_painters.end() )
        {
            lib = new wxDynamicLibrary;
#ifdef __WXMSW__
            libName = m_libraryPath + "tabledataedit";
#elif __WXOSX__
            libName = m_libraryPath + "liblibtabledataedit.dylib";
#else
            libName = m_libraryPath + "libtableedit";
#endif
            lib->Load( libName );
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
            TABLE func = (TABLE) lib->GetSymbol( "CreateDataEditWindow" );
            func( this, m_manager, m_db, TableView, m_conf );                 // create with possible alteration table
        }
        else if( !lib->IsLoaded() )
            wxMessageBox("Error loading the library. Please re-install the software and try again.");
        else
            wxMessageBox("Error connecting to the database. Please check the database is accessible and you can get a good connection, then try again.");
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
        tb->SetSize(0, 0, size.x, wxDefaultCoord);
        offset = tb->GetSize().y;
        child->SetSize(0, offset, size.x, size.y - offset);
        if( frame )
            frame->SetSize(0, 0, size.x, size.y - offset - 2);
    }
    else if( !tb || tb->GetName() == "PowerBar" )
        event.Skip();
}

void MainFrame::OnAttachDatabase(wxCommandEvent &WXUNUSED(event))
{
    auto lib = new wxDynamicLibrary;
    wxString libName;
#ifdef __WXMSW__
    libName = m_libraryPath + "dialogs";
#elif __WXOSX__
    libName = m_libraryPath + "liblibdialogs.dylib";
#else
    libName = m_libraryPath + "libdialogs";
#endif
    lib->Load( libName );
    ATTACHDATABASE func = (ATTACHDATABASE) lib->GetSymbol( "AttachToDatabase" );
    int result = func( this, m_db );
    if( result == wxID_OK )
        m_countAttached++;
    delete lib;
    lib = nullptr;
}

void MainFrame::OnDetachDatabase(wxCommandEvent &WXUNUSED(event))
{
    wxString libName;
    auto lib = new wxDynamicLibrary;
#ifdef __WXMSW__
    libName = m_libraryPath + "dialogs";
#elif __WXOSX__
    libName = m_libraryPath + "liblibdialogs.dylib";
#else
    libName = m_libraryPath + "libdialogs";
#endif
    lib->Load( libName );
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
        event.Enable(true);
    else
        event.Enable(false);
}

void MainFrame::OnLibrary(wxCommandEvent &WXUNUSED(event))
{
    wxDynamicLibrary *lib;
    wxString libName;
    LoadApplication( m_path );
    if( m_painters.find( "Library" ) == m_painters.end() )
    {
        lib = new wxDynamicLibrary;
#ifdef __WXMSW__
        libName = m_libraryPath + "library";
#elif __WXOSX__
        libName = m_libraryPath + "libliblibrary.dylib";
#else
        libName = m_libraryPath + "liblibrary";
#endif
        lib->Load( libName );
        if( lib->IsLoaded() )
        {
            m_painters["Library"] = lib;
            LIBRARYPAINTER func = (LIBRARYPAINTER) lib->GetSymbol( "CreateLibraryWindow" );
            func( this, m_manager, LibraryView, m_painters, m_conf, m_library.get() );
        }
        else
        {
            delete lib;
            lib = nullptr;
        }
    }
    else
        lib = m_painters["Library"];
}

bool MainFrame::LoadApplication(const std::vector<LibrariesInfo> &path)
{
    m_library = std::make_shared<LibraryObject>();
    if( m_doc.GetRoot() )
        return true;
    if( path.empty() )
    {
        wxMessageBox( _( "Need to create a library first" ) );
        return true;
    }
    for( std::vector<LibrariesInfo>::const_iterator it = path.begin(); it < path.end(); ++it )
    {
        if( (*it).m_isActive )
        {
            m_library->SetLibraryName( (*it).m_path );
            if( !m_doc.Load( (*it).m_path ) )
            {
                wxMessageBox( _( "Loading failure" ) );
                return true;
            }
        }
    }
    if( m_doc.GetRoot() == nullptr )
    {
        wxMessageBox( _( "XNL file error:" ) );
        return true;
    }
    if( !m_doc.GetRoot()->GetName().IsSameAs( "Library" ) )
    {
        wxMessageBox( _( "XML formatting rtot"));
        return true;
    }
    QueryInfo query;
    wxString widthStr, objectComment, objectName;
    unsigned int size = 0;
    wxDateTime lastmodified = wxDateTime::Now(), lastcompiled = wxDateTime::Now();
    wxXmlNode *children = m_doc.GetRoot()->GetChildren(), *bodyChildren, *queryChildren, *headerChildren;
    while( children )
    {
        if( children->GetName().IsSameAs( "Header" ) )
        {
            headerChildren = children->GetChildren();
            while( headerChildren )
            {
                if( headerChildren->GetName().IsSameAs( "Created" ) )
                {
                    wxDateTime dt;
                    wxString::const_iterator end;
                    widthStr = headerChildren->GetNodeContent();
                    auto res = dt.ParseISOCombined( widthStr );
                    if( res )
                        m_library->SetCreationTime( dt );
                }
                if( headerChildren->GetName().IsSameAs( "Comment" ) )
                {
                    widthStr = headerChildren->GetNodeContent();
                    m_library->SetComment( widthStr );
                }
                headerChildren = headerChildren->GetNext();
            }
            children = children->GetNext();
        }
        if( children->GetName().IsSameAs( "Body" ) )
        {
            bodyChildren = children->GetChildren();
            while( bodyChildren )
            {
                if( bodyChildren->GetName().IsSameAs( "Query" ) )
                {
                    queryChildren = bodyChildren->GetChildren();
                    while( queryChildren )
                    {
                        if( queryChildren->GetName().IsSameAs( "name" ) )
                        {
                            widthStr = queryChildren->GetNodeContent();
                            if( widthStr.substr( widthStr.size() - 3 ) == "qry" )
                            {
                                objectName = widthStr.substr( 0, widthStr.length() - 4 );
                                query.name = objectName;
                            }
                        }
                        if( queryChildren->GetName().IsSameAs( "lastmodified" ) )
                        {
                            wxString::const_iterator end;
                            widthStr = queryChildren->GetNodeContent();
                            lastmodified.ParseISOCombined( widthStr );
                        }
                        if( queryChildren->GetName().IsSameAs( "lastcompiled" ) )
                        {
                            wxString::const_iterator end;
                            widthStr = queryChildren->GetNodeContent();
                            lastcompiled.ParseISOCombined( widthStr );
                        }
                        if( queryChildren->GetName().IsSameAs( "sizeinbytes" ) )
                        {
                            size = wxAtoi( queryChildren->GetNodeContent() );
                        }
                        if( queryChildren->GetName().IsSameAs( "comment" ) )
                        {
                            objectComment = queryChildren->GetNodeContent();
                            query.comment = objectComment;
                        }
                        queryChildren = queryChildren->GetNext();
                    }
                }
                m_library->GetObjects().push_back( LibraryObjects( m_library->GetLibraryName(), objectName, lastmodified, lastcompiled, size, objectComment ) );
                queries.push_back( query );
                bodyChildren = bodyChildren->GetNext();
            }
        }
        children = children->GetNext();
    }
    m_libraryLoaded = true;
	return false;
}

/*void MainFrame::OnCustomize(wxMouseEvent &event)
{
    wxMessageBox( "Customizing" );
}
*/
