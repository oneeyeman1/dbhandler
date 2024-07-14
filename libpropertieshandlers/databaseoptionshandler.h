#pragma once

class WXEXPORT DatabaseOptionsHandler : public PropertiesHandler
{
public:
    DatabaseOptionsHandler(DatabaseOptions options);
    const DatabaseOptions &GetObjectProperties() const { return m_options; }
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
private:
    DatabaseOptions m_options;
};
