#ifndef _ERDLINESHAPE_H
#define _ERDLINESHAPE_H

class ErdLineShape : public wxSFRoundOrthoLineShape
{
public:
    ErdLineShape();
    virtual ~ErdLineShape();
    ErdLineShape(Constraint *pConstraint, ViewType type);
    virtual wxRect GetBoundingBox();
    wxRealPoint GetModTrgPoint();
    wxRealPoint GetModSrcPoint();
    Constraint *GetConstraint();
    bool GetLineSegment(size_t index, wxRealPoint& src, wxRealPoint& trg);
    void EnableDisableFK(bool enable);
protected:
    virtual void DrawNormal(wxDC& dc);
    virtual void DrawCompleteLine(wxDC& dc);
    wxRealPoint GetSourcePoint();
    wxRealPoint GetTargetPoint();
    void GetDirectionalLine(wxRealPoint& src, wxRealPoint& trg);
private:
    Constraint *m_constraint;
    ViewType m_type;
    ConstraintSign *m_signConstraint;
    bool m_isEnabled;
};

#endif