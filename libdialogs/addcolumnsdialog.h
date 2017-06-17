#pragma once
class AddColumnsDialog : public wxDialog
{
public:
    AddColumnsDialog(wxWindow *parent, int type);
    ~AddColumnsDialog(void);
protected:
    void set_properties();
    void do_layout();
private:
    wxListBox *m_fields;
    wxButton *m_paste, *m_cancel;
    wxPanel *m_panel;
};

