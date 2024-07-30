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
    int GetProperties(std::vector<std::wstring> &errors);
    DesignLabelProperties *GetOptions() { return m_prop; }
    virtual int ApplyProperties() wxOVERRIDE;
private:
    DesignLabelProperties *m_prop;
};
