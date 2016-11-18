#pragma once
class FieldShape :	public wxSFTextShape
{
public:
    FieldShape(void);
    virtual ~FieldShape(void);
protected:
    virtual void DrawNormal(wxDC &dc);
};

