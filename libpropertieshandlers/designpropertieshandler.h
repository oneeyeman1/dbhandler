#pragma once

class WXEXPORT DesignPropertiesHander : public PropertiesHandler
{
public:
    DesignPropertiesHander(DesignOptions *options);

    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
private:
    DesignOptions *m_options;
    DesignGeneral *m_page1;
};
