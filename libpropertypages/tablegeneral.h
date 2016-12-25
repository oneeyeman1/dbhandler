#ifndef _TABLEGENERALPROPERTY
#define _TABLEGENERALPROPERTY

class WXEXPORT TableGeneralProperty : public wxPanel
{
public:
    TableGeneralProperty(wxWindow *parent, DatabaseTable *table);
    ~TableGeneralProperty();
protected:
    void OnCommentKeyEntered(wxKeyEvent &event);
private:
    void set_properties();
    void do_layout();
    wxStaticText *m_label1;
    wxStaticText *m_label2;
    wxStaticText *m_label3;
    wxTextCtrl *m_owner;
    wxTextCtrl *m_tableName;
    wxTextCtrl *m_comment;
    wxCheckBox *m_log;
    DatabaseTable *m_table;
};

#endif