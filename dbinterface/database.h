#include <map>
#include <vector>
#include <cctype>
#if _MSC_VER >= 1900 || !defined WIN32
#include <mutex>
#endif

#ifndef DBMANAGER_DATABASE
#define DBMANAGER_DATABASE

#define UNUSED(str)
#define DEFAULTLABELALIGNMENT 0
#define DEFAULTHEADINGALIGNMENT 2

#define NO_MORE_ROWS 100

#define INTEGER_TYPE 1
#define DOUBLE_TYPE  2
#define STRING_TYPE  3
#define BLOB_TYPE    4
#define WSTRING_TYPE 5

enum FK_ONUPDATE
{
    NO_ACTION_UPDATE,
    RESTRICT_UPDATE,
    SET_NULL_UPDATE,
    SET_DEFAULT_UPDATE,
    CASCADE_UPDATE
};

enum FK_ONDELETE
{
    NO_ACTION_DELETE,
    RESTRICT_DELETE,
    SET_NULL_DELETE,
    SET_DEFAULT_DELETE,
    CASCADE_DELETE
};

struct TableDefinition
{
    std::wstring schemaName, tableName, catalogName;
    TableDefinition(const std::wstring &cat, const std::wstring &schema, const std::wstring &table) : schemaName(schema), tableName(table), catalogName(cat) {}
    bool operator==(const TableDefinition &def) const
    {
        if( def.schemaName == schemaName && def.tableName == tableName && def.catalogName == catalogName )
            return true;
        else
            return false;
    };
};

struct DataEditFiield
{
    int type, m_precision;
    unsigned long m_size;
    union ValuueType
    {
        void *blobValue;
        int intValue;
        double doubleValue;
        std::string strValue;
        std::wstring stringValue;
        ValuueType() : stringValue()  {}
        ValuueType(int value) : intValue( value ) {}
        ValuueType(double value) : doubleValue( value ) {}
        ValuueType(const std::wstring &value) : stringValue( value ) {}
        ValuueType(const std::string &value) : strValue( value ) {}
        ValuueType(const wchar_t *value) : stringValue( value ) {}
        ValuueType(const void *value) : blobValue( const_cast<void *>( value ) ) {}
        ValuueType(const ValuueType &) = delete;
        ValuueType& operator=(const ValuueType&) = delete;
#if defined _MSC_VER
        __int64 longvalue;
        ValuueType(__int64 value) : longvalue( value ) {}
#else
		long long int longvalue;
		ValuueType(long long int value) : longvalue( value ) {}
#endif
        ~ValuueType() noexcept {}
    } value;

    DataEditFiield(int myvalue) : type( INTEGER_TYPE ), m_size( 0 ), m_precision( 0 ), value( myvalue ) { }

    DataEditFiield(double myvalue, unsigned long size, int precision) : type( DOUBLE_TYPE ), m_size( size ), m_precision( precision ), value( myvalue ) { }

    DataEditFiield(const std::wstring &myvalue) : type( WSTRING_TYPE ), m_size( 0 ), m_precision( 0 ), value( myvalue ) {}

    DataEditFiield(const void *myvalue, int size) : type( BLOB_TYPE ), m_size( size ), m_precision( 0 ), value( myvalue ) {}

    DataEditFiield(const std::string &myvalue) : type( STRING_TYPE ), m_size( 0 ), m_precision( 0 ), value( myvalue ) {}

#if defined _MSC_VER
    DataEditFiield(__int64 myvalue) : type( INTEGER_TYPE ), m_size( 0 ), m_precision( 0 ), value( myvalue ) {}
#else
	DataEditFiield(long long int myvalue) : type( 1 ), value( myvalue ) {}
#endif

    DataEditFiield(const DataEditFiield &field)
    {
        switch( field.type )
        {
        case INTEGER_TYPE:
            value.intValue = field.value.intValue;
            break;
        case DOUBLE_TYPE:
            value.doubleValue = field.value.doubleValue;
            break;
        case WSTRING_TYPE:
            value.stringValue = field.value.stringValue;
            break;
        case BLOB_TYPE:
            value.blobValue = field.value.blobValue;
            break;
        case STRING_TYPE:
            value.strValue = field.value.strValue;
            break;
        }
        type = field.type;
        m_size = field.m_size;
        m_precision = field.m_precision;
    }

