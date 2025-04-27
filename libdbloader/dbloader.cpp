#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef WIN32
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include <map>
#include <vector>
#include <locale>
#include <codecvt>
#include <algorithm>
#ifdef _IODBCUNIX_FRAMEWORK
#include "iODBC/sqlext.h"
#else
#include <sqlext.h>
#endif
#include "wx/dynlib.h"
#include "wx/stdpaths.h"
#include "wx/filename.h"
#include "sqlite3.h"
#include <libpq-fe.h>
#include <mysql.h>
#include "database.h"
#include "database_sqlite.h"
#include "database_odbc.h"
#include "database_postgres.h"
#include "database_mysql.h"

struct Profile
{
    wxString m_name;
    bool m_isCurrent;

    Profile(wxString name, bool isCurrent) : m_name( name ), m_isCurrent( isCurrent ) {}
};

typedef int (*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, wxString &, bool, const std::vector<std::wstring> &, std::vector<Profile> &);
typedef int (*GETODBCCREDENTIALS)(wxWindow *, const wxString &, wxString &, wxString &);

#ifdef __WXMSW__
WXDLLIMPEXP_BASE void wxSetInstance( HINSTANCE hInst );

BOOL APIENTRY DllMain( HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    lpReserved = lpReserved;
    hModule = hModule;
#ifndef WXUSINGDLL
    int argc = 0;
    char **argv = NULL;
#endif
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
#ifdef WXUSINGDLL
        wxInitialize();
#else
        wxSetInstance( (HINSTANCE) hModule );
        wxEntryStart( argc, argv );
        if( !wxTheApp || !wxTheApp->CallOnInit() )
        {
            wxEntryCleanup();
            if( wxTheApp )
                wxTheApp->OnExit();
            return FALSE;
        }
#endif
        break;
    case DLL_PROCESS_DETACH:
#ifdef WXUSINGDLL
        wxUninitialize();
#else
        if( wxTheApp )
            wxTheApp->OnExit();
        wxEntryCleanup();
#endif
        break;
    }
    return TRUE;
}
#endif

class MyDllApp : public wxApp
{
public:
    bool OnInit()
    {
        return true;
    }

    int OnExit()
    {
        return 0;
    }
};

IMPLEMENT_APP_NO_MAIN(MyDllApp);

