/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * table-canvas.cc
 * Copyright (C) 2017 Unknown <igor@IgorDellGentoo>
 *
 * dbhandler is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * dbhandler is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/grid.h"
#include "wx/arrstr.h"
#include "wx/docmdi.h"
#include "database.h"
#include "typecombobox.h"
#include "tablecanvas.h"

TableCanvas::TableCanvas(wxView *view, const wxPoint &pt, Database *db, DatabaseTable *table, wxWindow *parent) : wxGrid(view->GetFrame(), wxID_ANY)
{
    wxArrayString fieldTypes;
    std::wstring type = db->GetTableVector().m_type, subtype = db->GetTableVector().m_subtype;
    if( type == L"SQLite" )
    {
        fieldTypes.Add( "blob" );
        fieldTypes.Add( "integer" );
        fieldTypes.Add( "numeric" );
        fieldTypes.Add( "real" );
        fieldTypes.Add( "text" );
    }
    else
    {
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "JSON" );
        fieldTypes.Add( "bigint" );
        fieldTypes.Add( "binary" );
        fieldTypes.Add( "bit" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "bool" );
        fieldTypes.Add( "char" );
        fieldTypes.Add( "cursor" );
        fieldTypes.Add( "date" );
        fieldTypes.Add( "datetime" );
        fieldTypes.Add( "datetime2" );
        fieldTypes.Add( "datetimeoffset" );
        fieldTypes.Add( "decimal" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
        {
            fieldTypes.Add( "double" );
            fieldTypes.Add( "double precision" );
            fieldTypes.Add( "enum" );
        }
        fieldTypes.Add( "float" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
        {
            fieldTypes.Add( "geometry" );
            fieldTypes.Add( "geometrycollection" );
        }
        fieldTypes.Add( "hierarchyid" );
        fieldTypes.Add( "image" );
        fieldTypes.Add( "int" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
        {
            fieldTypes.Add( "linestring" );
            fieldTypes.Add( "longblob" );
            fieldTypes.Add( "longtext" );
            fieldTypes.Add( "mediumblob" );
            fieldTypes.Add("mediumint");
            fieldTypes.Add( "mediumtext" );
        }
        fieldTypes.Add( "money" );
        if( ( type == L"ODBC" && subtype == L"MyODBC" ) || type == L"MyODBC" )
        {
            fieldTypes.Add( "multilinestring" );
            fieldTypes.Add( "multipoint" );
            fieldTypes.Add( "multipolygon" );
        }
        fieldTypes.Add( "nchar" );
        fieldTypes.Add( "ntext" );
        fieldTypes.Add( "numeric" );
        fieldTypes.Add( "nvarchar" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
        {
            fieldTypes.Add( "point" );
            fieldTypes.Add( "polygon" );
        }
        fieldTypes.Add( "real" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "set" );
        fieldTypes.Add( "smalldatetime" );
        fieldTypes.Add( "smallint" );
        fieldTypes.Add( "smallmoney" );
        fieldTypes.Add( "sql_variant" );
        fieldTypes.Add( "table" );
        fieldTypes.Add( "text" );
        fieldTypes.Add( "time" );
        fieldTypes.Add( "timestamp" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "tinyblob" );
        fieldTypes.Add( "tinyint" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "tinytext" );
		fieldTypes.Add("uniqueidentifier");
        fieldTypes.Add( "varbinary" );
        fieldTypes.Add( "varchar" );
        fieldTypes.Add( "xml" );
        if( ( type == L"ODBC" && subtype == L"MySQL" ) || type == L"MySQL" )
            fieldTypes.Add( "year" );
    }
	{
		fieldTypes.Add("bigint");
		fieldTypes.Add("bit");
		fieldTypes.Add("boolean");  // mySQL specific
		fieldTypes.Add("decimal");
		fieldTypes.Add("int");
		fieldTypes.Add("smallint");
		fieldTypes.Add("tinyint");


		fieldTypes.Add("binary");
		fieldTypes.Add("char");
		fieldTypes.Add("cursor");
		fieldTypes.Add("date");
		fieldTypes.Add("datetime");
		fieldTypes.Add("datetime2");
		fieldTypes.Add("datetimeoffset");
		fieldTypes.Add("float");
		fieldTypes.Add("hierarchyid");
		fieldTypes.Add("image");
		fieldTypes.Add("money");
		fieldTypes.Add("nchar");
		fieldTypes.Add("ntext");
		fieldTypes.Add("numeric");
		fieldTypes.Add("nvarchar");
		fieldTypes.Add("real");
		fieldTypes.Add("smalldatetime");
		fieldTypes.Add("smallmoney");
		fieldTypes.Add("sql_variant");
		fieldTypes.Add("table");
		fieldTypes.Add("text");
		fieldTypes.Add("time");
		fieldTypes.Add("timestamp");
		fieldTypes.Add("uniqueidentifier");
		fieldTypes.Add("varbinary");
		fieldTypes.Add("varchar");
		fieldTypes.Add("xml");
	}
    CreateGrid(1, 6);
    SetColLabelValue( 0, _( "Column Name" ) );
    SetColLabelValue( 1, _( "Data Type" ) );
    SetColLabelValue( 2, _( "Width" ) );
    SetColLabelValue( 3, _( "Dec" ) );
    SetColLabelValue( 4, _( "Null" ) );
    SetColLabelValue( 5, _( "Default" ) );
    if( !table )
    {
        AppendRows();
        SetCellRenderer( 0, 1, new FieldTypeRenderer( "" ) );
    }
    else
    {
        int i = 0;
        for( std::vector<Field *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); it++)
        {
            AppendRows();
            SetCellValue( i, 0, (*it)->GetFieldName() );
            SetCellRenderer( i, 1, new FieldTypeRenderer( "" ) );
            SetCellEditor( i, 1, new wxGridCellChoiceEditor( fieldTypes ) );
            SetCellValue( i, 1, (*it)->GetFieldType() );
            SetCellValue( i, 2, (*it)->GetFieldSize() );
            SetCellValue( i, 3, (*it)->GetPrecision() );
			SetCellValue( i, 4, (*it)->IsNullAllowed() ? _( "Yes" ) : _( "No" ) );
            SetCellValue( i, 5, (*it)->GetDefaultValue() );
            i++;
        }
    }
    AutoSizeColumns();
}
