#pragma once

class WXEXPORT DatabaseOptionsHandler : public PropertiesHandler
{
public:
    DatabaseOptionsHandler(DatabaseOptions options);
    const DatabaseOptions &GetObjectProperties() const { return m_options; }
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;
private:
    DatabaseOptions m_options;
};
