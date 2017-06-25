#ifndef _MYERDTABLE_H
#define _MYERDTABLE_H

class MyErdTable : public wxSFRoundRectShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(MyErdTable);
    MyErdTable();
    MyErdTable(DatabaseTable *table, ViewType type);
    virtual ~MyErdTable();
    void UpdateTable();
    void SetTableComment(const wxString &comment);
    const DatabaseTable &GetTable();
    wxSFTextShape *GetLabel();
    GridTableShape *GetFieldGrid();
    std::wstring &GetTableName();
protected:
    void ClearGrid();
    void ClearConnections();
    void DrawDetail(wxDC &dc);
    void AddColumn(Field *field, int id, Constraint::constraintType type);
    void SetCommonProps(wxSFShapeBase* shape);
//    virtual void DrawHighlighted(wxDC &dc);
    virtual void DrawHover(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
    virtual void DrawSelected(wxDC& dc);
private:
    ViewType m_type;
    HeaderGrid *m_header;
    wxSFTextShape *m_pLabel;
    CommentTableShape *m_comment;
    GridTableShape* m_pGrid;
    DatabaseTable *m_table;
};

#endif