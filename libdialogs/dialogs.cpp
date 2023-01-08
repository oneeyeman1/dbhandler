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
#include <list>
#include <memory>
#include "wx/wizard.h"
#include "wx/filepicker.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/listctrl.h"
#include "wx/notebook.h"
#include "wx/spinctrl.h"
#include "wx/bmpcbox.h"
#include "wx/grid.h"
#ifdef __WXGTK__
#include "gtk/gtk.h"
#include "wx/nativewin.h"
#if !GTK_CHECK_VERSION(3,6,0)
#include "wx/generic/stattextg.h"
#endif
#endif
#ifdef __WXOSX__
#include "wx/fontpicker.h"
#endif
#ifndef __WXMSW__
#include "odbccredentials.h"
#endif
#include "database.h"
#include "wxsf/ShapeCanvas.h"
#include "wx/fontenum.h"
#include "dialogs.h"
#include "objectproperties.h"
#include "colorcombobox.h"
#include "databasetype.h"
#include "field.h"
#include "fieldwindow.h"
#include "propertypagebase.h"
#include "tablegeneral.h"
#include "tableprimarykey.h"
#include "odbcconfigure.h"
#include "selecttables.h"
#include "createindex.h"
#include "bandgeneral.h"
#include "fontpropertypagebase.h"
#include "pointerproperty.h"
#include "fieldgeneral.h"
#include "fieldheader.h"
#include "foreignkey.h"
#include "getobjectname.h"
#include "jointype.h"
#include "designlabelgeneral.h"
#include "propertieshandlerbase.h"
#include "propertieshandler.h"
#include "properties.h"
#include "addcolumnsdialog.h"
#include "bitmappanel.h"
#include "newquery.h"
#include "quickselect.h"
#include "typecombobox.h"
#include "retrievalarguments.h"
#include "gotoline.h"
#include "attachdb.h"
#include "detachdb.h"
#include "datasource.h"

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

extern "C" WXEXPORT int DatabaseProfile(wxWindow *parent, const wxString &title, wxString &name, wxString &dbEngine, wxString &connectedUser, bool ask, const std::vector<std::wstring> &dsn, std::vector<Profile> &profiles)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    DatabaseType dlg( parent, title, name, dbEngine, dsn, profiles );
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
        for( auto profile : profiles )
        {
            if( profile.m_isCurrent )
            {
                profile.m_isCurrent = !profile.m_isCurrent;
                break;
            }
        }
        profiles.push_back( Profile( dlg.GetSelectedProfile(), true ) );
        res = wxID_OK;
    }
	else
        res = wxID_CANCEL;
    return res;
}

extern "C" WXEXPORT int SelectTablesForView(wxWindow *parent, Database *db, std::map<wxString, std::vector<TableDefinition> >&tableNames, std::vector<std::wstring> &names, bool isTableView, const int type)
{
    int res;
    std::vector<std::wstring> errors;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    SelectTables dlg( parent, wxID_ANY, "", db, names, isTableView, type );
    if( isTableView )
        dlg.Center();
	res = dlg.ShowModal();
    std::wstring catalog;
    if( db->GetTableVector().GetDatabaseType() == L"SQLite" )
        catalog = L"";;
    if( res != wxID_CANCEL )
    {
        wxBusyCursor wait;
        dlg.GetSelectedTableNames( tableNames );
        for( auto cat : tableNames )
        {
            for( auto def : cat.second )
            {
                db->AddDropTable( cat.first.ToStdWstring(), def.schemaName, def.tableName, errors );
            }
        }
    }
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

extern "C" WXEXPORT int CreatePropertiesDialog(wxWindow *parent, std::unique_ptr<PropertiesHandler> &handler, const wxString &title, wxString &command, bool logOnly, wxCriticalSection &cs)
{
    int res = 0;
    PropertiesDialog dlg( parent, wxID_ANY, title, handler.get(), cs );
	dlg.Center();
    res = dlg.ShowModal();
    return res;
}

extern "C" WXEXPORT int CreateForeignKey(wxWindow *parent, wxString &keyName, DatabaseTable *table, std::vector<std::wstring> &foreignKeyFields, std::vector<std::wstring> &refKeyFields, std::wstring &refTableName, int &deleteProp, int &updateProp, Database *db, bool &logOnly, bool isView, std::vector<FKField *> &newFK, int &match)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    wxString refTblName = wxString( refTableName );
    ForeignKeyDialog dlg( parent, wxID_ANY, _( "" ), table, db, keyName, foreignKeyFields, refTblName, isView, match );
    dlg.Center();
    res = dlg.ShowModal();
    if( res != wxID_CANCEL || dlg.IsForeignKeyEdited() )
    {
        logOnly = dlg.IsLogOnlyI();
        keyName = dlg.GetKeyNameCtrl()->GetValue();
        if( !isView )
        {
            foreignKeyFields = dlg.GetForeignKeyFields();
            refKeyFields = dlg.GetPrimaryKeyFields();
            refTableName = dlg.GetReferencedTable();
            deleteProp = dlg.GetDeleteParam();
            updateProp = dlg.GetUpdateParam();
            match = dlg.GetMatchingOptions();
		}
        newFK = dlg.GetForeignKeyVector();
    }
    return res;
}

extern "C" WXEXPORT int ChooseObject(wxWindow *parent, int objectId, const std::vector<QueryInfo> &queries, wxString &documentName, wxString comments)
{
    int res;
    wxString title;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    switch( objectId )
    {
    case 1:
        title = _( "Query" );
        break;
    case -1:
        title = _( "Save Query" );
        break;
    default:
        break;
    }
    GetObjectName dlg( parent, wxID_ANY, title, objectId, queries );
    dlg.Center();
    res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        documentName = dlg.GetDocumentName()->GetValue();
        comments = dlg.GetCommentObject()->GetValue();
    }
    return res;
}

