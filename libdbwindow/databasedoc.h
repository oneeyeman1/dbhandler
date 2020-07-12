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

    void AddTables(const std::vector<wxString> &selections);
    std::vector<MyErdTable *> &GetTables();
    std::vector<std::wstring> &GetTableNameVector();
    std::vector<DatabaseTable *> &GetDBTables();
    MyErdTable *GetReferencedTable(const wxString &tableName);
    std::vector<std::wstring> &GetTableNames();
    void AppendFieldToQueryFields(const std::wstring &field);
    void AddRemoveField(const std::wstring &fieldName, bool isAdded);
    const std::vector<std::wstring> &GetQueryFields();
    void SetQueryFields(const std::vector<Field *> &fields);
private:
    void DoUpdate();

    Database *m_db;
    std::vector<MyErdTable *> m_tables;
    std::vector<std::wstring> m_tableNames;
    std::vector<std::wstring> m_queryFields;
    std::vector<DatabaseTable *> m_dbTables;
    wxDECLARE_DYNAMIC_CLASS(DrawingDocument);
};
