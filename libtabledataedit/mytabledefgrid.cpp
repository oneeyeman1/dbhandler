#include "wx/grid.h"
#include "mytabledefgrid.h"

MyTableDefGrid::MyTableDefGrid(wxWindow *parent, wxWindowID id) : wxGrid( parent, id), m_newRow( 0 )
{

}

void MyTableDefGrid::DrawRowLabel(wxDC &dc, int row)
{
    wxGrid::DrawRowLabel( dc, row );
}
