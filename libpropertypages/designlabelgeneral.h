#pragma once
class DesignLabelGeneral : public wxPanel
{
public:
    DesignLabelGeneral(wxWindow *parent);
    ~DesignLabelGeneral();
protected:
    void do_layout();
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5;
    wxTextCtrl *m_name, *m_tag, *m_text;
    wxCheckBox *m_suppressPrint;
    wxComboBox *m_border, *m_alignment;
};

