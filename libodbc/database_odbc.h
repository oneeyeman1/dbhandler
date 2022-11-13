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
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) override;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) override;
    void SetWindowHandle(SQLHWND handle);
    void AskForConnectionParameter(bool ask);
    bool GetDriverList(std::map<std::wstring, std::vector<std::wstring> > &driversDSN, std::vector<std::wstring> &errMsg);
    bool AddDsn(SQLHWND hwnd, const std::wstring &driver, std::vector<std::wstring> &errorMsg);
    bool EditDsn(SQLHWND hwnd, const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool RemoveDsn(const std::wstring &driver, const std::wstring &dsn, std::vector<std::wstring> &errorMsg);
    bool GetDSNList(std::vector<std::wstring> &dsn, std::vector<std::wstring> &errorMsg);
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
	virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *table, std::vector<std::wstring> &errorMsg) override;
    virtual int GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg) override;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &field, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) override;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int UNUSED(match), std::vector<std::wstring> &errorMsg) override;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    int CreateSystemObjectsAndGetDatabaseInfo(std::vector<std::wstring> &errorMsg);
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) override;
    void GetConnectedUser(const std::wstring &dsn, std::wstring &connectedUser);
    void GetConnectionPassword(const std::wstring &dsn, std::wstring &connectionPassword);
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr) override;
    virtual int PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    virtual int EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg) override;
    virtual int FinalizeStatement(std::vector<std::wstring> &errorMsg) override;
    virtual int GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errors) override;
    virtual int AttachDatabase(const std::wstring &catalog, const std::wstring &, std::vector<std::wstring> &errorMsg) override;
    virtual int GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg) override;
protected:
    struct ODBCImpl;
    ODBCImpl *odbc_pimpl;
    int GetDriverForDSN(SQLWCHAR *dsn, SQLWCHAR *driver, std::vector<std::wstring> &errorMsg);
    int GetSQLStringSize(SQLWCHAR *str);
    void str_to_uc_cpy(std::wstring &dest, const SQLWCHAR *src);
    void str_to_c_cpy(std::string &dest, const SQLCHAR *src);
    void uc_to_str_cpy(SQLWCHAR *dest, const std::wstring &src);
    void copy_uc_to_uc(SQLWCHAR *dest, SQLWCHAR *src);
    bool equal(SQLWCHAR *dest, SQLWCHAR *src);
    int GetErrorMessage(std::vector<std::wstring> &errorMsg, int type, SQLHSTMT stmt = 0);
    int GetDSNErrorMessage(std::vector<std::wstring> &errorMsg);
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) override;
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) override;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schema_name, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) override;
    int GetTableId(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, long &tableId, std::vector<std::wstring> &errorMsg);
    int GetTableOwner(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, std::wstring &owner, std::vector<std::wstring> &errorMsg);
    void SetFullType(TableField *field);
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) override;
    int CreateIndexesOnPostgreConnection(std::vector<std::wstring> &errorMsg);
    int GetFieldProperties(const SQLWCHAR *tableName, const SQLWCHAR *schemaName, const SQLWCHAR *ownerName, const SQLWCHAR *fieldName, TableField *field, std::vector<std::wstring> &errorMsg);
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) override;
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg) override;
    virtual int AddDropTable(const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &tableOwner, long tableId, bool tableAdded, std::vector<std::wstring> &errorMsg) override;
private:
    SQLHENV m_env;
    SQLHDBC m_hdbc;
    SQLHSTMT m_hstmt;
    SQLHWND m_handle;
    bool m_ask;
    SQLUSMALLINT m_statementsNumber;
    bool m_oneStatement, m_isConnected;
    SQLWCHAR *m_connectString;
};

struct ODBCDatabase::ODBCImpl
{
    std::wstring m_currentTableOwner, m_driverName;
};

#endif