extern "C" WXEXPORT int NewQueryDlg(wxWindow *parent, int &source, int &presentation)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    NewQuery dlg( parent, _( "New Query" ), source, presentation );
    dlg.Center();
    res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        source = dlg.GetSource();
        presentation = dlg.GetPresentation();
    }
    return res;
}

extern "C" WXEXPORT int QuickSelectDlg(wxWindow *parent, const Database *db, std::vector<DatabaseTable *> &tableName, std::vector<TableField *> &queryFields, std::vector<FieldSorter> &allSorted, std::vector<FieldSorter> &querySorted)
{
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    QuickSelect dlg( parent, db );
    dlg.Center();
    int res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        tableName.push_back( dlg.GetDatabaseTable() );
        queryFields = dlg.GetQueryFields();
        allSorted = dlg.GetAllSorted();
        querySorted = dlg.GetQuerySorted();
    }
    return res;
}

extern "C" WXEXPORT int SelectJoinType(wxWindow *parent, const wxString &origTable, const wxString &refTable, const wxString &origField, const wxString &refField, long &type)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    JointType dlg( dynamic_cast<wxDocMDIChildFrame *>( parent ), wxID_ANY, _( "Join" ), origTable, refTable, origField, refField, type );
    res = dlg.ShowModal();
    if( res == wxID_OK )
        type = dlg.GetTypeCtrl()->GetNextItem( -1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED );
    return res;
}

extern "C" WXEXPORT int AddColumnToQuery(wxWindow *parent, int type, const wxPoint &pos, const std::vector<std::wstring> &fields, wxString &selection, const wxString &dbType, const wxString &dbSubtype, const std::vector<QueryArguments> &args)
{
    int res;
    AddColumnsDialog dlg( parent, type, pos, fields, dbType, dbSubtype, args );
    res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        wxListBox *flds = dlg.GetFieldsControl();
        selection = flds->GetString( flds->GetSelection() );
    }
    else
        selection = wxEmptyString;
    return res;
}

extern "C" WXEXPORT int GetODBCCredentails(wxWindow *parent, const wxString &dsn, wxString &userID, wxString &password)
{
    int res = wxID_OK;
#ifndef __WXMSW__
    ODBCCredentials dlg( dynamic_cast<wxDocMDIParentFrame *>( parent )->GetActiveChild(), wxID_ANY, L"", dsn, userID, password );
    res = dlg.ShowModal();
    if( res == wxID_OK )
    {
        userID = dlg.GetUserIDControl().GetValue();
        password = dlg.GetPasswordControl().GetValue();
    }
#endif
    return res;
}

extern "C" WXEXPORT int GetQueryArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType)
{
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    RetrievalArguments dlg( dynamic_cast<wxDocMDIParentFrame *>( parent )->GetActiveChild(), arguments, dbType, subType );
    dlg.Center();
    int result = dlg.ShowModal();
    if( result == wxID_OK )
    {
        std::list<QueryLines> lines = dlg.GetArgumentLines();
        arguments.clear();
        for( std::list<QueryLines>::iterator it = lines.begin(); it != lines.end(); ++it )
        {
            arguments.clear();
            arguments.push_back( QueryArguments( wxAtoi( (*it).m_number->GetLabel() ), (*it).m_name->GetValue(), (*it).m_type->GetValue() ) );
        }
    }
    return result;
}

extern "C" WXEXPORT int GotoLine(wxWindow *parent, int &lineNo)
{
    int result;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    GoToDialog dlg( dynamic_cast<wxDocMDIParentFrame *>( parent )->GetActiveChild() );
    result = dlg.ShowModal();
    if( result == wxID_OK )
        lineNo = wxAtoi( dlg.GetLineNumberCtrl()->GetValue() );
    return result;
}

extern "C" WXEXPORT int AttachToDatabase(wxWindow *parent, Database *db)
{
    int result = 0;
    std::vector<std::wstring> errorMsg;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    std::vector<std::wstring> names;
    if( db->GetDatabaseNameList(names, errorMsg) )
    {
        for( auto msg : errorMsg )
        {
            wxMessageBox( msg, _( "Retrieving database list" ), wxICON_ERROR );
            result = wxID_CANCEL;
        }
    }
    else
    {
        AttachDB dlg( dynamic_cast<wxDocMDIParentFrame *>( parent )->GetActiveChild(), db, names );
        dlg.Center();
        result = dlg.ShowModal();
        if( result == wxID_OK )
        {
            wxBusyCursor cursor;
            if( db->AttachDatabase(dlg.GetCatalog().ToStdWstring(), dlg.GetSchea().ToStdWstring(), errorMsg) )
            {
                for( auto error : errorMsg )
                    wxMessageBox( error, _( "Attaching DB error" ), wxICON_ERROR );
            }
        }
    }
    return result;
}

extern "C" WXEXPORT int DetachDatabase(wxWindow *parent)
{
    int result;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    DetachDB dlg( dynamic_cast<wxDocMDIParentFrame *>( parent )->GetActiveChild() );
    result = dlg.ShowModal();
    return result;
}

extern "C" WXEXPORT int GetDataSource(wxWindow *parent, wxString &dataSource, const std::vector<Profile> &profiles)
{
    int res;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    DataSorces dlg( parent, wxID_ANY, "", profiles );
    if( ( res = dlg.ShowModal() ) == wxID_OK )
        dataSource = dlg.GetDataSource();
    return res;
}
