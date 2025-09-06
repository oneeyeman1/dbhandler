//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : constraint.cpp
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wxxmlserializer/XmlSerializer.h>
#include "wxsf/BitmapShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/ShapeBase.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "configuration.h"
#include "constraint.h"
#include "constraintsign.h"

XS_IMPLEMENT_CLONABLE_CLASS(QueryConstraint,xsSerializable);

DatabaseConstraint::DatabaseConstraint(std::wstring fkName)
{
    m_viewType = DatabaseView;
    SetType( foreignKey );
    SetName( fkName );
    SetOnDelete( restrict );
    SetOnUpdate( restrict );
}

DatabaseConstraint::~DatabaseConstraint()
{
}

QueryConstraint::QueryConstraint(ViewType type)
{
    m_viewType = type;
    SetType( foreignKey );
    SetOnDelete( restrict );
    SetOnUpdate( restrict );
    InitSerializable();
}

QueryConstraint::QueryConstraint()
{
    m_viewType = DatabaseView;
    SetType( foreignKey );
    SetOnDelete( restrict );
    SetOnUpdate( restrict );
    InitSerializable();
}

QueryConstraint::QueryConstraint(const QueryConstraint& obj):Constraint(obj)
{
    m_viewType = obj.m_viewType;
    SetName( obj.GetName() );
    SetType( obj.GetType() );
    SetRefTable( obj.GetRefTable() );
    m_refCol = obj.m_refCol;
    SetOnDelete( obj.GetOnDelete() );
    SetOnUpdate( obj.GetOnUpdate() );
    InitSerializable();
}

QueryConstraint::QueryConstraint(const wxString& name, const wxString &localColumn, constraintType type, constraintAction onDelete, constraintAction onUpdate)
{
    SetName( name );
    SetType( type );
    SetOnDelete( onDelete );
    SetOnUpdate( onUpdate );
    InitSerializable();
}

QueryConstraint::~QueryConstraint()
{
}

void QueryConstraint::InitSerializable()
{
    wxString name = GetName(), refTable = const_cast<wxString &>( GetRefTable() );
    int type = GetType();
    wxString temp( GetLocalColumn() );
    constraintAction onDelete = GetOnDelete(), onUpdate = GetOnUpdate();
    XS_SERIALIZE( name, wxT( "name" ) );
    XS_SERIALIZE( temp, wxT( "localColumn" ) );
    XS_SERIALIZE_INT( type, wxT( "type" ) );
    XS_SERIALIZE( refTable, wxT( "refTable" ) );
    XS_SERIALIZE( m_refCol, wxT( "refCol" ) );
    XS_SERIALIZE_INT( onDelete, wxT( "onDelete" ) );
    XS_SERIALIZE_INT( onUpdate, wxT( "onUpdate" ) );
    XS_SERIALIZE_INT( m_sign, wxT( "querySign" ) );
}
