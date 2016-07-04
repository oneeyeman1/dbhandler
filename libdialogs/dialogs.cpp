#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <map>
#include <vector>
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/dynlib.h"
#include "database.h"
#include "databasetype.h"
#include "odbcconfigure.h"
#include "selecttables.h"

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

extern "C" WXEXPORT void ODBCSetup(wxWindow *pParent)
{
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( pParent );
#endif
    CODBCConfigure dlg( pParent, wxID_ANY, _( "Configure ODBC" ) );
    dlg.ShowModal();
}

extern "C" WXEXPORT int DatabaseProfile(wxWindow *parent, const wxString &title, wxString &name, wxString &dbEngine, bool ask, const std::vector<std::wstring> &dsn)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    DatabaseType dlg( parent, title, name, dbEngine, dsn );
    bool result = dlg.RunWizard( dlg.GetFirstPage() );
    if( result )
    {
        dlg.GetDatabaseEngine( dbEngine );
        name = dlg.GetDatabaseName();
        ask = dlg.GetODBCConnectionParam();
        res = wxID_OK;
    }
	else
        res = wxID_CANCEL;
    return res;
}

extern "C" WXEXPORT int SelectTablesForView(wxWindow *parent, Database *db)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    SelectTables dlg( parent, wxID_ANY, "", db );
	res = dlg.ShowModal();
    return res;
}