    DataEditFiield &operator=(const DataEditFiield &) = delete;

    ~DataEditFiield()
    {
        using std::wstring;
        if( type == WSTRING_TYPE )
            value.stringValue.~wstring();
    }
};

class TableProperties
{
public:
    TableProperties()
    {
        m_comment = L"";
        m_dataFontName = L"MS Sans Serif";
        m_headingFontName = L"MS Sans Serif";
        m_labelFontName = L"MS Sans Serif";
        m_dataFontWeight = 0;
        m_headingFontWeight = 1;
        m_labelFontWeight = 1;
        m_dataFontItalic = false;
        m_headingFontItalic = false;
        m_labelFontItalic = false;
        m_dataFontSize = 8;
        m_headingFontSize = 8;
        m_labelFontSize = 8;
        m_dataFontUnderline = false;
        m_headingFontUnderline = false;
        m_labelFontUnderline = false;
        m_dataFontStrikethrough = false;
        m_headingFontStrikethrough = false;
        m_labelFontStrikethrough = false;
        m_dataFontCharacterSet = -1;
        m_headingFontCharacterSet = -1;
        m_labelFontCharacterSer = -1;
    }
    std::wstring m_comment, m_dataFontName, m_headingFontName, m_labelFontName, table_name, m_owner, schema_name, catalog;
    int m_dataFontSize, m_dataFontEncoding, m_headingFontSize, m_headingFontEncoding, m_labelFontSize, m_labelFontEncoding;
    int m_dataFontPixelSize, m_headingFontPixelSize, m_labelFontPixelSize;
    int m_dataFontWeight, m_headingFontWeight, m_labelFontWeight, m_dataFontCharacterSet, m_labelFontCharacterSer, m_headingFontCharacterSet;
    bool m_dataFontUnderline, m_dataFontStrikethrough, m_headingFontUnderline, m_headingFontStrikethrough, m_labelFontUnderline, m_labelFontStrikethrough;
    bool m_dataFontItalic, m_headingFontItalic, m_labelFontItalic;
    std::vector<std::wstring> m_pkFields;
};

class FieldProperties
{
public:
    FieldProperties()
    {
        m_comment = m_label = m_heading = L"";
        m_labelPosition = m_headingPosition = 0;
    }
    std::wstring m_comment, m_label, m_heading;
    int m_labelPosition, m_headingPosition;
};

class TableField
{
public:
    TableField()
    {
        column_name = column_type = full_name = column_defaultValue = L"";
        field_size = -1;
        decimal_size = -1;
        column_isNull = autoIncrement = column_pk = column_fk = false;
        m_props.m_label = m_props.m_heading = L"";
        m_props.m_labelPosition = m_props.m_headingPosition = 0;
    }

    TableField(const std::wstring &columnName, const std::wstring &columnType, int size, int decimalsize, const std::wstring &fullName, const std::wstring &columnDefaultValue = L"", const bool columnIsNull = false, bool autoincrement = false, const bool columnPK = false, const bool columnFK = false)
    {
        column_name = columnName;
        column_type = columnType;
        field_size = size;
        decimal_size = decimalsize;
        column_defaultValue = columnDefaultValue;
        column_isNull = columnIsNull;
        autoIncrement = autoincrement;
        column_pk = columnPK;
        column_fk = columnFK;
        full_name = fullName;
        std::wstring temp = columnName;
        std::replace( temp.begin(), temp.end(), L'_', L'.' );
        m_props.m_label = temp;
        temp = columnName;
        std::replace( temp.begin(), temp.end(), L'_', L'\n' );
        m_props.m_heading = temp;
        m_props.m_labelPosition = DEFAULTLABELALIGNMENT;
        m_props.m_headingPosition = DEFAULTHEADINGALIGNMENT;
    }
    const std::wstring &GetFieldName() const { return column_name; }
    const std::wstring &GetFieldType() const { return column_type; }
    const std::wstring &GetDefaultValue() const { return column_defaultValue; }
    bool IsNullAllowed() const { return column_isNull; }
    int GetFieldSize() const { return field_size; }
    int GetPrecision() const { return decimal_size; }
    bool IsPrimaryKey() const { return column_pk; }
    bool IsForeignKey() const { return column_fk; }
    bool IsAutoIncrement() { return autoIncrement; }
    void SetFullType(const std::wstring type) { full_type = type; }
    const std::wstring &GetFullType() const { return full_type; }
    const std::wstring &GetFullName() const { return full_name; }
    FieldProperties &GetFieldProperties() { return m_props; }
    void SetFieldProperties(const FieldProperties &props) { m_props = props; }
private:
    std::wstring column_name, column_type, column_defaultValue, full_type, full_name;
    bool autoIncrement, column_isNull, column_pk, column_fk;
    int field_size, decimal_size;
    FieldProperties m_props;
};

