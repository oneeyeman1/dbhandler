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

#ifdef __WXGTK__
#define SQL_WCHART_CONVERT
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>
#ifdef _IODBCUNIX_H
#include "iODBC/sqlext.h"
#else
#include <sqlext.h>
#endif
#include "wx/dynlib.h"
#include "sqlite3.h"
#include "database.h"
#include "database_sqlite.h"
#include "database_odbc.h"

typedef int (*DBPROFILE)(wxWindow *, const wxString &, wxString &, wxString &, bool);

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
/*
extern "C" WXEXPORT SQLWCHAR *ConvertFromString(wxString str)
{
	SQLWCHAR *s = new SQLWCHAR[str.length() * sizeof( SQLWCHAR )];
    SQLWCHAR *tmp;
#if defined(__WXMSW__) || defined( SQL_ATTR_UNIXODBC_VERSION )
    tmp = const_cast<SQLWCHAR *>( str.wc_str() );
#elif defined(  _IODBCUNIX_H )
    tmp = str.wc_str();
#endif
    wcscpy( s, tmp/*, sizeof( temp ) );
    return s;
}

extern "C" WXEXPORT void ConvertFromSQLWCHAR(SQLWCHAR *str, wxString &result)
{
    wxString string;
#if defined(__WXMSW__) || defined( SQL_ATTR_UNIXODBC_VERSION )
    string = str;
#elif defined(  _IODBCUNIX_H )
    string = str.wc_str();
#endif
//    return string;
    result = string;
}
*/
extern "C" WXEXPORT Database *ConnectToDb(wxWindow *parent, wxString &name, wxString &engine)
{
    std::vector<std::wstring> errorMsg;
    bool ask = false;
    Database *pdb = NULL;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#else
    lib.Load( "libdialogs" );
#endif
    if( lib.IsLoaded() )
    {
        DBPROFILE func = (DBPROFILE) lib.GetSymbol( "DatabaseProfile" );
        int result = func( parent, _( "Select Database Profile" ), name, engine, ask );
        if( result != wxID_CANCEL )
        {
            if( engine == "SQLite" )
            {
                pdb = new SQLiteDatabase();
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
            if( engine == "ODBC" )
            {
                pdb = new ODBCDatabase();
                dynamic_cast<ODBCDatabase *>( pdb )->SetWindowHandle( parent->GetHandle() );
                result = pdb->Connect( name.ToStdWstring(), errorMsg );
                if( result )
                {
                    pdb->Disconnect( errorMsg );
                    delete pdb;
                    pdb = NULL;
                    result = 1;
                    for( std::vector<std::wstring>::iterator it = errorMsg.begin(); it < errorMsg.end(); it++ )
                    {
                        wxMessageBox( (*it) );
                    }
                    wxMessageBox( _( L"Problem retrieving database information. Please check the database is running and operational and try again" ) );
                }
            }
        }
    }
    return pdb;
}

/*extern "C" */WXEXPORT void DisconnectFromDB(Database *db, const wxString &engine)
{
    delete db;
    db = NULL;
//    std::vector<std::wstring> errorMsg;
//    db->Disconnect( errorMsg );
}
