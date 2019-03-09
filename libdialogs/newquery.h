#pragma once
class NewQuery : public wxDialog
{
public:
    NewQuery(wxWindow *parent, const wxString &title);
    ~NewQuery();
protected:
    void do_layout();
private:
    wxPanel *m_panel;
    BitmapPanel *m_panels[14];
    wxButton *m_ok, *m_cancel, *m_help, *m_options;
    wxCheckBox *m_preview;
};