class FKField
{
public:
    FKField(int id, const std::wstring &name, const std::wstring &orig_schema, const std::wstring &table_name, const std::wstring &original_field, const std::wstring &ref_schema, const std::wstring &ref_table, const std::wstring &referenced_field, const std::vector<std::wstring> &origFields, const std::vector<std::wstring> &refFields, FK_ONUPDATE update_constraint, FK_ONDELETE delete_constraint, int match = -1)
    {
        this->fkId = id;
        this->fkName = name;
        this->origSchema = orig_schema;
        this->tableName = table_name;
        this->originalField = original_field;
        this->refSchema = ref_schema;
        this->refTable = ref_table;
        this->referencedField = referenced_field;
        this->updateConstraint = update_constraint;
        this->deleteConstraint = delete_constraint;
        this->origFields = origFields;
        this->refFields = refFields;
        this->match = match;
    }
    const int GetForeignKeyId() const { return fkId; }
    const std::wstring &GetFKName() const { return fkName; }
    const std::wstring &GetReferencedTableName() const { return refTable; }
    const std::wstring &GetReferentialSchemaName() const { return refSchema; }
    const std::wstring &GetOriginalFieldName() const { return originalField; }
    const std::vector<std::wstring> &GetOriginalFields() const { return origFields; }
    const std::wstring &GetReferencedFieldName() const { return referencedField; }
    const std::vector<std::wstring> &GetReferencedFields() const { return refFields; }
    const FK_ONUPDATE GetOnUpdateConstraint() const { return updateConstraint; }
    const FK_ONDELETE GetOnDeleteConstraint() const { return deleteConstraint; }
    const int GetMatchOPtion() const { return match; }
private:
    int fkId;
    std::wstring tableName, originalField, referencedField, refTable, origSchema, refSchema, fkName;
    std::vector<std::wstring> origFields, refFields;
    FK_ONUPDATE updateConstraint;
    FK_ONDELETE deleteConstraint;
    int match;
};

class DatabaseTable
{
public:
    DatabaseTable(const std::wstring &tableName, const std::wstring &schemaName, const std::vector<TableField *> &tableFields, const std::map<unsigned long,std::vector<FKField *> > &foreignKeys)
    {
        m_objectId = 0;
        m_props.m_owner = L"";
        m_props.table_name = tableName;
        m_props.schema_name = schemaName;
        table_fields = tableFields;
        foreign_keys = foreignKeys;
    }

    ~DatabaseTable()
    {
        for( std::vector<TableField *>::iterator it = table_fields.begin(); it < table_fields.end(); ++it )
        {
            delete (*it);
            (*it) = NULL;
        }
        table_fields.clear();
        for( std::map<unsigned long, std::vector< FKField *> >::iterator it1 = foreign_keys.begin(); it1 != foreign_keys.end(); ++it1 )
        {
            for( std::vector<FKField *>::iterator it2 = (*it1).second.begin(); it2 < (*it1).second.end(); ++it2 )
            {
                delete (*it2);
                (*it2) = NULL;
            }
            (*it1).second.clear();
        }
        foreign_keys.clear();
    }

