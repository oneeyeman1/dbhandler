#ifndef __CONSTRAINTSIGN__
#define __CONSTRAINTSIGN__

class ConstraintSign : public wxSFRectShape
{
public:
    ConstraintSign();
    virtual ~ConstraintSign();
    const wxString &GetSign();
    void SetSign(const wxString &sign);
private:
    wxSFTextShape *m_sign;
};
#endif