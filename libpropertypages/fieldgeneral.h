#pragma once
class WXEXPORT FieldGeneral : public PropertyPageBase
{
public:
    FieldGeneral(wxWindow *parent);
    ~FieldGeneral();
    bool IsLogOnly();
protected:
    void do_layout();
private:
    wxStaticText *m_label;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
};

