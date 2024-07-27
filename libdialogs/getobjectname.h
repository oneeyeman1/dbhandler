#ifndef __GETOBJECTNAME_H__
#define __GETOBJECTNAME_H__

#define wxID_NEWOBJECT 1000

class GetObjectName : public wxDialog
{
public:
    GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId, const std::vector<QueryInfo> &queries, const std::vector<LibrariesInfo> &path);
    const wxString &GetFileName() const;
    const int GetSource() const;
    const int GetPresentation() const;
    const wxString &GetDocumentName() const;
    const wxTextCtrl *GetCommentObject() const;
    std::vector<LibrariesInfo> &GetLibrariesVector() { return m_path; }
    bool isUpdating();
protected:
    void set_properties();
    void OnButtonNew(wxCommandEvent &event);
    void OnButtonBrowse(wxCommandEvent &event);
    void OnOKButton(wxCommandEvent &event);
    void OnOKButtonUpdateUI(wxUpdateUIEvent &event);
    void OnNameSelected(wxListEvent &event);
    void OnNameActivated(wxListEvent &event);
private:
    wxPanel *m_panel;
    wxStaticText *m_painterNameLabel, *m_comments;
    wxTextCtrl *m_painterName, *m_commentsText;
    wxListCtrl *m_objectList, *m_librariesList;
    wxButton *m_ok, *m_cancel, *m_new, *m_browse, *m_help, *m_browseLibs;
    int m_id, m_source, m_presentation;
    wxString m_fileName, m_title;
    std::vector<LibrariesInfo> m_path;
    wxString m_objectFileName;
};
#endif
