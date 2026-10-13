#pragma once
class WXEXPORT FieldHeader :	public PropertyPageBase
{
public:
    FieldHeader(wxWindow *parent, const FieldTableHeadingProperties &prop);
    ~FieldHeader(void);
    wxTextCtrl *GetLabelCtrl() const;
    wxTextCtrl *GetHeadingCtrl() const;
    wxComboBox *GetLabeAlignmentCtrl() const;
    wxComboBox *GetHeaderAlignmentCtrl() const;
protected:
    void do_layout();
    void set_properties();
    void OnChanged(wxCommandEvent &event);
private:
    wxStaticText *m_label1, *m_label2, *m_label3, *m_label4;
    wxTextCtrl *m_label, *m_heading;
    wxComboBox *m_labelPos, *m_headingPos;
    wxString m_labelText, m_headingText;
    int m_labelAlignment, m_headingAlignment;
    wxLayoutDirection m_layout;
};

