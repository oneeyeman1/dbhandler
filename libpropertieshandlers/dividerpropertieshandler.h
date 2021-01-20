#pragma once
class WXEXPORT DividerPropertiesHander : public PropertiesHandler
{
public:
    DividerPropertiesHander(BandProperties props);
    virtual ~DividerPropertiesHander() { }
    virtual void EditProperies(wxNotebook *parent)  wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;

private:
    BandGeneralProperties *m_page1;
    BandProperties m_props;
};