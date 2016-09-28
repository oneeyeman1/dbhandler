#ifndef _ERDLINESHAPE_H
#define _ERDLINESHAPE_H

class ErdLineShape : public wxSFRoundOrthoLineShape
{
public:
    ErdLineShape();
    ErdLineShape(Constraint *pConstraint);
    virtual wxRect GetBoundingBox();
    wxRealPoint GetModTrgPoint();
    wxRealPoint GetModSrcPoint();
    Constraint *GetConstraint();
    bool GetLineSegment(size_t index, wxRealPoint& src, wxRealPoint& trg);
protected:
    virtual void DrawNormal(wxDC& dc);
    virtual void DrawCompleteLine(wxDC& dc);
    wxRealPoint GetSourcePoint();
    wxRealPoint GetTargetPoint();
    void GetDirectionalLine(wxRealPoint& src, wxRealPoint& trg);
private:
    Constraint *m_constraint;
    bool m_isEnabled;
};

#endif