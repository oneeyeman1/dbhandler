#ifndef __GETOBJECTNAME_H__
#define __GETOBJECTNAME_H__

class GetObjectName : public wxDialog
{
public:
    GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId);
    void OnButtonNew(wxCommandEvent &event);
    void OnButtonBrowse(wxCommandEvent &event);
    const wxString &GetFileName();
protected:
    void set_properties();
    void do_layout();
private:
    wxPanel *m_panel;
    wxStaticText *m_painterNameLabel, *m_comments;
    wxTextCtrl *m_painterName, *m_commentsText;
    wxListCtrl *m_objectList;
    wxButton *m_ok, *m_cancel, *m_new, *m_browse, *m_help;
    int m_id;
    wxString m_fileName;
};
#endif