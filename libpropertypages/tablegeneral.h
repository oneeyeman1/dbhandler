#ifndef _TABLEGENERALPROPERTY
#define _TABLEGENERALPROPERTY

class WXEXPORT TableGeneralProperty : public wxPanel
{
public:
    TableGeneralProperty(wxWindow *parent);
    ~TableGeneralProperty();
private:
    void set_properties();
    void do_layout();
    wxStaticText *m_label1;
    wxStaticText *m_label2;
    wxStaticText *m_label3;
    wxTextCtrl *m_owner;
    wxTextCtrl *m_table;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
};

#endif