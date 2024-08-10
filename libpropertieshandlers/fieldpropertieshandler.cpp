//
//  fieldpropertieshandler.cpp
//  libpropertieshandlers
//
//  Created by Igor Korot on 8/18/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/stockitem.h"
#endif

#include <vector>
#include <map>
#include "wx/notebook.h"
#include "wx/any.h"
#include "database.h"
#include "propertieshandlerbase.h"
#include "fieldpropertieshandler.h"

FieldPropertiesHandler::FieldPropertiesHandler(const Database *db, const wxString &tableName, const wxString &ownerName, TableField *field, wxTextCtrl *log)
{
    m_db = db;
    m_tableName = tableName;
    m_ownerName = ownerName;
    m_field = field;
    m_log = log;
    m_prop = m_field->GetFieldProperties();
}

wxAny FieldPropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
/*    m_prop.m_comment = m_page1->GetCommentCtrl()->GetValue();
    m_prop.m_label = m_page2->GetLabelCtrl()->GetValue();
    m_prop.m_heading = m_page2->GetHeadingCtrl()->GetValue();
    m_prop.m_labelPosition = m_page2->GetLabeAlignmentCtrl()->GetSelection();
    m_prop.m_headingPosition = m_page2->GetHeaderAlignmentCtrl()->GetSelection();
    bool isLogOnly = m_page1->IsLogOnly();
    {
#if defined __WXMSW__ && _MSC_VER < 1900
        wxCriticalSectionLocker( *pcs );
#else
        //#if _MSC_VER >= 1900 || !(defined __WXMSW__)
        std::lock_guard<std::mutex> lock( const_cast<Database *>( m_db )->GetTableVector().my_mutex );
#endif
        int result = const_cast<Database *>( m_db )->SetFieldProperties( m_tableName.ToStdWstring(), m_ownerName.ToStdWstring(), m_field->GetFieldName(), m_prop, isLogOnly, m_command, errors );
        if( !result && !isLogOnly )
            m_field->SetFieldProperties( m_prop );
        if( isLogOnly )
            m_log->AppendText( m_command );
    }*/
    return wxAny( 0 );
}

int FieldPropertiesHandler::ApplyProperties()
{
    return 0;
}
