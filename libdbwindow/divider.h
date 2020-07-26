#pragma once

struct DividerPropertiesType
{
    wxString m_color;
    wxString m_cursorFile;
    wxString m_type;
    int m_height, m_stockCursor;
};

class Divider : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(Divider);
    Divider();
    Divider(const wxString &text, const wxString &cursorFile, int stocCursor, wxSFDiagramManager *manager);
    virtual ~Divider();
    virtual wxRect GetBoundingBox() wxOVERRIDE;
    const wxString &GetDividerType();
    DividerPropertiesType GetDividerProperties();
    virtual void OnDragging(const wxPoint& pos) wxOVERRIDE;
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC &dc) wxOVERRIDE;
    virtual void DrawHover(wxDC &dc) wxOVERRIDE;
private:
    wxSFGridShape *m_grid;
    wxSFTextShape *m_text, *m_arrow;
    DividerPropertiesType m_props;
};