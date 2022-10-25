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

/*#include <map>
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
*/
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
