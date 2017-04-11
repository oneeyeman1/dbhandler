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
#include "wx/docmdi.h"
#include "database.h"
#include "tablecanvas.h"

TableCanvas::TableCanvas(wxView *view, const wxPoint &pt, DatabaseTable *table, wxWindow *parent) : wxGrid(parent, wxID_ANY)
{
    CreateGrid( 1, 6 );
    SetColLabelValue( 0, _( "Column Name" ) );
    SetColLabelValue( 1, _( "Data Type" ) );
    SetColLabelValue( 2, _( "Width" ) );
    SetColLabelValue( 3, _( "Dec" ) );
    SetColLabelValue( 4, _( "Null" ) );
    SetColLabelValue( 5, _( "Default" ) );
    if (!table)
        AppendRows();
    else
    {
        int i = 0;
        for( std::vector<Field *>::const_iterator it = table->GetFields().begin(); it < table->GetFields().end(); it++)
        {
            AppendRows();
            SetCellValue( i, 0, (*it)->GetFieldName() );
            i++;
        }
    }
}