#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <memory>
#include "wx/scopedptr.h"
#include "wx/except.h"
#include "wx/docview.h"
#include "wx/dynlib.h"
#include "wx/fontenum.h"
#include "wx/docmdi.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/bmpcbox.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "configuration.h"
#include "propertieshandlerbase.h"
#include "guiobjectsproperties.h"
#include "ablbaseview.h"
#include "constraint.h"
#include "constraintsign.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "database.h"
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

DatabaseTemplate::DatabaseTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo, long flags)
  : wxDocTemplate(manager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo)
{
}

DrawingView *DatabaseTemplate::CreateDatabaseView(wxWindow *parent, wxDocument *doc, ViewType type, std::map<wxString, wxDynamicLibrary *> &painters, Configuration *conf, long flags)
{
    wxScopedPtr<DrawingView> view( (DrawingView *) DoCreateView() );
    if( !view )
        return NULL;
    view->SetViewType( type );
    view->SetParentWindow( parent );
    view->SetDocument( doc );
    view->SetPaintersMap( painters );
    view->SetToolbarOptions( conf );
    if( !view->OnCreate( doc, flags ) )
        return NULL;
    return view.release();
}

bool DatabaseTemplate::CreateDatabaseDocument(wxWindow *parent, const wxString &path, ViewType type, Database *db, std::map<wxString, wxDynamicLibrary *> &painters, const std::vector<QueryInfo> &queriess, std::vector<LibrariesInfo> &libPath, Configuration *conf, long flags)
{
    DrawingDocument * const doc = (DrawingDocument *) DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetDatabase( db/*, true, queriess, libPath*/ );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        if( CreateDatabaseView( parent, doc, type, painters, conf, flags ) )
            return true;
        if( GetDocumentManager()->GetDocuments().Member( doc ) )
            doc->DeleteAllViews();
        return false;
    }
    wxCATCH_ALL
    (
        if( GetDocumentManager()->GetDocuments().Member( doc ) )
            doc->DeleteAllViews();
        throw;
    )
}

bool DatabaseTemplate::CreateDatabaseDocument(wxWindow *parent, const wxString &path, ViewType type, Database *db, Configuration *conf, long flags)
{
    DrawingDocument * const doc = (DrawingDocument *) DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetDatabase( db );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        if( CreateDatabaseView( parent, doc, type, conf, flags ) )
            return true;
        if( GetDocumentManager()->GetDocuments().Member( doc ) )
            doc->DeleteAllViews();
        return false;
    }
    wxCATCH_ALL
    (
        if( GetDocumentManager()->GetDocuments().Member( doc ) )
            doc->DeleteAllViews();
        throw;
     )
}

DrawingView *DatabaseTemplate::CreateDatabaseView(wxWindow *parent, wxDocument *doc, ViewType type,  Configuration *conf, long flags)
{
    wxScopedPtr<DrawingView> view( (DrawingView *) DoCreateView() );
    if( !view )
        return NULL;
    view->SetViewType( type );
    view->SetParentWindow( parent );
    view->SetDocument( doc );
    view->SetToolbarOptions( conf );
    if( !view->OnCreate( doc, flags ) )
        return NULL;
    return view.release();
}
