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
class TableDocument : public wxDocument
{
public:
    TableDocument() : wxDocument() { }
    ~TableDocument();
    DocumentOstream& SaveObject(DocumentOstream& stream) wxOVERRIDE;
    DocumentIstream& LoadObject(DocumentIstream& stream) wxOVERRIDE;

    void SetDatabase(Database *db, DatabaseTable *table);
    Database *GetDatabase();
    void AddTables(const std::vector<wxString> &selections);
    std::vector<std::wstring> &GetTableNames();
private:
    void DoUpdate();

    Database *m_db;
    std::vector<std::wstring> m_tableNames;
    wxDECLARE_DYNAMIC_CLASS(DrawingDocument);
};
