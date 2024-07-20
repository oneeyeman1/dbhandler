#ifndef __CONSTRAINTSIGN__
#define __CONSTRAINTSIGN__

class ConstraintSign : public wxSFRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(MyErdTable);
    ConstraintSign();
    ConstraintSign(ViewType type, int joinType);
    virtual ~ConstraintSign();
    const wxString &GetSign();
    void SetSign(const wxString &sign);
    void SetConstraint(const Constraint *constraint);
    Constraint *GetConstraint() const;
    void DeleteConstraint();
    int GetType() const { return m_joinType; };
    void SetType(int type) { m_joinType = type; };
protected:
    virtual void DrawSelected(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
    void initSerializable();
private:
    wxSFFlexGridShape *m_grid;
    wxSFTextShape *m_sign;
    wxSFBitmapShape *m_fKey;
    ViewType m_type;
    Constraint *m_constraint;
    int m_joinType;
};
#endif