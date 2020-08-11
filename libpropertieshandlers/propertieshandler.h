#pragma once
class WXEXPORT PropertiesHandler
{
public:
    PropertiesHandler() {}
    ~PropertiesHandler() {}
    PropertiesHandler(const PropertiesHandler &props) { }
    virtual void EditProperies(wxNotebook *parent) = 0;
    virtual void GetProperties() = 0;
};

class WXEXPORT DatabasePropertiesHandler : public PropertiesHandler
{
public:
    DatabasePropertiesHandler(const Database *db, DatabaseTable *table);
    virtual ~DatabasePropertiesHandler() {};
    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual void GetProperties() wxOVERRIDE;
    TableProperties &GetProperty() { return m_prop; }

private:
    TableProperties m_prop;
    TableGeneralProperty *m_page1;
    CFontPropertyPage *m_page2, *m_page3, *m_page4;
    const Database *m_db;
};

