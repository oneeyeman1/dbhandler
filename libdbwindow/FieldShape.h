#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldShape);
    FieldShape(void);
    FieldShape(const FieldShape &shape);
    virtual ~FieldShape(void);
    virtual wxRect GetBoundingBox();
    virtual bool Contains(const wxPoint& pos);
    void SetParentRect(const wxRect &rect);
    void SetField(TableField *field);
    void Select(bool state);
    TableField *GetField();
protected:    
    virtual void DrawNormal(wxDC &dc);
    virtual void DrawSelected(wxDC& dc);
private:
    TableField *m_field;
    wxRect m_parentRect;
    wxColour m_backColour;
};

