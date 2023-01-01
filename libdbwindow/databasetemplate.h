#ifndef __DATABASEQUERYTEMPLATE__H
#define __DATABASEQUERYTEMPLATE__H

class DatabaseTemplate : public wxDocTemplate
{
public:
    DatabaseTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo=0, wxClassInfo *viewClassInfo=0, long flags=wxTEMPLATE_VISIBLE);
    bool CreateDatabaseDocument(const wxString &path, ViewType type, Database *db, std::map<wxString, wxDynamicLibrary *> &painters, const std::vector<QueryInfo> &queriess, long flags = 0);
    DrawingView *CreateDatabaseView(wxDocument *doc, ViewType type, std::map<wxString, wxDynamicLibrary *> &painter, const std::vector<QueryInfo> &queriess, long flags = 0);
};

#endif