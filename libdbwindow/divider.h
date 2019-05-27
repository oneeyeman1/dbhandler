#pragma once
class Divider : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(Divider);
    Divider();
    Divider(const wxString &text, wxSFDiagramManager *manager);
    virtual ~Divider();
    virtual wxRect GetBoundingBox() wxOVERRIDE;
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC &dc) wxOVERRIDE;
    virtual void DrawHover(wxDC &dc) wxOVERRIDE;
private:
    wxSFGridShape *m_grid;
    wxSFTextShape *m_text, *m_arrow;
    wxString m_color;
    int m_height;
};