#include <map>
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "configuration.h"
#include "librarydocument.h"
#include "libraryview.h"
#include "librarydoctemplate.h"

LibraryDocTemplate::LibraryDocTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo, long flags)
    : wxDocTemplate(manager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo)
{
}

LibraryViewPainter *LibraryDocTemplate::CreateLibraryView(wxWindow *parent, wxDocument *doc, ViewType type,  Configuration *conf, long flags)
{
    wxScopedPtr<LibraryViewPainter> view( (LibraryViewPainter *) DoCreateView() );
    if( !view )
        return NULL;
    view->SetViewType( type );
    view->SetParentWindow( parent );
    view->SetDocument( doc );
    view->SetConfiguration( conf );
    if( !view->OnCreate( doc, flags ) )
        return NULL;
    return view.release();
}

bool LibraryDocTemplate::CreateDatabaseDocument(wxWindow *parent, const wxString &path, ViewType type, Configuration *conf, long flags)
{
    LibraryDocument * const doc = (LibraryDocument *) DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        if( CreateLibraryView( parent, doc, type, conf, flags ) )
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
