#pragma once

struct DatabasePropertiesGeneral
{
    wxString m_sharedProfile, m_sqlTerminator, m_tableRefresh, m_tableColumns;
    bool m_tableLst, m_useRepo, m_readOnly, m_keepAlive;
};

struct DatabasePropertiesColors
{

};

struct DatabaseProperties
{
    DatabasePropertiesGeneral m_general;
    DatabasePropertiesColors m_colors;
};

class WXEXPORT DatabaseOptionsHandler : public PropertiesHandler
{
public:
    DatabaseOptionsHandler(DatabaseProperties options);
    const DatabaseProperties GetObjectProperties() const { return m_options; }
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
private:
    DatabaseProperties m_options;
};
