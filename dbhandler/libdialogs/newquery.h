#pragma once
class NewQuery : public wxDialog
{
public:
    NewQuery(wxWindow *parent, const wxString &title, int source, int presentation);
    ~NewQuery();
    const int GetSource();
    const int GetPresentation();
    void OnPanelSourceClicked(wxMouseEvent &event);
    void OnPanelPresentationClicked(wxMouseEvent &event);
    void OnOptionsUpdateUI(wxUpdateUIEvent &event);
protected:
    void do_layout();
    void set_properties();
private:
    wxPanel *m_panel;
    BitmapPanel *m_panels[15];
    wxButton *m_ok, *m_cancel, *m_help, *m_options;
    wxCheckBox *m_preview;
    wxString m_title;
    int m_source, m_presentation;
};

