#pragma once
class BitmapPanel : public wxPanel
{
public:
    BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label);
#if GTK_CHECK_VERSION(3,6,0)
    wxStaticText *GetLabel();
#else
    wxGenericStaticText *GetLabel();
#endif
    void OnBitmapClicked(wxMouseEvent &event);
    void OnLabelClicked(wxMouseEvent &event);
protected:
    void do_layout();
private:
    wxStaticBitmap *m_bitmap;
#if GTK_CHECK_VERSION( 3, 6, 0 )
    wxStaticText *m_label;
#else
    wxGenericStaticText *m_label;
#endif
};