#ifndef _MYERDTABLE_H
#define _MYERDTABLE_H

class MyErdTable : public wxSFRoundRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(MyErdTable);
    MyErdTable();
    MyErdTable(const DatabaseTable &table);
    virtual ~MyErdTable();
    void AddColumn(GUIColumn *col);
    void UpdateTable();
protected:
    void ClearGrid();
    void ClearConnections();
    void DrawDetail(wxDC &dc);
    void AddColumn(const wxString &colName, int id, Constraint::constraintType type);
    void SetCommonProps(wxSFShapeBase* shape);
//    virtual void DrawHighlighted(wxDC &dc);
    virtual void DrawHover(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
private:
    wxSFTextShape *m_pLabel;
    wxSFFlexGridShape* m_pGrid;
    std::vector<GUIColumn *> m_columns;
    DatabaseTable *m_table;
};

#endif