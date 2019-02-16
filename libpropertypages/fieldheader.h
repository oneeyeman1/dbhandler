#pragma once
class WXEXPORT FieldHeader :	public PropertyPageBase
{
public:
    FieldHeader(wxWindow *parent);
    ~FieldHeader(void);
protected:
    void do_layout();
private:
    wxStaticText *m_label1, *m_label2, *m_label3;
    wxTextCtrl *m_label, *m_heading;
    wxComboBox *m_labelPos, *m_headingPos;
};

