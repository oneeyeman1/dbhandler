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
#include "database.h"
#include "propertypagebase.h"
#include "fieldgeneral.h"
#include "fieldheader.h"
#include "propertieshandlerbase.h"
#include "fieldpropertieshandler.h"

FieldPropertiesHandler::FieldPropertiesHandler(const Database *db, Field *field)
{
    m_db = db;
    m_field = field;
    m_prop = m_field->GetFieldProperties();
}

void FieldPropertiesHandler::EditProperies(wxNotebook *parent)
{
    wxString comment( m_prop.m_comment );
    m_page1 = new FieldGeneral( parent, comment );
    parent->AddPage( m_page1, _( "General" ) );    m_page2 = new FieldHeader( parent, m_prop.m_label, m_prop.m_heading, m_prop.m_labelPosition, m_prop.m_headingPosition );
    parent->AddPage( m_page2, _( "Data Font" ) );
}

int FieldPropertiesHandler::GetProperties(std::vector<std::wstring> &errors)
{
    return 0;
}
