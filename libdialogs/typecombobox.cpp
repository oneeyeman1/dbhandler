/***************************************************************************
*   Copyright (C) 2005 by Igor Korot                                      *
*   igor@IgorsGentoo                                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "typecombobox.h"

TypeComboBox::TypeComboBox(wxWindow *parent, const std::wstring &type, const std::wstring &subtype) : wxComboBox(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, nullptr, wxCB_READONLY | wxCB_SORT)
{
    if( type == "SQLite" )
    {
        Append( "NULL" );
        Append( "Text" );
        Append( "Integer" );
        Append( "Real" );
        Append( "BLOB" );
    }
    if( ( type == "ODBC" && subtype == "Microsoft SQL Server" ) || type == "Microsoft SQL Server" )
    {
        Append( "bigint" );
        Append( "binary" );
        Append( "bit" );
        Append( "char" );
        Append( "date" );
        Append( "datetime" );
        Append( "datetime2" );
        Append( "datetimeoffset" );
        Append( "decimal" );
        Append( "float" );
        Append( "geography" );
        Append( "geometry" );
        Append( "hierarchyid" );
        Append( "image" );
        Append( "int" );
        Append( "money" );
        Append( "nchar" );
        Append( "ntextr" );
        Append( "numeric" );
        Append( "nvarchar" );
        Append( "real" );
        Append( "smalldatetime" );
        Append( "smallint" );
        Append( "smallmoney" );
        Append( "sql_variant" );
        Append( "text" );
        Append( "time" );
        Append( "timestamp" );
        Append( "tinyint" );
        Append( "uniqueidentifier" );
        Append( "varbinary" );
        Append( "varchar" );
        Append( "xml" );
    }
}