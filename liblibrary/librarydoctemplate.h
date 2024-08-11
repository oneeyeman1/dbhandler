#pragma once
class LibraryDocTemplate : public wxDocTemplate
{
public:
    LibraryDocTemplate(wxDocManager *manager, const wxString &descr, const wxString &filter, const wxString &dir, const wxString &ext, const wxString &docTypeName, const wxString &viewTypeName, wxClassInfo *docClassInfo, wxClassInfo *viewClassInfo, long flags);
    LibraryViewPainter *CreateLibraryView(wxWindow *parent, wxDocument *doc, ViewType type,  Configuration *conf, long flags);
    bool CreateDatabaseDocument(wxWindow *parent, const wxString &path, ViewType type, Configuration *conf, long flags);
};

