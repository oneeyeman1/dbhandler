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
#include <string>
#include <memory>
#include "wx/grid.h"
#include "wx/docmdi.h"
#include "wx/docview.h"
#include "wx/dynlib.h"
#include "wx/cmdproc.h"
#include "database.h"
#include "tabledoc.h"
#include "tableview.h"
#include "tablecanvas.h"

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

extern "C" WXEXPORT void CreateDatabaseWindow(wxWindow *parent, wxDocManager *docManager, Database *db, DatabaseTable *table, const wxString &fieldName)
{
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    wxDocTemplate *docTemplate = docManager->FindTemplate( CLASSINFO(TableDocument ) );
    if( !docTemplate )
        new wxDocTemplate( docManager, "Drawing", "*.tbl", "", "tbl", "Table Doc", "Table View", CLASSINFO(TableDocument), CLASSINFO(TableView) );
    docManager->CreateDocument( "*.tbl", wxDOC_NEW | wxDOC_SILENT );
    dynamic_cast<TableDocument *>( docManager->GetCurrentDocument() )->SetDatabase( db, table );
    new TableCanvas( dynamic_cast<TableView *>( docManager->GetCurrentView() ), wxDefaultPosition, db, table, fieldName );
}

/*
extern "C" WXEXPORT void ODBCSetup(wxWindow *pParent)
{
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( pParent );
#endif
    CODBCConfigure dlg( pParent, wxID_ANY, _T( "Configure ODBC" ) );
    dlg.ShowModal();
}
*/
