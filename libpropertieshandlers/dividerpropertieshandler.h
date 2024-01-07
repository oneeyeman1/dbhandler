#pragma once

struct BandProperties
{
    wxString m_color = "Transparent";
    wxString m_cursorFile, m_stockCursor;
    wxString m_type;
    int m_height = 200, m_cursor = -1;
    bool m_autosize = false;
};

class WXEXPORT DividerPropertiesHander : public PropertiesHandler
{
public:
    DividerPropertiesHander(BandProperties props);
    virtual ~DividerPropertiesHander() { }
    const BandProperties &GetObjectProperties() const { return m_props; }
//    virtual void EditProperies(wxNotebook *parent)  wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;

private:
    BandProperties m_props;
};