#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif
#ifdef WIN32
#include <windows.h>
#include "wx/msw/winundef.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>
#include <sqlext.h>
#include "wx/dynlib.h"
#include "sqlite3.h"
#include "database.h"
#include "database_sqlite.h"
#include "database_odbc.h"

typedef int (*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, bool, const std::vector<std::wstring> &);

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

extern "C" WXEXPORT Database *ConnectToDb(wxWindow *parent, wxString &name, wxString &engine)
{
    std::vector<std::wstring> errorMsg, dsn;
    bool ask = false;
    Database *pdb = NULL;
    wxDynamicLibrary lib;
    wxBeginBusyCursor();
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        pdb = new ODBCDatabase();
        bool res = dynamic_cast<ODBCDatabase *>( pdb )->GetDSNList( dsn, errorMsg );
        delete pdb;
        pdb = NULL;
        if( res )
        {
            for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
            {
                wxMessageBox( (*it) );
            }
        }
        DBPROFILE func = (DBPROFILE) lib.GetSymbol( "DatabaseProfile" );
        int result = func( parent, _( "Select Database Profile" ), name, engine, ask, dsn );
        if( result != wxID_CANCEL )
        {
            if( engine == "SQLite" )
            {
                pdb = new SQLiteDatabase();
            }
            if( engine == "ODBC" )
            {
                pdb = new ODBCDatabase();
                dynamic_cast<ODBCDatabase *>( pdb )->SetWindowHandle( parent->GetHandle() );
            }
            result = pdb->Connect( name.ToStdWstring(), errorMsg );
            if( result )
            {
                for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
                {
                    wxMessageBox( (*it) );
                }
                delete pdb;
                pdb = NULL;
                result = 1;
            }
        }
    }
    wxEndBusyCursor();
    return pdb;
}

extern "C" WXEXPORT Database *GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg)
{
    Database *db = new ODBCDatabase();
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