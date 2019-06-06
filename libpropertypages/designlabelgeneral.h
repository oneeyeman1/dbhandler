#pragma once
struct Properties
{
    wxString m_name, m_tag, m_text;
    bool m_supressPrint;
    int m_border, m_alignment;
    wxFont m_font;
    wxPoint m_position;
    wxSize m_size;
};

class WXEXPORT DesignLabelGeneral : public PropertyPageBase
{
public:
    DesignLabelGeneral(wxWindow *parent, const Properties *prop);
    ~DesignLabelGeneral();
protected:
    void do_layout();
    void DataChange(wxCommandEvent &event);
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4, *m_label5;
    wxTextCtrl *m_name, *m_tag, *m_text;
    wxCheckBox *m_suppressPrint;
    wxComboBox *m_border, *m_alignment;
};

