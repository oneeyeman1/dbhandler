//
//  dataeditdoctemplate.cpp
//  libtabledataedit
//
//  Created by Igor Korot on 2/17/21.
//  Copyright © 2021 Igor Korot. All rights reserved.
//
#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include <map>
#include <string>
#include "wx/except.h"
#include "wx/scopedptr.h"
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/grid.h"
#include "database.h"
#include "configuration.h"
#include "tableattributes.h"
#include "tableeditdocument.h"
#include "tableeditview.h"
#include "dataeditdoctemplate.h"

DataEditDocTemplate::DataEditDocTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo)
: wxDocTemplate( manager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo)
{
}

TableEditView *DataEditDocTemplate::CreateDatabaseView(wxDocument *doc, ViewType type, Configuration *conf, long flags)
{
    wxScopedPtr<TableEditView> view( (TableEditView *) DoCreateView() );
    if( !view )
        return NULL;
    view->SetViewType( type );
    view->SetDocument( doc );
    view->SetToolbarOption( conf );
    if( !view->OnCreate( doc, flags ) )
        return NULL;
    return view.release();
}


bool DataEditDocTemplate::CreateDataEditDocument(const wxString &path, int flags, Database *db, ViewType type, Configuration *conf)
{
    TableEditDocument *const doc = (TableEditDocument *) DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetDatabaseAndTableName( db );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        if( CreateDatabaseView( doc, type, conf, flags ) )
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
