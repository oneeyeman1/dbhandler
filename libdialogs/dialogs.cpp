#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
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
#ifdef __WXGTK__
#include "gtk/gtk.h"
#include "wx/nativewin.h"
#endif
#ifdef __WXOSX_COCOA__
#include "wx/nativewin.h"
#endif
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "wx/fontenum.h"
//#include "fontpropertypagebase.h"
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
#include "addcolumnsdialog.h"

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

extern "C" WXEXPORT int DatabaseProfile(wxWindow *parent, const wxString &title, wxString &name, wxString &dbEngine, wxString &connectedUser, bool ask, const std::vector<std::wstring> &dsn)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    DatabaseType dlg( parent, title, name, dbEngine, dsn );
    bool result = dlg.RunWizard( dlg.GetFirstPage() );
    if( result )
    {
        wxTextCtrl *userId = dlg.GetUserControl();
        if( userId )
        {
            connectedUser = userId->GetValue();
        }
        else
            connectedUser = "";
        dlg.GetDatabaseEngine( dbEngine );
        name = dlg.GetDatabaseName();
        ask = dlg.GetODBCConnectionParam();
        if( name.empty() )
            name = dlg.GetConnectString();
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

extern "C" WXEXPORT int CreateIndexForDatabase(wxWindow *parent, DatabaseTable *table, Database *db, wxString &command, wxString &indexName)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    CreateIndex dlg( parent, wxID_ANY, "", table, table->GetSchemaName(), db );
    dlg.Center();
    res = dlg.ShowModal();
    if( res != wxID_CANCEL )
    {
        command = dlg.GetCommand();
        indexName = dlg.GetIndexNameCtrl()->GetValue();
    }
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

extern "C" WXEXPORT int CreateForeignKey(wxWindow *parent, wxString &keyName, DatabaseTable *table, std::vector<FKField *> &fkfield, Database *db, wxString &command, bool &logOnly)
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
        keyName = dlg.GetKeyNameCtrl()->GetValue();
        fkfield = dlg.GetForeignKeyVector();
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

extern "C" WXEXPORT int AddColumnToQuery(wxWindow *parent, int type, const std::vector<std::wstring> &fields, wxString &selection, const wxString &dbType, const wxString &dbSubtype)
{
    int res;
    AddColumnsDialog dlg( parent, type, fields, dbType, dbSubtype );
    res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        wxListBox *fields = dlg.GetFieldsControl();
        selection = fields->GetString( fields->GetSelection() );
    }
	else
        selection = wxEmptyString;
    return res;
}