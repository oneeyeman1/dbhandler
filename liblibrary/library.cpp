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
#include "wx/docmdi.h"
#include "wx/dir.h"
#include "wx/filename.h"
#include "wx/dynlib.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/generic/dirctrlg.h"
#include "wx/bmpcbox.h"
#include "wx/treelist.h"
#include "database.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "painterobjects.h"
#include "ablbaseview.h"
#include "propertieshandlerbase.h"
#include "librarydocument.h"
#include "libraryview.h"
#include "librarydoctemplate.h"

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

extern "C" WXEXPORT void CreateLibraryWindow(wxWindow *parent, wxDocManager *docManager, ViewType type, std::map<wxString, wxDynamicLibrary *> &painter, Configuration *conf, LibraryObject *library)
{
    LibraryDocTemplate *docTemplate;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    wxString path;
    docTemplate = (LibraryDocTemplate *) docManager->FindTemplate( CLASSINFO( wxDocTemplate ) );
    if( !docTemplate )
    {
        docTemplate = new LibraryDocTemplate( docManager, "Library", "*.abl", "", "abl", "Library Doc", "Library View", CLASSINFO( LibraryDocument ), CLASSINFO( LibraryViewPainter ), wxDOC_NEW | wxDOC_SILENT );
        path = conf->m_currentLibrary.IsEmpty() ? wxGetCwd() + "/library1.abl" : conf->m_currentLibrary;
        docTemplate->CreateLibraryDocument( parent, path, type, painter, conf, library, wxDOC_NEW | wxDOC_SILENT );
    }
}
