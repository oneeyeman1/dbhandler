#pragma once

struct DesignOptions
{
    int units, interval, display;
    wxColour colorBackground;
    bool customMove, mouseSelect, rowResize;
    int cursor;
    wxString cursorName;
};

class WXEXPORT DesignPropertiesHandler : public PropertiesHandler
{
public:
    DesignPropertiesHandler(DesignOptions canvas);
    virtual ~DesignPropertiesHandler() { }
//    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;
private:
    DesignOptions m_options;
};
