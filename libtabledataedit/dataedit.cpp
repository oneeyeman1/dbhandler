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
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/grid.h"
#include "wx/dynlib.h"
#include "database.h"
#include "configuration.h"
#include "tableattributes.h"
#include "tableeditview.h"
#include "dataeditdoctemplate.h"
#include "tableeditdocument.h"

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
#if defined __MEMORYLEAKS__
        _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif
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

extern "C" WXEXPORT void CreateDataEditWindow(wxWindow *parent, wxDocManager *docManager, Database *db, ViewType type, Configuration *conf)
{
    DataEditDocTemplate *docTemplate;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    docTemplate = (DataEditDocTemplate *) docManager->FindTemplate( CLASSINFO( TableEditDocument ) );
    if( !docTemplate )
    {
        docTemplate = new DataEditDocTemplate( docManager, "TableEdit", "*.edt", "", "edt", "TableEdit Doc", "TableEdit View", CLASSINFO( TableEditDocument ), CLASSINFO( TableEditView ) );
    }
    docTemplate->CreateDataEditDocument( "*.edt", wxDOC_NEW | wxDOC_SILENT, db, type, conf );
    dynamic_cast<TableEditDocument *>( docManager->GetCurrentDocument() )->SetDatabase( db );
    dynamic_cast<TableEditView *>( docManager->GetCurrentDocument()->GetFirstView() )->GetTablesForView( db, true );
//    auto view = docManager->GetCurrentView();
//    if( view )
//        dynamic_cast<TableEditView *>( view )->SetProfiles( profiles );
}
