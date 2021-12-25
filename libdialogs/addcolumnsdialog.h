#pragma once

class AddColumnsDialog : public wxDialog
{
public:
    AddColumnsDialog(wxWindow *parent, int type, const wxPoint &pos, const std::vector<std::wstring> &fields, const wxString &dbType, const wxString &dbSubtype, const std::vector<QueryArguments> &args);
    ~AddColumnsDialog(void);
    wxListBox *GetFieldsControl() const;
    void OnPasteUpdateUI(wxUpdateUIEvent &event);
    void OnFieldsDoubleClick(wxCommandEvent &event);
    virtual int ShowModal() wxOVERRIDE;
protected:
    void set_properties();
    void do_layout();
private:
    wxListBox *m_fields;
    wxButton *m_paste, *m_cancel;
    wxPanel *m_panel;
    int m_type;
    std::vector<std::wstring> m_allFields;
    std::vector<QueryArguments> m_args;
    wxString m_dbType, m_dbSubtype;
    wxPoint m_pos;
};

