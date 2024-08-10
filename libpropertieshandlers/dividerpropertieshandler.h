#pragma once

struct BandProperties
{
    wxString m_color = "Transparent";
    wxString m_cursorFile, m_stockCursor;
    wxString m_type;
    int m_height = 200, m_cursor = -1;
    bool m_autosize = false;
};

class WXEXPORT DividerPropertiesHandler : public PropertiesHandler
{
public:
    DividerPropertiesHandler(BandProperties props);
    virtual ~DividerPropertiesHandler() { }
    const BandProperties &GetObjectProperties() const { return m_props; }
//    virtual void EditProperies(wxNotebook *parent)  wxOVERRIDE;
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;

private:
    BandProperties m_props;
};
