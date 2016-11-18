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
    virtual int CreateIndex(std::wstring &command, bool isUnique, bool isAscending, const std::wstring &indexName, const std::wstring &tableName, const std::vector<std::wstring> &fields, bool logOnly, std::vector<std::wstring> &errorMsg);
protected:
    void GetErrorMessage(int code, std::wstring &errorMsg);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
private:
    sqlite3 *m_db;
    std::wstring m_catalog;
};

#endif