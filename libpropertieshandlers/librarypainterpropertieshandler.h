#pragma once
class WXEXPORT LibraryPainterPropertiesHandler : public PropertiesHandler
{
public:
    LibraryPainterPropertiesHandler(LibraryPainterOptions *prop);
    wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    LibraryPainterOptions *GetOptions() { return m_prop.As<LibraryPainterOptions *>(); }
    virtual int ApplyProperties() wxOVERRIDE;
private:
    wxAny m_prop;
};

