#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#ifdef __WXMSW__
#include <vld.h>
#endif

#include <map>
#include <vector>
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/dynlib.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/bmpcbox.h"
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "fontpropertypagebase.h"
#include "databasetype.h"
#include "tablegeneral.h"
#include "odbcconfigure.h"
#include "selecttables.h"
#include "fieldwindow.h"
#include "createindex.h"
#include "fontpropertypagebase.h"
#include "foreignkey.h"
#include "getobjectname.h"
#include "jointype.h"
#include "properties.h"

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

extern "C" WXEXPORT int SelectTablesForView(wxWindow *parent, Database *db, std::vector<wxString> &tableNames, std::vector<std::wstring> &names, bool isTableView)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    SelectTables dlg( parent, wxID_ANY, "", db, names, isTableView );
    if( isTableView )
        dlg.Center();
	res = dlg.ShowModal();
    if( res != wxID_CANCEL )
        dlg.GetSelectedTableNames( tableNames );
    return res;
}

extern "C" WXEXPORT int CreateIndexForDatabase(wxWindow *parent, DatabaseTable *table, Database *db, wxString &command)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    CreateIndex dlg( parent, wxID_ANY, "", table, db );
    dlg.Center();
    res = dlg.ShowModal();
    if( res != wxID_CANCEL )
        command = dlg.GetCommand();
    return res;
}

extern "C" WXEXPORT int CreatePropertiesDialog(wxWindow *parent, Database *db, int type, void *object, wxString &command, bool logOnly, const wxString &tableName, const wxString &schemaName)
{
    wxString title;
    int res = 0;
    if( type == 0 )
    {
        title = _( "Table " );
        title += static_cast<DatabaseTable *>( object )->GetTableName();
    }
    if( type == 1 )
    {
        title = _( "Column " );
        title += tableName + ".";
        title += static_cast<Field *>( object )->GetFieldName();
    }
    PropertiesDialog dlg( parent, wxID_ANY, title, db, type, object, tableName, schemaName );
	dlg.Center();
    res = dlg.ShowModal();
    if( res != wxID_CANCEL )
    {
        command = dlg.GetCommand();
        logOnly = dlg.IsLogOnly();
    }
    return res;
}

extern "C" WXEXPORT int CreateForeignKey(wxWindow *parent, DatabaseTable *table, Database *db, wxString &command, bool &logOnly)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    ForeignKeyDialog dlg( parent, wxID_ANY, _( "" ), table, db );
    dlg.Center();
    res = dlg.ShowModal();
    if( res != wxID_CANCEL )
    {
        command = dlg.GetCommand();
        logOnly = dlg.IsLogOnlyI();
    }
    return res;
}

extern "C" WXEXPORT int ChooseObject(wxWindow *parent, int objectId)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    GetObjectName dlg( parent, wxID_ANY, _( "Query" ), objectId );
    res = dlg.ShowModal();
    return res;
}

extern "C" WXEXPORT int SelectJoinType(wxWindow *parent, const wxString &origTable, const wxString &refTable, const wxString &origField, const wxString &refField)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    JointType dlg( parent, wxID_ANY, _( "Join" ), origTable, refTable, origField, refField );
    res = dlg.ShowModal();
    return res;
}
