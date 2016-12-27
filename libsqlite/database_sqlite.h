#ifndef DBMANAGER_SQLITE
#define DBMANAGER_SQLITE

#ifdef WIN32
class __declspec(dllexport) SQLiteDatabase : public Database
#else
class SQLiteDatabase : public Database
#endif
{
public:
    SQLiteDatabase();
    virtual ~SQLiteDatabase();
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &errorMsg);
    virtual int CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg);
	virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual void GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual void SetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
protected:
    struct SQLiteImpl;
    SQLiteImpl *sqlite_pimpl;
    void GetErrorMessage(int code, std::wstring &errorMsg);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual void GetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, std::wstring &comment, std::vector<std::wstring> &errorMsg);
    virtual void SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg);
private:
    sqlite3 *m_db;
};

struct SQLiteDatabase::SQLiteImpl
{
    std::wstring m_catalog;
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
};

#endif