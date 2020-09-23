#pragma once

class WXEXPORT DesignPropertiesHander : public PropertiesHandler
{
public:
    DesignPropertiesHander(DesignCanvas *canvas);
    virtual ~DesignPropertiesHander() { }
    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
private:
    DesignCanvas *m_canvas;
    DesignOptions m_options;
    DesignGeneral *m_page1;
    PointerPropertiesPanel *m_page2;
    PrintSpec *m_page3;
};
