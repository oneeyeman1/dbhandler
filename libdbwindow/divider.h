#pragma once

class Divider : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(Divider);
    Divider();
    Divider(const wxString &text, const wxString &cursorFile, int stocCursor, wxSFDiagramManager *manager);
    virtual ~Divider();
    virtual wxRect GetBoundingBox() wxOVERRIDE;
    const wxString &GetDividerType();
    BandProperties *GetDividerProperties() const;
    virtual void OnDragging(const wxPoint& pos) wxOVERRIDE;
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC &dc) wxOVERRIDE;
    virtual void DrawHover(wxDC &dc) wxOVERRIDE;
private:
    wxSFGridShape *m_grid;
    wxSFTextShape *m_text, *m_arrow;
    BandProperties *m_props;
};
