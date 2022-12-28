#ifndef __GETOBJECTNAME_H__
#define __GETOBJECTNAME_H__

#define wxID_NEWOBJECT 1000

class GetObjectName : public wxDialog
{
public:
    GetObjectName(wxWindow *parent, int id, const wxString &title, int objectId);
    const wxString &GetFileName();
    const int GetSource();
    const int GetPresentation();
protected:
    void set_properties();
    void do_layout();
    void OnButtonNew(wxCommandEvent &event);
    void OnButtonBrowse(wxCommandEvent &event);
    void OnOKButton(wxCommandEvent &event);
    void OnOKButtonUpdateUI(wxUpdateUIEvent &event);
private:
    wxPanel *m_panel;
    wxStaticText *m_painterNameLabel, *m_comments;
    wxTextCtrl *m_painterName, *m_commentsText;
    wxListCtrl *m_objectList;
    wxButton *m_ok, *m_cancel, *m_new, *m_browse, *m_help;
    int m_id, m_source, m_presentation;
    wxString m_fileName;
};
#endif