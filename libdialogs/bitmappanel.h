#pragma once
class BitmapPanel : public wxPanel
{
public:
    BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label);
private:
    wxBitmap *m_bitmap;
    wxStaticText *m_label;
};