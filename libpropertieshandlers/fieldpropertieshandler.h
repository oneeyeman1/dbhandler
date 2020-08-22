//
//  fieldpropertieshandler.hpp
//  libpropertieshandlers
//
//  Created by Igor Korot on 8/18/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//

#ifndef fieldpropertieshandler_hpp
#define fieldpropertieshandler_hpp

class WXEXPORT FieldPropertiesHandler : public PropertiesHandler
{
public:
    FieldPropertiesHandler(const Database *db, const wxString &tableName, const wxString &ownerName, Field *field);
    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual int GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    FieldProperties &GetProperty() { return m_prop; }
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
    virtual bool IsLogOnly() const wxOVERRIDE { return m_page1->IsLogOnly(); }
    
private:
    Field *m_field;
    FieldProperties m_prop;
    FieldGeneral *m_page1;
    FieldHeader *m_page2;
    const Database *m_db;
    std::wstring m_command;
    wxString m_tableName, m_ownerName;
};


#endif /* fieldpropertieshandler_hpp */
