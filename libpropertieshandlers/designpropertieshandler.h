#pragma once

struct DesignOptions
{
    int units, interval, display;
    wxColour colorBackground;
    bool customMove, mouseSelect, rowResize;
    int cursor;
    wxString cursorName;
};

class WXEXPORT DesignPropertiesHander : public PropertiesHandler
{
public:
    DesignPropertiesHander(DesignOptions canvas);
    virtual ~DesignPropertiesHander() { }
//    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;
private:
    DesignOptions m_options;
};
