//
//  propertieshandlerbase.h
//  libpropertieshandlers
//
//  Created by Igor Korot on 8/18/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef propertieshandlerbase_h
#define propertieshandlerbase_h

#define UNUSED(str)

enum
{
    DatabaseTablePropertiesType,
    DatabaseFieldPropertiesType,
    DesignPropertiesType,
    DesignLabelPropertiesType,
    DesignFieldPropertiesType,
    DividerPropertiesType,
    SignPropertiesType,
    DatabasePropertiesType,
    QueryPropertiesType,
    LibraryPropertiesType,
    LibraryObjectsProperties,
    LibraryPainterPropertiesType,
    TablePrpertiesType
};

class
#ifdef __WXMSW__
 __declspec(dllexport)
#endif
 PropertiesHandler
{
public:
    PropertiesHandler() { }
    virtual ~PropertiesHandler() { }
//    virtual void EditProperies(wxNotebook *parent) = 0;
    virtual int ApplyProperties(const wxAny &any, bool logOnly, std::wstring &command) = 0;
    virtual wxAny &GetProperties() = 0;
    virtual bool IsLogOnly() const { return false; };
    virtual const std::wstring &GetCommand() const { return L""; };
    void SetType(int type) { m_object = type; };
    int GetType() const { return m_object; };
    void SetDatabase(const Database *db) { m_db = const_cast<Database *>( db ); }
    void SetTable(const DatabaseTable *table) { m_table = const_cast<DatabaseTable *>( table ); }
    Database *GetDatabase() const { return m_db; }
    DatabaseTable *GetTable() const { return m_table; }
    void SetFieldType(const wxString type) { m_type = type; }
    const wxString &GetFieldType() const { return m_type; }
protected:
    int m_object;
    wxAny m_any;
    Database *m_db;
    DatabaseTable *m_table;
    wxString m_type, m_objectName;
};



#endif /* propertieshandlerbase_h */
