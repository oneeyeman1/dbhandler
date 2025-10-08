#ifndef _TABLEGENERALPROPERTY
#define _TABLEGENERALPROPERTY

class WXEXPORT TableGeneralProperty : public PropertyPageBase
{
public:
    TableGeneralProperty(wxWindow *parent, const wxString &name, const wxString &owner, const wxString &comment, bool type);
    ~TableGeneralProperty();
    wxTextCtrl *GetCommentCtrl();
    const std::wstring &GetComment();
    virtual bool IsLogOnly() const override;
protected:
    void OnCommentKeyEntered(wxKeyEvent &event);
    void OnEditComment(wxCommandEvent &event);
    void OnLogOnly(wxCommandEvent &event);
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
    wxString m_commentText, m_ownerText, m_nameText;
    bool m_isTable;
};

#endif
