#pragma once
class WXEXPORT FieldGeneral : public PropertyPageBase
{
public:
    FieldGeneral(wxWindow *parent, Field *field);
    ~FieldGeneral();
    bool IsLogOnly();
    void OnCommentChanged(wxCommandEvent &event);
protected:
    void set_properties();
    void do_layout();
private:
    wxStaticText *m_label;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
    Field *m_field;
};

