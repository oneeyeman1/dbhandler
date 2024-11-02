#pragma once
class WXEXPORT FieldHeader :	public PropertyPageBase
{
public:
    FieldHeader(wxWindow *parent, const FieldTableHeadingProperties &prop);
    ~FieldHeader(void);
    wxTextCtrl *GetLabelCtrl();
    wxTextCtrl *GetHeadingCtrl();
    wxComboBox *GetLabeAlignmentCtrl();
    wxComboBox *GetHeaderAlignmentCtrl();
protected:
    void do_layout();
    void set_properties();
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4;
    wxTextCtrl *m_label, *m_heading;
    wxComboBox *m_labelPos, *m_headingPos;
    wxString m_labelText, m_headingText;
    int m_labelAlignment, m_headingAlignment;
    wxLayoutDirection m_layout;
};

