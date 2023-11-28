//
//  dataeditdoctemplate.hpp
//  libtabledataedit
//
//  Created by Igor Korot on 2/17/21.
//  Copyright © 2021 Igor Korot. All rights reserved.
//

#ifndef dataeditdoctemplate_hpp
#define dataeditdoctemplate_hpp

class DataEditDocTemplate : public wxDocTemplate
{
public:
    DataEditDocTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo=0, wxClassInfo *viewClassInfo=0);
    TableEditView *CreateDatabaseView(wxDocument *doc, ViewType type, ToolbarSetup &tbSetup, long flags);
    bool CreateDataEditDocument(const wxString &path, int flags, Database *db, ViewType type, ToolbarSetup &tbSetup);
};

#endif /* dataeditdoctemplate_hpp */
