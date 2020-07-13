#include <map>
#include <vector>
#include <cctype>
#if _MSC_VER >= 1900 || !defined WIN32
#include <mutex>
#endif

#ifndef DBMANAGER_DATABASE
#define DBMANAGER_DATABASE

#define UNUSED(str)

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

struct TableProperties
{
    std::wstring m_comment, m_dataFontName, m_headingFontName, m_labelFontName;
    int m_dataFontSize, m_dataFontEncoding, m_headingFontSize, m_headingFontEncoding, m_labelFontSize, m_labelFontEncoding;
    int m_dataFontPixelSize, m_headingFontPixelSize, m_labelFontPixelSize;
    bool m_isDataFontUnderlined, m_isDataFontStriken, m_isHeadingFontUnderlined, m_isHeadingFontStriken, m_isLabelFontUnderlined, m_isLabelFontStrioken;
    bool m_isDataFontBold, m_isDataFontItalic, m_isHeadingFontBold, m_isHeadingFontItalic, m_isLabelFontBold, m_isLabelFontItalic;
};

class Field
{
public:
    Field()
    {
        m_comment = L"";
        column_name = column_type = full_name = column_defaultValue = L"";
        field_size = -1;
        decimal_size = -1;
        column_isNull = autoIncrement = column_pk = column_fk = false;
    }

