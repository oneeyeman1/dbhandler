#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldShape);
    FieldShape(void);
    virtual ~FieldShape(void);
    void SetParentRect(const wxRect &rect);
protected:
    wxString m_name, m_type;
    long m_size, m_decimal;
    bool m_isNotNull, m_isPK, m_isAutoInc;
    
    virtual void DrawNormal(wxDC &dc);
private:
    wxRect m_parentRect;
    wxColour m_backColour;
};

