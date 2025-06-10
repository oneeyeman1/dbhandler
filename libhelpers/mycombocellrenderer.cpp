//
//  colorcombobox.cpp
//  libpropertypages
//
//  Created by Igor Korot on 9/17/20.
//  Copyright © 2020 Igor Korot. All rights reserved.
//
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <wx/grid.h>
#include <wx/renderer.h>
#include "mycombocellrenderer.h"

wxGridCellRenderer *MyComboCellRenderer::Clone() const
{
    return new MyComboCellRenderer();
}

void MyComboCellRenderer::Draw(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, const wxRect& rect, int row, int col, bool isSelected)
{
    auto value = grid.GetTable()->GetValue( row, col );
    wxGridCellRenderer::Draw( grid, attr, dc, rect, row, col, isSelected );
    wxRendererNative::Get().DrawChoice( &grid, dc, rect );
    dc.DrawText( value, rect.GetLeft() + 5, rect.GetTop() );
}

wxSize MyComboCellRenderer::GetBestSize(wxGrid &grid, wxGridCellAttr& attr, wxDC& dc, int row, int col)
{
    return dynamic_cast<wxComboBox *>( grid.GetCellEditor( row, col )->GetControl() )->GetBestSize();
}
