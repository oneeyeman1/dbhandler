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
#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wx/grid.h"
#include "database.h"
#include "dataretriever.h"
#include "dbtableedit.h"
#include "tableeditdocument.h"
#include "tableeditview.h"
#include "dataeditdoctemplate.h"

DataEditDocTemplate::DataEditDocTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo)
: wxDocTemplate( manager, descr, filter, dir, ext, docTypeName, viewTypeName, docClassInfo, viewClassInfo)
{
}

bool DataEditDocTemplate::CreateDataEditDocument(const wxString &path, int flags, Database *db, const wxString &tableName)
{
    TableEditDocument *const doc = (TableEditDocument *) DoCreateDocument();
    wxTRY
    {
        doc->SetFilename( path );
        doc->SetDocumentTemplate( this );
        GetDocumentManager()->AddDocument( doc );
        doc->SetDatabaseAndTableName( db, tableName );
        doc->SetCommandProcessor( doc->OnCreateCommandProcessor() );
        wxScopedPtr<TableEditView> view( (TableEditView *) DoCreateView() );
        if( !view )
            return true;
        view->SetDocument( doc );
        if( !view->OnCreate( doc, flags ) )
            return true;
        if( view.release() )
            return true;
        return false;
    }
    wxCATCH_ALL
    (
        if( GetDocumentManager()->GetDocuments().Member( doc ) )
            doc->DeleteAllViews();
        throw;
    )
}
