#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldShape);
    FieldShape(void);
    FieldShape(const FieldShape &shape);
    virtual ~FieldShape(void);
    virtual wxRect GetBoundingBox();
    void SetParentRect(const wxRect &rect);
    void SetField(Field *field);
    Field *GetField();
protected:    
    virtual void DrawNormal(wxDC &dc);
private:
    Field *m_field;
    wxRect m_parentRect;
    wxColour m_backColour;
};

