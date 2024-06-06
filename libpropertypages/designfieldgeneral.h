#pragma once

class WXEXPORT DesignFieldGeneral : public wxPanel
{
public:
    DesignFieldGeneral(wxWindow *parent);
    ~DesignFieldGeneral() { };
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4;
    wxTextCtrl *m_name, *m_tag;
    wxCheckBox *m_suppressPrint, *m_equality, *m_override, *m_picture, *m_rtl;
    wxComboBox *m_border, *m_alignment;
};