extern "C" WXEXPORT Database *ConnectToDb(wxWindow *parent, wxString &name, wxString &engine, wxString &connectStr, wxString &connectedUser, std::vector<Profile> &profiles)
{
    auto osId = wxPlatformInfo::Get().GetOperatingSystemId();
    wxString desktop = wxPlatformInfo::Get().GetDesktopEnvironment();
    std::vector<std::wstring> errorMsg, dsn;
    int result = wxID_OK;
    bool ask = false;
    Database *pdb = NULL;
    wxDynamicLibrary lib;
    wxString libName;
    auto stdPath = wxStandardPaths::Get();
    if( engine == "SQLite" && !name.IsEmpty() )
        connectStr = name;
#ifdef __WXMSW__
    wxFileName fn( stdPath.GetExecutablePath() );
    libName = fn.GetPathWithSep() + "dialogs";
#elif __WXOSX__
    wxFileName fn( stdPath.GetExecutablePath() );
    fn.RemoveLastDir();
    auto path = fn.GetPathWithSep() + "Frameworks/";
    libName = path + "liblibdialogs.dylib";
#else
    libName = stdPath.GetInstallPrefix() + "/lib/libdialogs";
#endif
    lib.Load( libName );
    if( lib.IsLoaded() )
    {
        bool res;
        {
            std::unique_ptr<ODBCDatabase> pdb( new ODBCDatabase( osId, desktop.ToStdWstring() ) );
            res = pdb->GetDSNList( dsn, errorMsg );
        }
        if( res )
        {
            for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
            {
                wxMessageBox( (*it) );
            }
        }
        for( std::vector<std::wstring>::iterator it = dsn.begin(); it < dsn.end(); ++it )
        {
            wxString temp( (*it) );
            if( std::find_if( profiles.begin(), profiles.end(), 
                               [temp](Profile prof)
                               {
                                   return prof.m_name == temp;
                               } ) == profiles.end() )
                profiles.push_back( Profile( temp, false ) );
        }
        result = wxID_OK;
        if( engine.IsEmpty() )
        {
            DBPROFILE func = (DBPROFILE) lib.GetSymbol( "DatabaseProfile" );
            result = func( parent, _( "Select Database Profile" ), name, engine, connectedUser, ask, dsn, profiles );
        }
        if( result != wxID_CANCEL )
        {
            if( engine == "SQLite" )
            {
                pdb = new SQLiteDatabase(  osId, desktop.ToStdWstring() );
            }
            else if( engine == "ODBC" )
            {
                pdb = new ODBCDatabase( osId, desktop.ToStdWstring() );
                dynamic_cast<ODBCDatabase *>( pdb )->SetWindowHandle( parent->GetHandle() );
#if !defined(__WXMSW__)
                std::wstring user, password;
                dynamic_cast<ODBCDatabase *>( pdb )->GetConnectedUser( name.ToStdWstring(), user );
                dynamic_cast<ODBCDatabase *>( pdb )->GetConnectionPassword ( name.ToStdWstring(), password );
	            wxString user_wx( user );
                wxString password_wx( password );
                GETODBCCREDENTIALS func1 = (GETODBCCREDENTIALS) lib.GetSymbol( "GetODBCCredentails" );
                result = func1( parent, name, user_wx, password_wx );
                name = name + L";" + user_wx + L";" + password_wx;
#endif
            }
            else if( engine == "PostgreSQL" )
            {
                pdb = new PostgresDatabase( osId, desktop.ToStdWstring() );
            }
            else if( engine == "mySQL" || engine == "MySQL" )
                pdb = new MySQLDatabase( osId, desktop.ToStdWstring() );
            else
            {
                wxMessageBox( _( "Unknown engine. Please try to reinstall the program!" ) );
                return nullptr;
            }
            wxBeginBusyCursor();
            result = pdb->Connect( name.ToStdWstring(), dsn, errorMsg );
            wxEndBusyCursor();
            if( result )
            {
                for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
                {
                    wxMessageBox( (*it) );
                }
                errorMsg.clear();
                wxMessageBox( _( "Can't connect to the database! Please restart the application" ) );
                pdb->Disconnect( errorMsg );
                delete pdb;
                pdb = NULL;
                result = 1;
            }
        }
    }
    return pdb;
}

extern "C" WXEXPORT Database *GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg)
{
    auto osId = wxPlatformInfo::Get().GetOperatingSystemId();
    wxString desktop = wxPlatformInfo::Get().GetDesktopEnvironment();
    Database *db = new ODBCDatabase( osId, desktop.ToStdWstring() );
    bool result = dynamic_cast<ODBCDatabase *>( db )->GetDriverList( driversDSN, errMsg );
    if( !result )
    {
        delete db;
        db = NULL;
        result = 1;
        for( std::vector<std::wstring>::iterator it = errMsg.begin(); it < errMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
        wxMessageBox( _( L"Problem retrieving database information. Please check the database is running and operational and try again" ) );
    }
    return db;
}

extern "C" WXEXPORT int AddNewDSN(Database *db, wxWindow *win, const wxString &driver)
{
    std::vector<std::wstring> errorMsg;
    int result = dynamic_cast<ODBCDatabase *>( db )->AddDsn( win->GetHandle(), driver.ToStdWstring(), errorMsg );
    if( !result )
    {
        result = 1;
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
        wxMessageBox( _( L"Problem adding Data Source Name" ) );
    }
    return result;
}

extern "C" WXEXPORT int EditDSN(Database *db, wxWindow *win, const wxString &driver, const wxString &dsn)
{
    std::vector<std::wstring> errorMsg;
    int result = dynamic_cast<ODBCDatabase *>( db )->EditDsn( win->GetHandle(), driver.ToStdWstring(), dsn.ToStdWstring(), errorMsg );
    if( !result )
    {
        result = 1;
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
        wxMessageBox( _( L"Problem adding Data Source Name" ) );
    }
    return result;
}

extern "C" WXEXPORT int RemoveDSN(Database *db, const wxString &driver, const wxString &dsn)
{
    std::vector<std::wstring> errorMsg;
    int result = dynamic_cast<ODBCDatabase *>( db )->RemoveDsn( driver.ToStdWstring(), dsn.ToStdWstring(), errorMsg );
    if( !result )
    {
        result = 1;
        for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
        {
            wxMessageBox( (*it) );
        }
        wxMessageBox( _( L"Problem adding Data Source Name" ) );
    }
    return result;
}
