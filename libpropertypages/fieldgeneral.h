#pragma once
class WXEXPORT FieldGeneral : public wxPanel
{
public:
    FieldGeneral(wxWindow *parent);
    ~FieldGeneral();
protected:
    void do_layout();
private:
    wxStaticText *m_label1;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
};

