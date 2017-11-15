#ifndef __CONSTRAINTSIGN__
#define __CONSTRAINTSIGN__

class ConstraintSign : public wxSFRectShape
{
public:
    ConstraintSign(ViewType type);
    virtual ~ConstraintSign();
    const wxString &GetSign();
    void SetSign(const wxString &sign);
protected:
    virtual void DrawSelected(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
private:
    wxSFFlexGridShape *m_grid;
    wxSFTextShape *m_sign;
    wxSFBitmapShape *m_fKey;
    ViewType m_type;
};
#endif