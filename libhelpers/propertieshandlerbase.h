//
//  propertieshandlerbase.h
//  libpropertieshandlers
//
//  Created by Igor Korot on 8/18/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef propertieshandlerbase_h
#define propertieshandlerbase_h

#define UNUSED(str)

enum
{
    DatabaseTablePropertiesType,
    DatabaseFieldPropertiesType,
    DesignPropertiesType,
    DesignLabelPropertiesType,
    DesignFieldPropertiesType,
    DividerPropertiesType,
    SignPropertiesType,
    DatabasePropertiesType,
    QueryPropertiesType,
    LibraryPropertiesType,
    LibraryObjectsProperties,
    LibraryPainterPropertiesType
};

class
#ifdef __WXMSW__
 __declspec(dllexport)
#endif
 PropertiesHandler
{
public:
    PropertiesHandler() { }
    virtual ~PropertiesHandler() { }
//    virtual void EditProperies(wxNotebook *parent) = 0;
    virtual int ApplyProperties() = 0;
    virtual wxAny &GetProperties() = 0;
    virtual bool IsLogOnly() const { return false; };
    virtual const std::wstring &GetCommand() const { return L""; };
    void SetType(int type) { m_object = type; };
    int GetType() const { return m_object; };
protected:
    int m_object;
    wxAny any;
};



#endif /* propertieshandlerbase_h */
