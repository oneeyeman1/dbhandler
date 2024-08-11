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

#include <map>
#include <vector>
#include <string>
#include <memory>
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/bmpcbox.h"
#include "wx/treectrl.h"
#include "configuration.h"
#include "librarydocument.h"
#include "libraryview.h"

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

extern "C" WXEXPORT void CreateLibraryWindow(wxWindow *parent, wxDocManager *docManager, std::map<wxString, wxDynamicLibrary *> &painter, Configuration *conf)
{
    wxDocTemplate *docTemplate;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    wxString path;
    docTemplate = (wxDocTemplate *) docManager->FindTemplate( CLASSINFO( wxDocTemplate ) );
    if( !docTemplate )
    {
        docTemplate = new wxDocTemplate( docManager, "Library", "*.abl", "", "abl", "Library Doc", "Library View", CLASSINFO( LibraryDocument ), CLASSINFO( LibraryViewPainter ) );
        path = conf->m_currentLibrary;
        LibraryDocument * const doc = (LibraryDocument *) docTemplate->CreateDocument( path );
        wxTRY
        {
            doc->SetFilename( path.IsEmpty() ? wxGetCwd() + "/library1.abl" : path );
            doc->SetDocumentTemplate( docTemplate );
            docManager->AddDocument( doc );
            doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );

            wxScopedPtr<LibraryViewPainter> view( (LibraryViewPainter *) docTemplate->CreateView( doc ) );
            if( !view )
                return;
            view->SetViewType( LibraryView );
            view->SetParentWindow( parent );
            view->SetDocument( doc );
            view->SetConfiguration( conf );
            if( !view->OnCreate( doc, wxDOC_NEW | wxDOC_SILENT ) )
            {
                if( docManager->GetDocuments().Member( doc ) )
                    doc->DeleteAllViews();
                return;
            }
            else
            {
                view.release();
                return;
            }
        }
        wxCATCH_ALL
        (
            if( docManager->GetDocuments().Member( doc ) )
                doc->DeleteAllViews();
            throw;
        )
    }
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
