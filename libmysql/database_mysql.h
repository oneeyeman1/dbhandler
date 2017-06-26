#ifndef DBMANAGER_MYSQL
#define DBMANAGER_MYSQL

#define WXUNUSED(var)

#ifdef WIN32
class __declspec(dllexport) MySQLDatabase : public Database
#else
class MySQLDatabase : public Database
#endif
{
public:
    MySQLDatabase();
    virtual ~MySQLDatabase();
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    int mySQLConnect();
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &WXUNUSED(errorMsg));
    virtual int CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(const std::wstring &command, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
protected:
    struct MySQLImpl;
    MySQLImpl *m_pimpl;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual int SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg);
    int TokenizeConnectionString(std::wstring &connectStr, std::vector<std::wstring> &errorMsg);
private:
    MYSQL *m_db;
    int m_port, m_flags;
};

struct MySQLDatabase::MySQLImpl
{
    std::wstring_convert<std::codecvt_utf8<wchar_t> > m_myconv;
    std::wstring m_host, m_user, m_password, m_dbName, m_socket;
};
#endif