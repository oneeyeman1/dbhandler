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
