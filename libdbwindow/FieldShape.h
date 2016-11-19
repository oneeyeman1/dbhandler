#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    FieldShape(void);
    virtual ~FieldShape(void);
    void SetParentRect(const wxRect &rect);
protected:
    virtual void DrawNormal(wxDC &dc);
private:
    wxRect m_parentRect;
    wxColour m_backColour;
};

