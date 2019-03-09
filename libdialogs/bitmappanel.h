#pragma once
class BitmapPanel : public wxPanel
{
public:
    BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label);
    void OnSetFocus(wxFocusEvent &event);
    void OnKillFocus(wxFocusEvent &event);
protected:
    void do_layout();
private:
    wxStaticBitmap *m_bitmap;
    wxStaticText *m_label;
};