    const std::wstring &GetTableName() const { return m_props.table_name; }
    const std::wstring &GetSchemaName() const { return m_props.schema_name; }
    const std::wstring &GetCatalog() const { return m_props.catalog; }
    TableProperties &GetTableProperties() { return m_props; }
    void SetTableProperties(const TableProperties &props) { m_props = props; }
    const std::vector<TableField *> &GetFields() const { return table_fields; }
    std::map<unsigned long,std::vector<FKField *> > &GetForeignKeyVector() { return foreign_keys; }
    const unsigned long GetTableId() const { return m_objectId; }
    void SetTableId(unsigned long id) { m_objectId = id; }
    const std::wstring &GetTableOwner() const { return m_props.m_owner; }
    void SetTableOwner(const std::wstring &owner) { m_props.m_owner = owner; }
    void SetIndexNames(const std::vector<std::wstring> &indexes) { m_indexes = indexes; }
    const std::vector<std::wstring> &GetIndexNames() const { return m_indexes; }
    void SetNumberOfFields(size_t count) { m_numFields = count; }
    size_t GetNumberOfFields() const { return m_numFields; }
    void SetNumberOfIndexes(int count) { m_numIndex = count; }
    int GetNumberOfIndexes() const { return m_numIndex; }
private:
    std::vector<TableField *> table_fields;
    std::map<unsigned long,std::vector<FKField *> > foreign_keys;
    size_t m_numFields;
    int m_numIndex;
    unsigned long m_objectId;
    std::vector<std::wstring> m_indexes;
    TableProperties m_props;
};

#ifdef WIN32
class __declspec(dllexport) Database
#else
class Database
#endif
{
protected:
    struct Impl;
    Impl *pimpl;
    bool connectToDatabase;
    unsigned int m_numOfTables, m_fieldsInRecordSet;
    void ltrim(std::wstring &str) { str.erase( str.begin(), std::find_if( str.begin(), str.end(), [](int ch) { return !std::isspace( ch ); } ) ); };
    void rtrim(std::wstring &str) { str.erase( std::find_if( str.rbegin(), str.rend(), [](int ch) { return !std::isspace( ch ); } ).base(), str.end() ); };
    void trim(std::wstring str) { ltrim( str ); rtrim( str ); };
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) = 0;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) = 0;
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) = 0;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &ownerName, long tableId, bool tableAdded, std::vector<std::wstring> &errorMsg) = 0;
public:
    virtual ~Database() = 0;
    Impl &GetTableVector() { return *pimpl; };
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) = 0;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) = 0;
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &catalogName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, TableField *field, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldProperties(const std::wstring &table, TableField *field, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const FieldProperties &properties, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) = 0;
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldHeader(const std::wstring &tabeName, const std::wstring &fieldName, std::wstring &headerStr) = 0;
    virtual int PrepareStatement(const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int EditTableData(std::vector<DataEditFiield> &row, std::vector<std::wstring> &errorMsg) = 0;
    virtual int FinalizeStatement(std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableCreationSyntax(const std::wstring tableName, std::wstring &syntax, std::vector<std::wstring> &errorMsg) = 0;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errors) = 0;
    virtual int AttachDatabase(const std::wstring &catalog, const std::wstring &schema, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetDatabaseNameList(std::vector<std::wstring> &names, std::vector<std::wstring> &errorMsg) = 0;
};

struct Database::Impl
{
    static std::mutex my_mutex;
    std::map<std::wstring, std::vector<DatabaseTable *> > m_tables;
    std::map<std::wstring, std::vector<TableDefinition> > m_tableDefinitions;
    std::vector<std::wstring> m_tableNames;
    std::wstring m_dbName, m_type, m_subtype, m_connectString, m_connectedUser;
    std::wstring m_serverVersion;
    std::wstring m_pgLogFile, m_pgLogDir;
    unsigned long m_versionMajor, m_versionMinor, m_versionRevision;
    unsigned long m_clientVersionMajor, m_clientVersionMinor, m_clientVersionRevision;
    const std::wstring &GetConnectedUser() const { return m_connectedUser; };
    void SetConnectedUser(const std::wstring &user) { m_connectedUser = user; };
    const std::wstring &GetDatabaseType() const { return m_type; };
    const std::wstring &GetDatabaseSubtype() const { return m_subtype; };
    void PushTableName(const std::wstring tableName) { m_tableNames.push_back( tableName ); };
    const std::vector<std::wstring> &GetTableNames() const { return m_tableNames; };
    const std::wstring &GetPostgreLogFile() const { return m_pgLogFile; };
    const std::wstring &GetPostgresLogDir() const { return m_pgLogDir; };
};

inline Database::~Database()
{
    delete pimpl;
}

#endif
