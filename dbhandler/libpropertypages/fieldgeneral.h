#pragma once
class WXEXPORT FieldGeneral : public PropertyPageBase
{
public:
    FieldGeneral(wxWindow *parent, const wxString &comment);
    ~FieldGeneral();
    bool IsLogOnly();
    void OnCommentChanged(wxCommandEvent &event);
    wxTextCtrl *GetCommentCtrl();
protected:
    void set_properties();
    void do_layout();
private:
    wxStaticText *m_label;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
    wxString m_commentText;
};

