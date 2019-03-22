#pragma once
class BitmapPanel : public wxPanel
{
public:
    BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label);
    wxStaticText *GetLabel();
    void OnBitmapClicked(wxMouseEvent &event);
    void OnLabelClicked(wxMouseEvent &event);
protected:
    void do_layout();
private:
    wxStaticBitmap *m_bitmap;
    wxStaticText *m_label;
};