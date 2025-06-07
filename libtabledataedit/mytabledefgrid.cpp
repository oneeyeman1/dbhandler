#include "wx/grid.h"
#include "mytabledefgrid.h"

MyTableDefGrid::MyTableDefGrid(wxWindow *parent, wxWindowID id) : wxGrid( parent, id), m_newRow( 0 )
{
//    Bind( wxEVT_GRID_CELL_LEFT_CLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_LEFT_DCLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_RIGHT_DCLICK, &MyTableDefGrid::OnCellClicked, this );
    Bind( wxEVT_GRID_SELECT_CELL, &MyTableDefGrid::OnCellClicked, this );
}

void MyTableDefGrid::DrawRowLabel(wxDC &dc, int row)
{
    wxGrid::DrawRowLabel( dc, row );
}

void MyTableDefGrid::OnCellClicked(wxGridEvent &event)
{
    auto row = event.GetRow();
}
