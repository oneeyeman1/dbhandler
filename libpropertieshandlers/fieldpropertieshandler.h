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
    FieldPropertiesHandler(const Database *db, const wxString &tableName, const wxString &ownerName, TableField *field, wxTextCtrl *log);
    virtual ~FieldPropertiesHandler() { }
//    virtual void EditProperies(wxNotebook *parent) wxOVERRIDE;
    virtual wxAny GetProperties(std::vector<std::wstring> &errors) wxOVERRIDE;
    virtual int ApplyProperties() wxOVERRIDE;
    FieldProperties &GetProperty() { return m_prop; }
    virtual const std::wstring &GetCommand() const wxOVERRIDE { return m_command; }
    
private:
    TableField *m_field;
    FieldProperties m_prop;
    FieldVisualAttributes m_vsual;
    const Database *m_db;
    std::wstring m_command;
    wxString m_tableName, m_ownerName;
    wxTextCtrl *m_log;
};


#endif /* fieldpropertieshandler_hpp */
