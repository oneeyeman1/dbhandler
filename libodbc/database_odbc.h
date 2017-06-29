#ifndef DBMANAGER_ODBC
#define DBMANAGER_ODBC

struct SQLTablesDataBinding
{
    SQLSMALLINT TargetType;
    SQLPOINTER TargetValuePtr;
    SQLINTEGER BufferLength;
    SQLLEN StrLen_or_Ind;
};

#ifdef UNIXODBC
#define TRUE 1
#define FALSE 0
#endif

#ifdef WIN32
class __declspec(dllexport) ODBCDatabase : public Database
#else
class ODBCDatabase : public Database
#endif
{
public:
    ODBCDatabase();
    virtual ~ODBCDatabase();
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg);
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &errorMsg);
    void SetWindowHandle(SQLHWND handle);
    void AskForConnectionParameter(bool ask);
    bool GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg);
    bool AddDsn(SQLHWND hwnd, const std::wstring &driver, std::vector<std::wstring> &errorMsg);
    bool EditDsn(SQLHWND hwnd, const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool RemoveDsn(const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool GetDSNList(std::vector<std::wstring> &dsn, std::vector<std::wstring> &errorMsg);
    virtual int CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schema_name, const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg);
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg);
    virtual int SetTableProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg);
    virtual int ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg);
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg);
protected:
    int GetDriverForDSN(SQLWCHAR *dsn, SQLWCHAR *driver, std::vector<std::wstring> &errorMsg);
    int GetSQLStringSize(SQLWCHAR *str);
    void str_to_uc_cpy(std::wstring &dest, const SQLWCHAR *src);
    void uc_to_str_cpy(SQLWCHAR *dest, const std::wstring &src);
    void copy_uc_to_uc(SQLWCHAR *dest, SQLWCHAR *src);
    bool equal(SQLWCHAR *dest, SQLWCHAR *src);
    int GetErrorMessage(std::vector<std::wstring> &errorMsg, int type, SQLHSTMT stmt = 0);
    int GetDSNErrorMessage(std::vector<std::wstring> &errorMsg);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg);
    virtual int SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &user, const std::wstring &comment, std::vector<std::wstring> &errorMsg);
    virtual bool IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, std::vector<std::wstring> &errorMsg);
private:
    SQLHENV m_env;
    SQLHDBC m_hdbc;
    SQLHSTMT m_hstmt;
    SQLHWND m_handle;
    bool m_ask;
    SQLUSMALLINT m_statementsNumber;
    bool m_oneStatement;
    SQLWCHAR *m_connectString;
};

#endif
