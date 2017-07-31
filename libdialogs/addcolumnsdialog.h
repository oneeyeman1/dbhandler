#pragma once
class AddColumnsDialog : public wxDialog
{
public:
    AddColumnsDialog(wxWindow *parent, int type, const std::vector<std::wstring> &fields);
    ~AddColumnsDialog(void);
    wxListBox *GetFieldsControl() const;
    void OnPasteUpdateUI(wxUpdateUIEvent &event);
protected:
    void set_properties();
    void do_layout();
private:
    wxListBox *m_fields;
    wxButton *m_paste, *m_cancel;
    wxPanel *m_panel;
    int m_type;
    std::vector<std::wstring> m_allFields;
};

