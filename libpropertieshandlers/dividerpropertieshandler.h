#pragma once

struct BandProperties
{
    wxString m_color;
    wxString m_cursorFile, m_stockCursor;
    wxString m_type;
    int m_height, m_cursor;
};

class WXEXPORT DividerPropertiesHander : public PropertiesHandler
{
public:
    DividerPropertiesHander(BandProperties props);
    virtual ~DividerPropertiesHander() { }
//    virtual void EditProperies(wxNotebook *parent)  wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;

private:
    BandProperties m_props;
};