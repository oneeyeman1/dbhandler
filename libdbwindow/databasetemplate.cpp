#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "databasetemplate.h"

DrawingView *DatabaseTemplate::CreateDatabaseView(wxDocument *doc, ViewType type, long flags)
{
    wxScopedPtr<DrawingView> view( DoCreateView() );
    if( !view )
        return NULL;
    view->SetViewType( type );
    view->SetDocument( doc );
    if( !view->OnCreate( doc, flags ) )
        return NULL;
    return view.release();
}

bool CreateDatabaseDocument(const wxString &path, ViewType type, long flags)
{
    DrawingDocument * const doc = DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        if( CreateDatabaseView( doc, type, flags ) )
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