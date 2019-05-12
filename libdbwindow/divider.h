#pragma once
class Divider : public wxSFRectShape
{
public:
    Divider();
    Divider(const wxString &text);
    virtual ~Divider();
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC &dc) wxOVERRIDE;
private:
    wxSFGridShape *m_grid;
    wxSFTextShape *m_text;
    wxSFBitmapShape *m_arrow;
};