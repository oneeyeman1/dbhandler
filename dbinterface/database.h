#include <map>
#include <vector>

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
    bool m_isDataFontUnderlined, m_isDataFontStriken, m_isHeadingFontUnderlined, m_isHeadingFontStriken, m_isLabelFontUnderlined, m_isLabelFontStrioken;
    bool m_isDataFontBold, m_isDataFontItalic, m_isHeadingFontBold, m_isHeadingFontItalic, m_isLabelFontBold, m_isLabelFontItalic;
};

class Field
{
public:
    Field()
    {
        comment = L"";
        column_name = column_type = column_defaultValue = L"";
        field_size = -1;
        decimal_size = -1;
        column_isNull = autoIncrement = column_pk = column_fk = false;
    }

    Field(const std::wstring &columnName, const std::wstring &columnType, int size, int decimalsize, const std::wstring &columnDefaultValue = L"", const bool columnIsNull = false, bool autoincrement = false, const bool columnPK = false, const bool columnFK = false)
    {
        comment = L"";
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
    }
	const std::wstring &GetLabel() { return label; }
	const std::wstring &GetHeading() { return heading; }
	void SetLabel(const std::wstring &lbl) { label = lbl; }
	void SetHeading(const std::wstring &hding) { heading = hding; }
    const std::wstring &GetFieldName() { return column_name; }
    const std::wstring &GetFieldType() { return column_type; }
    const std::wstring &GetDefaultValue() { return column_defaultValue; }
    const std::wstring &GetComment() { return comment; }
    bool IsNullAllowed() { return column_isNull; }
    int GetFieldSize() { return field_size; }
    int GetPrecision() { return decimal_size; }
    bool IsPrimaryKey() { return column_pk; }
    bool IsForeignKey() { return column_fk; }
    bool IsAutoIncrement() { return autoIncrement; }
    void SetComment(const std::wstring &comment) { this->comment = comment; }
	void SetFullType(const std::wstring &type) { full_type = type; }
private:
    std::wstring column_name, column_type, column_defaultValue, comment, label, heading, full_type;
    bool autoIncrement, column_isNull, column_pk, column_fk;
    int field_size, decimal_size;
};

class FKField
{
public:
    FKField(int id, const std::wstring &table_name, const std::wstring &original_field, const std::wstring &referenced_field, const std::wstring &schema, FK_ONUPDATE update_constraint, FK_ONDELETE delete_constraint)
    {
        fkId = id;
        schemaName = schema;
        tableName = table_name;
        originalField = original_field;
        referencedField = referenced_field;
        updateConstraint = update_constraint;
        deleteConstraint = delete_constraint;
    }
    const int GetForeignKeyId() { return fkId; } 
    const std::wstring &GetReferencedTableName() { return tableName; }
    const std::wstring &GetOriginalFieldName() { return originalField; }
    const std::wstring &GetReferencedFieldName() { return referencedField; }
    const FK_ONUPDATE GetOnUpdateConstraint() { return updateConstraint; }
    const FK_ONDELETE GetOnDeleteConstraint() { return deleteConstraint; }
private:
    int fkId;
    std::wstring schemaName, tableName, originalField, referencedField;
    FK_ONUPDATE updateConstraint;
    FK_ONDELETE deleteConstraint;
};

