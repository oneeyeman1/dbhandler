#ifndef DBMANAGER_ODBC
#define DBMANAGER_ODBC

struct SQLTablesDataBinding
{
    SQLSMALLINT TargetType;
    SQLPOINTER TargetValuePtr;
    SQLINTEGER BufferLength;
    SQLLEN StrLen_or_Ind;
};

#ifdef WIN32
class __declspec(dllexport) ODBCDatabase : public Database
#else
class ODBCDatabase : public Database
#endif
{
public:
    ODBCDatabase();
    virtual ~ODBCDatabase();
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg);
    virtual int Disconnect(std::vector<std::wstring> &errorMsg);
    void SetWindowHandle(SQLHWND handle);
    void AskForConnectionParameter(bool ask);
    bool GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg);
    bool AddDsn(SQLHWND hwnd, const std::wstring &driver, std::vector<std::wstring> &errorMsg);
    bool EditDsn(SQLHWND hwnd, const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool RemoveDsn(const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool GetDSNList(std::vector<std::wstring> &dsn, std::vector<std::wstring> &errorMsg);
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