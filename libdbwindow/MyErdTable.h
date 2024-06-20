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
    const DatabaseTable *GetTable();
    wxSFTextShape *GetLabel();
    GridTableShape *GetFieldGrid();
    void DisplayTypes(bool display) { m_displayTypes = display; m_pGrid->ShowDataTypes( display ); if( display ) m_columns++; else m_columns--;  }
    void DisplayComments(bool display) { m_displayComments = display; m_pGrid->ShowComments( display ); if( display ) { m_columns++; } else { m_columns--; } }
    void SetDataaseTable(const DatabaseTable *table) { m_table = const_cast<DatabaseTable *>( table ); }
    const wxString &GetCatalogName() const { return m_catalogName; }
    const wxString &GetSchemaName() const { return m_schemaName; }
    const wxString &GetTableName() const { return m_tableName; }
protected:
    void ClearGrid();
    void ClearConnections();
    void DrawDetail(wxDC &dc);
    void AddColumn(TableField *field, int id, Constraint::constraintType type);
    void SetCommonProps(wxSFShapeBase* shape);
//    virtual void DrawHighlighted(wxDC &dc);
    virtual void DrawHover(wxDC &dc);
    virtual void DrawNormal(wxDC &dc);
    virtual void DrawSelected(wxDC& dc);
private:
    void MarkSerializableDataMembers();
    ViewType m_type;
    HeaderGrid *m_header;
    NameTableShape *m_pLabel;
    CommentTableShape *m_comment;
    GridTableShape* m_pGrid;
    DatabaseTable *m_table;
    bool m_displayTypes, m_displayComments;
    int m_columns;
    wxString m_catalogName, m_schemaName, m_tableName;
};

#endif
