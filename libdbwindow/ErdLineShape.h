#ifndef _ERDLINESHAPE_H
#define _ERDLINESHAPE_H

class ErdLineShape : public wxSFRoundOrthoLineShape
{
public:
    ErdLineShape();
    virtual ~ErdLineShape();
    ErdLineShape(Constraint *pConstraint, ViewType type, const wxSFDiagramManager &pManager);
    virtual wxRect GetBoundingBox() wxOVERRIDE;
    wxRealPoint GetModTrgPoint();
    wxRealPoint GetModSrcPoint();
    Constraint *GetConstraint();
    bool GetLineSegment(size_t index, wxRealPoint& src, wxRealPoint& trg);
    void SetSourceTable(const MyErdTable *table) { m_sourceTbl = const_cast<MyErdTable *>( table ); }
    void SetTargetTable(const MyErdTable *table) { m_targetTbl = const_cast<MyErdTable *>( table ); }
    void EnableDisableFK(bool enable);
    const wxString &GetSourceName() const { return m_source; }
    const wxString &GetTargetName() const { return m_target; }
protected:
    virtual void DrawNormal(wxDC& dc) wxOVERRIDE;
    virtual void DrawSelected(wxDC &dc) wxOVERRIDE;
    virtual void DrawCompleteLine(wxDC& dc) wxOVERRIDE;
    virtual void OnLeftDoubleClick(const wxPoint& pos) wxOVERRIDE;
    wxRealPoint GetSourcePoint();
    wxRealPoint GetTargetPoint();
    void GetDirectionalLine(wxRealPoint& src, wxRealPoint& trg);
private:
    Constraint *m_constraint;
    ViewType m_type;
    ConstraintSign *m_signConstraint;
    MyErdTable *m_sourceTbl, *m_targetTbl;
    FieldShape *m_sourceFld, *m_targetFld;
    bool m_isEnabled;
    wxString m_source, m_target;
};

#endif
