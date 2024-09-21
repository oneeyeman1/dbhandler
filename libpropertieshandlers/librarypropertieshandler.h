#pragma once
class WXEXPORT LibraryPropertiesHandler :  public PropertiesHandler
{
public:
    LibraryPropertiesHandler(LibraryProperty prop);
    virtual int ApplyProperties() override;
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) override;
    const LibraryProperty &GetLibraryProperties() const { return m_properties; }
private:
    LibraryProperty m_properties;
};

