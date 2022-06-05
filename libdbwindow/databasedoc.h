// This sample is written to build both with wxUSE_STD_IOSTREAM==0 and 1, which
// somewhat complicates its code but is necessary in order to support building
// it under all platforms and in all build configurations
//
// In your own code you would normally use std::stream classes only and so
// wouldn't need these typedefs
#if wxUSE_STD_IOSTREAM
    typedef wxSTD istream DocumentIstream;
    typedef wxSTD ostream DocumentOstream;
#else // !wxUSE_STD_IOSTREAM
    typedef wxInputStream DocumentIstream;
    typedef wxOutputStream DocumentOstream;
#endif // wxUSE_STD_IOSTREAM/!wxUSE_STD_IOSTREAM

struct GroupFields
{
    wxString fieldName;
    long position, internal_position;

    GroupFields(const wxString &name, long pos, long internal_pos) : fieldName(name), position(pos), internal_position(internal_pos) {}
    GroupFields() : fieldName( "" ), position( -1 ), internal_position( -1 ) {}
    GroupFields &operator=(const GroupFields group)
    {
        if( fieldName == group.fieldName )
            return *this;
        else
        {
            fieldName = group.fieldName;
            position = group.position;
            internal_position = group.internal_position;
            return *this;
        }
    }
};

struct FieldSorter
{
    wxString m_name;
    bool m_isAscending;
    long m_originalPosition;
    FieldSorter(wxString name, bool isAscending, long original_position) : m_name(name), m_isAscending(isAscending), m_originalPosition(original_position) {};
    FieldSorter &operator=(const FieldSorter &sorter)
    {
        if( m_name == sorter.m_name )
            return *this;
        else
        {
            m_name = sorter.m_name;
            m_isAscending = sorter.m_isAscending;
            m_originalPosition = sorter.m_originalPosition;
            return *this;
        }
    }
};

// The drawing document (model) class itself
class DrawingDocument : public wxDocument
{
public:
    DrawingDocument() : wxDocument() { }
    ~DrawingDocument();
    DocumentOstream& SaveObject(DocumentOstream& stream) wxOVERRIDE;
    DocumentIstream& LoadObject(DocumentIstream& stream) wxOVERRIDE;
    void SetDatabase(Database *db, bool isInit);
    Database *GetDatabase();
    size_t GetGroupByFieldCount() { return m_groupByFields.size(); };
    void AddTables(const std::vector<wxString> &selections);
    std::vector<MyErdTable *> &GetTables();
    std::vector<std::wstring> &GetTableNameVector();
    std::vector<DatabaseTable *> &GetDBTables();
    MyErdTable *GetReferencedTable(const wxString &tableName);
    std::vector<std::wstring> &GetTableNames();
    void AppendFieldToQueryFields(const std::wstring &field);
    void AddRemoveField(const std::wstring &fieldName, bool isAdded);
    const std::vector<std::wstring> &GetQueryFields();
    void SetQueryFields(const std::vector<TableField *> &fields);
    void AddGroupByAvailableField(const wxString &name, long position);
    void AddGroupByFieldToQuery(const wxString &name, long position, long original, wxString &replace);
    void DeleteGroupByField(const wxString &name, long original, wxString &replace);
    void DeleteGroupByTable(const wxString &tableName, wxString &replace);
    void ClearGroupByVector() { m_groupByFields.clear(); }
    std::vector<GroupFields> &GetGroupFields() { return m_groupByFields; }
    void AddAllSortedFeld(const wxString &name, long original_position);
    void AddSortedField(const wxString &name, long position, long original_position, wxString &replace);
    void DeleteSortedField(const wxString &name, long original, wxString &replace);
    void DeleteSortedTable(const wxString &tableName, wxString &replace);
    void ClearSortedVector() { m_sortedFields.clear(); }
    void ChangeSortOrder(const wxString &fieldName, long newPosition);
    void ShuffleGroupByFields(const wxString &fieldName, long newPosition, wxString &replace);
private:
    void DoUpdate();

    Database *m_db;
    std::vector<MyErdTable *> m_tables;
    std::vector<std::wstring> m_tableNames;
    std::vector<std::wstring> m_queryFields;
    std::vector<DatabaseTable *> m_dbTables;
    std::vector<GroupFields> m_groupByFields, m_groupByFieldsAll;
    std::vector<FieldSorter> m_sortedFields, m_sortedFieldsAll;
    wxDECLARE_DYNAMIC_CLASS(DrawingDocument);
};