class DatabaseTable
{
public:
    DatabaseTable(const std::wstring &tableName, const std::wstring &schemaName, const std::vector<Field *> &tableFields, const std::map<int,std::vector<FKField *> > &foreignKeys)
    {
        comment = L"";
        table_name = tableName;
        schema_name = schemaName;
        table_fields = tableFields;
        foreign_keys = foreignKeys;
        m_dataFontName = L"";
        m_headingFontName = L"";
        m_labelFontName = L"";
        m_dataFontWeight = 0;
        m_headingFontWeight = 0;
        m_labelFontWeight = 0;
        m_dataFontItalic = false;
        m_headingFontItalic = false;
        m_labelFontItalic = false;
        m_dataFontSize = 0;
        m_headingFontSize = 0;
        m_labelFontSize = 0;
        m_dataFontUnderline = false;
        m_headingFontUnderline = false;
        m_labelFontUnderline = false;
        m_dataFontStrikethrough = false;
        m_labelFontStrikethrough = false;
    }
    const std::wstring &GetTableName() { return table_name; }
    const std::wstring &GetSchemaName() { return schema_name; }
    const std::wstring &GetComment() { return comment; }
    void SetComment(const std::wstring &comment) { this->comment = comment; }
    void SetDataFontName(const std::wstring &name) { m_dataFontName = name; }
    const std::wstring &GetDataFontName() { return m_dataFontName; }
    void SetHeadingFontName(const std::wstring &name) { m_headingFontName = name; }
    const std::wstring &GetHeadingFontName() { return m_headingFontName; }
    void SetLabelFontName(const std::wstring &name) { m_labelFontName = name; }
    const std::wstring &GetLabelFontName() { return m_labelFontName; }
    void SetDataFontWeight(int weight) { m_dataFontWeight = weight; }
    int GetDataFontWeight() { return m_dataFontWeight; }
    void SetHeadingFontWeight(int weight) { m_headingFontWeight = weight; }
    int GetHeadingFontWeight() { return m_headingFontWeight; }
    void SetLabelFontWeight(int weight) { m_labelFontWeight = weight; }
    int GetLabelFontWeight() { return m_labelFontWeight; }
    void SetDataFontItalic(bool italic) { m_dataFontItalic = italic; }
    bool GetDataFontItalic() { return m_dataFontItalic; }
    void SetHeadingFontItalic(bool italic) { m_headingFontItalic = italic; }
    bool GetHeadingFontItalic() { return m_headingFontItalic; }
    void SetLabelFontItalic(bool italic) { m_labelFontItalic = italic; }
    bool GetLabelFontItalic() { return m_labelFontItalic; }
    void SetDataFontSize(int size) { m_dataFontSize = size; }
    int GetDataFontSize() { return m_dataFontSize; }
    void SetHeadingFontSize(int size) { m_headingFontSize = size; }
    int GetHeadingFontSize() { return m_headingFontSize; }
    void SetLabelFontSize(int size) { m_labelFontSize = size; }
    int GetLabelFontSize() { return m_labelFontSize; }
    void SetDataFontUnderline(bool param) { m_dataFontUnderline = param; }
    bool GetDataFontUnderline() { return m_dataFontUnderline; }
    void SetHeadingFontUnderline(bool underline) { m_headingFontUnderline = underline; }
    bool GetHeadingFontUnderline() { return m_headingFontUnderline; }
    void SetLabelFontUnderline(bool param) { m_labelFontUnderline = param; }
    bool GetLabelFontUnderline() { return m_labelFontUnderline; }
    void SetDataFontStrikethrough(bool param) { m_dataFontStrikethrough = param; }
    bool GetDataFontStrikethrough() { return m_dataFontStrikethrough; }
    void SetHeadingFontStrikethrough(bool strikethrough) { m_headingFontStrikethrough = strikethrough; }
    bool GetHeadingFontStrikethrough() { return m_headingFontStrikethrough; }
    void SetLabelFontStrikethrough(bool param) { m_labelFontStrikethrough = param; }
    bool GetLabelFontStrikethrough() { return m_labelFontStrikethrough; }
    const std::vector<Field *> &GetFields() { return table_fields; }
    std::map<int,std::vector<FKField *> > &GetForeignKeyVector() { return foreign_keys; }
    int GetTableId() { return m_objectId; }
    void SetTableId(int id) { m_objectId = id; }
private:
    std::wstring table_name, schema_name, comment;
    std::vector<Field *> table_fields;
    std::map<int,std::vector<FKField *> > foreign_keys;
    std::wstring m_dataFontName, m_labelFontName, m_headingFontName;
    int m_dataFontWeight, m_labelFontWeight, m_headingFontWeight, m_objectId;
    bool m_dataFontItalic, m_labelFontItalic, m_headingFontItalic;
    int m_dataFontSize, m_labelFontSize, m_headingFontSize;
    bool m_dataFontUnderline, m_labelFontUnderline, m_headingFontUnderline;
    bool m_dataFontStrikethrough, m_labelFontStrikethrough, m_headingFontStrikethrough;
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
    virtual bool IsTablePropertiesExist(const std::wstring &tableName, const std::wstring &schemaName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableListFromDb(std::vector<std::wstring> &errorMsg) = 0;
public:
    virtual ~Database() = 0;
    Impl &GetTableVector() { return *pimpl; };
    virtual int CreateDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DropDatabase(const std::wstring &name, std::vector<std::wstring> &errorMsg) = 0;
    virtual int Connect(std::wstring selectedDSN, std::vector<std::wstring> &errorMsg) = 0;
    virtual int Disconnect(std::vector<std::wstring> &errorMsg) = 0;
    virtual int CreateIndex(const std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual bool IsIndexExists(const std::wstring &indexName, const std::wstring &schemaName, const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetTableProperties(DatabaseTable *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual int GetFieldProperties(const std::wstring &tableName, const std::wstring &schemaName, const std::wstring &fieldName, Field *table, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetTableProperties(const DatabaseTable *table, const TableProperties &properties, bool isLog, std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetFieldProperties(const std::wstring &command, std::vector<std::wstring> &errorMsg) = 0;
    virtual int ApplyForeignKey(const std::wstring &command, const std::wstring &keyName, DatabaseTable &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int DeleteTable(const std::wstring &tableName, std::vector<std::wstring> &errorMsg) = 0;
    virtual int SetColumnComment(const std::wstring &tableName, const std::wstring &fieldName, const std::wstring &comment, std::vector<std::wstring> &errorMsg) = 0;
};

struct Database::Impl
{
    std::map<std::wstring, std::vector<DatabaseTable *> > m_tables;
    std::wstring m_dbName, m_type, m_subtype, m_connectString, m_connectedUser;
	const std::wstring &GetConnectedUser() { return m_connectedUser; };
	void SetConnectedUser(const std::wstring &user) { m_connectedUser = user; };
};

inline Database::~Database()
{
    delete pimpl;
}

#endif
