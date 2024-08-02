#pragma once

struct DesignLabelProperties
{
    wxString m_name, m_tag, m_text;
    bool m_suppressPrint;
    int m_border, m_alignment;
    wxFont m_font;
    wxSize m_size;
    wxPoint m_position;
};

class LabelPropertiesHandler :  public PropertiesHandler
{
public:
    LabelPropertiesHandler(DesignLabelProperties *prop);
    wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    DesignLabelProperties *GetOptions() { return m_prop; }
    virtual int ApplyProperties() wxOVERRIDE;
private:
    DesignLabelProperties *m_prop;
};