    Field(const std::wstring &columnName, const std::wstring &columnType, int size, int decimalsize, const std::wstring &fullName, const std::wstring &columnDefaultValue = L"", const bool columnIsNull = false, bool autoincrement = false, const bool columnPK = false, const bool columnFK = false)
    {
        m_comment = L"";
        column_name = columnName;
        column_type = columnType;
        field_size = size;
        decimal_size = decimalsize;
        column_defaultValue = columnDefaultValue;
        column_isNull = columnIsNull;
        autoIncrement = autoincrement;
        column_pk = columnPK;
        column_fk = columnFK;
        label = columnName;
        heading = columnName;
        full_name = fullName;
    }
    const std::wstring &GetLabel() const { return label; }
    const std::wstring &GetHeading() const { return heading; }
    void SetLabel(const std::wstring &lbl) { label = lbl; }
    void SetHeading(const std::wstring &hding) { heading = hding; }
    const std::wstring &GetFieldName() const { return column_name; }
    const std::wstring &GetFieldType() const { return column_type; }
    const std::wstring &GetDefaultValue() const { return column_defaultValue; }
    const std::wstring &GetComment() const { return m_comment; }
    bool IsNullAllowed() const { return column_isNull; }
    int GetFieldSize() const { return field_size; }
    int GetPrecision() const { return decimal_size; }
    bool IsPrimaryKey() const { return column_pk; }
    bool IsForeignKey() const { return column_fk; }
    bool IsAutoIncrement() { return autoIncrement; }
    void SetComment(const std::wstring &comment) { m_comment = comment; }
    void SetFullType(const std::wstring type) { full_type = type; }
    const std::wstring &GetFullType() const { return full_type; }
private:
    std::wstring column_name, column_type, column_defaultValue, m_comment, label, heading, full_type, full_name;
    bool autoIncrement, column_isNull, column_pk, column_fk;
    int field_size, decimal_size;
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
    DatabaseTable(const std::wstring &tableName, const std::wstring &schemaName, const std::vector<Field *> &tableFields, const std::map<int,std::vector<FKField *> > &foreignKeys)
    {
        m_objectId = 0;
        m_owner = L"";
        m_comment = L"";
        table_name = tableName;
        schema_name = schemaName;
        table_fields = tableFields;
        foreign_keys = foreignKeys;
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

    ~DatabaseTable()
    {
        for( std::vector<Field *>::iterator it = table_fields.begin(); it < table_fields.end(); ++it )
        {
            delete (*it);
            (*it) = NULL;
        }
        table_fields.clear();
        for( std::map<int, std::vector< FKField *> >::iterator it1 = foreign_keys.begin(); it1 != foreign_keys.end(); ++it1 )
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

    const std::wstring &GetTableName() const { return table_name; }
    const std::wstring &GetSchemaName() const { return schema_name; }
    const std::wstring &GetComment() const { return m_comment; }
    void SetComment(const std::wstring &comment) { m_comment = comment; }
    void SetDataFontName(const std::wstring &name) { m_dataFontName = name; }
    const std::wstring &GetDataFontName() const { return m_dataFontName; }
    void SetHeadingFontName(const std::wstring &name) { m_headingFontName = name; }
    const std::wstring &GetHeadingFontName() const { return m_headingFontName; }
    void SetLabelFontName(const std::wstring &name) { m_labelFontName = name; }
    const std::wstring &GetLabelFontName() const { return m_labelFontName; }
    void SetDataFontWeight(int weight) { m_dataFontWeight = weight; }
    int GetDataFontWeight() const { return m_dataFontWeight; }
    void SetHeadingFontWeight(int weight) { m_headingFontWeight = weight; }
    int GetHeadingFontWeight() const { return m_headingFontWeight; }
    void SetLabelFontWeight(int weight) { m_labelFontWeight = weight; }
    int GetLabelFontWeight() const { return m_labelFontWeight; }
    void SetDataFontItalic(bool italic) { m_dataFontItalic = italic; }
    bool GetDataFontItalic() const { return m_dataFontItalic; }
    void SetHeadingFontItalic(bool italic) { m_headingFontItalic = italic; }
    bool GetHeadingFontItalic() const { return m_headingFontItalic; }
    void SetLabelFontItalic(bool italic) { m_labelFontItalic = italic; }
    bool GetLabelFontItalic() const { return m_labelFontItalic; }
    void SetDataFontSize(int size) { m_dataFontSize = size; }
    int GetDataFontSize() const { return m_dataFontSize; }
    void SetHeadingFontSize(int size) { m_headingFontSize = size; }
    int GetHeadingFontSize() const { return m_headingFontSize; }
    void SetLabelFontSize(int size) { m_labelFontSize = size; }
    int GetLabelFontSize() const { return m_labelFontSize; }
    void SetDataFontUnderline(bool param) { m_dataFontUnderline = param; }
    bool GetDataFontUnderline() const { return m_dataFontUnderline; }
    void SetHeadingFontUnderline(bool underline) { m_headingFontUnderline = underline; }
    bool GetHeadingFontUnderline() const { return m_headingFontUnderline; }
    void SetLabelFontUnderline(bool param) { m_labelFontUnderline = param; }
    bool GetLabelFontUnderline() const { return m_labelFontUnderline; }
    void SetDataFontStrikethrough(bool param) { m_dataFontStrikethrough = param; }
    bool GetDataFontStrikethrough() const { return m_dataFontStrikethrough; }
    void SetHeadingFontStrikethrough(bool strikethrough) { m_headingFontStrikethrough = strikethrough; }
    bool GetHeadingFontStrikethrough() const { return m_headingFontStrikethrough; }
    void SetLabelFontStrikethrough(bool param) { m_labelFontStrikethrough = param; }
    bool GetLabelFontStrikethrough() const { return m_labelFontStrikethrough; }
    int GetDataFontCharacterSet() { return m_dataFontCharacterSet; }
    int GetHeadingFontCharacterSet() const { return m_headingFontCharacterSet; }
    int GetLabelFontCharacterSet() const { return m_labelFontCharacterSer; }
    void SetDataFontCharacterSet(int charSet) { m_dataFontCharacterSet = charSet; }
    void SetHeadingFontCharacterSet(int charSet) { m_headingFontCharacterSet = charSet; }
    void SetLabelFontCharacterSet(int charSet) { m_labelFontCharacterSer = charSet; }
    int GetDataFontPixelSize() const { return m_dataFontPixelSize; }
    int GetHeadingFontPixelSize() const { return m_headingFontPixelSize; }
    int GetLabelFontPixelSize() const { return m_labelFontPixelSize; }
    void SetDataFontPixelSize(int size) { m_dataFontPixelSize = size; }
    void SetHeadingFontPixelSize(int size) { m_headingFontPixelSize = size; }
    void SetLabelFontPixelSize(int size) { m_labelFontPixelSize = size; }
    const std::vector<Field *> &GetFields() const { return table_fields; }
    std::map<int,std::vector<FKField *> > &GetForeignKeyVector() { return foreign_keys; }
    const unsigned long GetTableId() const { return m_objectId; }
    void SetTableId(unsigned long id) { m_objectId = id; }
    const std::wstring &GetTableOwner() const { return m_owner; }
    void SetTableOwner(const std::wstring &owner) { m_owner = owner; }
    void SetIndexNames(const std::vector<std::wstring> &indexes) { m_indexes = indexes; }
    const std::vector<std::wstring> &GetIndexNames() const { return m_indexes; }
    void SetNumberOfFields(int count) { m_numFields = count; }
    int GetNumberOfFields() const { return m_numFields; }
    void SetNumberOfIndexes(int count) { m_numIndex = count; }
    int GetNumberOfIndexes() const { return m_numIndex; }
private:
    std::wstring table_name, schema_name, m_comment, m_owner;
    std::vector<Field *> table_fields;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring m_dataFontName, m_labelFontName, m_headingFontName;
    int m_dataFontWeight, m_labelFontWeight, m_headingFontWeight, m_numFields, m_numIndex;
    unsigned long m_objectId;
    bool m_dataFontItalic, m_labelFontItalic, m_headingFontItalic;
    int m_dataFontSize, m_labelFontSize, m_headingFontSize, m_dataFontCharacterSet, m_labelFontCharacterSer, m_headingFontCharacterSet;
    int m_dataFontPixelSize, m_headingFontPixelSize, m_labelFontPixelSize;
    bool m_dataFontUnderline, m_labelFontUnderline, m_headingFontUnderline;
    bool m_dataFontStrikethrough, m_labelFontStrikethrough, m_headingFontStrikethrough;
    std::vector<std::wstring> m_indexes;
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
    unsigned int m_numOfTables;
    void ltrim(std::wstring &str) { str.erase( str.begin(), std::find_if( str.begin(), str.end(), [](int ch) { return !std::isspace( ch ); } ) ); };
    void rtrim(std::wstring &str) { str.erase( std::find_if( str.rbegin(), str.rend(), [](int ch) { return !std::isspace( ch ); } ).base(), str.end() ); };
    void trim(std::wstring str) { ltrim( str ); rtrim( str ); };
    virtual bool IsTablePropertiesExist(const DatabaseTable *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual bool IsFieldPropertiesExist(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, std::vector<std::wstring> &errorMsg) = 0;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetServerVersion(std::vector<std::wstring> &errorMsg) = 0;
    virtual int ServerConnect(std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) = 0;
    virtual int AddDropTable(const std::wstring &catalog, const std::wstring &schemaName, const std::wstring &tableName, const std::wstring &ownerName, int tableId, bool tableAdded, std::vector<std::wstring> &errorMsg) = 0;
public:
    virtual ~Database() = 0;
    Impl &GetTableVector() { return *pimpl; };
    virtual int Connect(const std::wstring &selectedDSN, std::vector<std::wstring> &dbList, std::vector<std::wstring> &errorMsg) = 0;
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) = 0;
    virtual int CreateIndex(const std::wstring &command, const std::wstring &index_name, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &ownerName, const std::wstring &fieldName, Field *field, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldProperties(const std::wstring &table, Field *field, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetFieldProperties(const std::wstring &tableName, const std::wstring &ownerName, const std::wstring &fieldName, const Field *field, bool isLogOnly, std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int ApplyForeignKey(std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, const std::vector<std::wstring> &foreignKeyFields, const std::wstring &refTableName, const std::vector<std::wstring> &refKeyFields, int deleteProp, int updateProp, bool logOnly, std::vector<FKField *> &newFK, bool isNew, int match, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int NewTableCreation(std::vector<std::wstring> &errorMsg) = 0;
    virtual int DropForeignKey(std::wstring &command, const DatabaseTable &tableName, const std::wstring &keyName, bool logOnly, std::vector<std::wstring> &errorMsg) = 0;
};

struct Database::Impl
{
#if _MSC_VER >= 1900 || !defined WIN32
    static std::mutex my_mutex;
#endif
    std::map<std::wstring, std::vector<DatabaseTable *> > m_tables;
    std::vector<std::wstring> m_tableNames;
    std::wstring m_dbName, m_type, m_subtype, m_connectString, m_connectedUser;
    std::wstring m_serverVersion;
    std::wstring m_pgLogFile, m_pgLogDir;
    int m_versionMajor, m_versionMinor, m_versionRevision;
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
