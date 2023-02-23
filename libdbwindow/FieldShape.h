#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldShape);
    FieldShape(void);
    FieldShape(const FieldShape &shape);
    virtual ~FieldShape(void);
    virtual wxRect GetBoundingBox() wxOVERRIDE;
    virtual bool Contains(const wxPoint& pos) wxOVERRIDE;
    void SetParentRect(const wxRect &rect);
    void SetField(TableField *field);
    void Select(bool state);
    TableField *GetField();
protected:    
    virtual void DrawNormal(wxDC &dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC& dc) wxOVERRIDE;
private:
    bool m_isNotNull, m_isPK, m_isAutoInc;
    TableField *m_field;
    wxRect m_parentRect;
    int m_size, m_decimal;
    wxString m_name, m_type;
    wxColour m_backColour;
};

