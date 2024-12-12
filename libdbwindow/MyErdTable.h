#ifndef _MYERDTABLE_H
#define _MYERDTABLE_H

#define DISPLAYTYPES 1
#define DISPLAYCOMMENTS 2

class MyErdTable : public wxSFRoundRectShape, public PropertiesHandler
{
public:
    XS_DECLARE_CLONABLE_CLASS(MyErdTable);
    MyErdTable();
    MyErdTable(DatabaseTable *table, ViewType type);
    virtual ~MyErdTable();
    void UpdateTable();
    void SetTableComment(const wxString &comment);
    WXEXPORT DatabaseTable *GetTable();
    wxSFTextShape *GetLabel();
    GridTableShape *GetFieldGrid();
    void DisplayTypes(bool display, int type)
    {
        if( type == DISPLAYTYPES )
            m_displayTypes = display;
        else
            m_displayComments = display;
        if( type == DISPLAYCOMMENTS )
            m_header->ShowComments( display );
        if( display ) m_columns++; else m_columns--;
    }
    void SetDataaseTable(const DatabaseTable *table) { m_table = const_cast<DatabaseTable *>( table ); }
    void SetDatabase(const Database *db) { m_db = db; }
    const wxString &GetCatalogName() const { return m_catalogName; }
    const wxString &GetSchemaName() const { return m_schemaName; }
    const wxString &GetTableName() const { return m_tableName; }
    void SetProperties(TableProperties properties);
    virtual int ApplyProperties(const wxAny &any, bool logOnly, std::wstring &command) wxOVERRIDE;
    virtual wxAny &GetProperties() wxOVERRIDE;
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
    const Database *m_db;
    wxString m_catalogName, m_schemaName, m_tableName;
};

#endif
