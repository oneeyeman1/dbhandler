#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifdef _WINDOWS
BOOL APIENTRY DllMain( HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
    lpReserved = lpReserved;
    hModule = hModule;
    int argc = 0;
    char **argv = NULL;
    switch( fdwReason )
    {
    case DLL_PROCESS_ATTACH:
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
#endif
