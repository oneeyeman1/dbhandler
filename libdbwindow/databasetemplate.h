#ifndef __DATABASEQUERYTEMPLATE__H
#define __DATABASEQUERYTEMPLATE__H

class DatabaseTemplate : public wxDocTemplate
{
    bool CreateDatabaseDocument(const wxString &path, ViewType type, long flags = 0);
    wxView *CreateDatabaseView(wxDocument *doc, ViewType type, long flags = 0);
};

#endif