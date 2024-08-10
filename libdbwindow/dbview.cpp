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
#include "wx/fontenum.h"
#include "wx/docview.h"
#include "wx/cmdproc.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/bmpcbox.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "configuration.h"
#include "objectproperties.h"
#include "constraint.h"
#include "constraintsign.h"
#include "table.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "FieldShape.h"
#include "field.h"
#include "fieldwindow.h"
#include "sortgroupbypage.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
#include "designcanvas.h"
#include "databaseview.h"
#include "databasetemplate.h"

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

extern "C" WXEXPORT void CreateDatabaseWindow(wxWindow *parent, wxDocManager *docManager, Database *db, ViewType type, std::map<wxString, wxDynamicLibrary *> &painters, const std::vector<Profile> &profiles, const std::vector<QueryInfo> &queries, std::vector<LibrariesInfo> &path, Configuration *conf)
{
    DatabaseTemplate *docTemplate;
#ifdef __WXMSW__
    wxTheApp->SetTopWindow( parent );
#endif
    docTemplate = (DatabaseTemplate *) docManager->FindTemplate( CLASSINFO( DrawingDocument ) );
    if( !docTemplate )
    {
        if( type == DatabaseView )
            docTemplate = new DatabaseTemplate( docManager, "Drawing", "*.drw", "", "drw", "Drawing Doc", "Drawing View", CLASSINFO( DrawingDocument ), CLASSINFO( DrawingView ) );
        else
            docTemplate = new DatabaseTemplate( docManager, "Drawing", "*.qry", "", "qry", "Drawing Doc", "Drawing View", CLASSINFO( DrawingDocument ), CLASSINFO( DrawingView ) );
    }
    bool init;
    if( type == DatabaseView )
    {
        docTemplate->CreateDatabaseDocument( parent, "*.drw", type, db, painters, queries, path, conf, wxDOC_NEW | wxDOC_SILENT );
        init = false;
    }
    else
    {
        docTemplate->CreateDatabaseDocument( parent, "*.qry", type, db, painters, queries, path,  conf, wxDOC_NEW | wxDOC_SILENT );
        init = true;
    }
    dynamic_cast<DrawingDocument *>( docManager->GetCurrentDocument() )->SetDatabase( db );
    dynamic_cast<DrawingView *>( docManager->GetCurrentDocument()->GetFirstView() )->GetTablesForView( db, init, queries, path );
    auto view = docManager->GetCurrentView();
    if( view )
        dynamic_cast<DrawingView *>( view )->SetProfiles( profiles );
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
