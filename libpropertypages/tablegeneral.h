#ifndef _TABLEGENERALPROPERTY
#define _TABLEGENERALPROPERTY

class WXEXPORT TableGeneralProperty : public PropertyPageBase
{
public:
    TableGeneralProperty(wxWindow *parent, void *table, int type);
    ~TableGeneralProperty();
    wxTextCtrl *GetCommentCtrl();
    const std::wstring &GetComment();
    bool IsLogOnly();
protected:
    void OnCommentKeyEntered(wxKeyEvent &event);
    void OnEditComment(wxCommandEvent &event);
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
    void *m_table;
    int m_type;
};

#endif