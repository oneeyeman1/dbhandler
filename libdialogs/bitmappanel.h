#pragma once
class BitmapPanel : public wxPanel
{
public:
    BitmapPanel(wxWindow *parent, const wxBitmap &bitmap, const wxString &label);
#ifdef __WXGTK__
#if GTK_CHECK_VERSION(3,6,0)
    wxStaticText *GetLabel();
#else
    wxGenericStaticText *GetLabel();
#endif
#else
    wxStaticText *GetLabel();
#endif
    void OnBitmapClicked(wxMouseEvent &event);
    void OnLabelClicked(wxMouseEvent &event);
protected:
    void do_layout();
private:
    wxStaticBitmap *m_bitmap;
#ifdef __WXGTK__
#if GTK_CHECK_VERSION( 3, 6, 0 )
    wxStaticText *m_label;
#else
    wxGenericStaticText *m_label;
#endif
#else
    wxStaticText *m_label;
#endif
};