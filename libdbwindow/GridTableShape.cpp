#include "wxsf/gridshape.h"
#include "wxsf/TextShape.h"
#include "wxsf/ShapeBase.h"
#include "FieldShape.h"
#include "GridTableShape.h"

GridTableShape::GridTableShape(void) : wxSFGridShape()
{
}

GridTableShape::~GridTableShape(void)
{
}

bool GridTableShape::InsertToTableGrid(wxSFShapeBase *shape)
{
    int col;
    int row = m_arrCells.GetCount() / m_nCols;
    if( wxDynamicCast( shape, FieldShape ) )
        col = 2;
    else if( wxDynamicCast( shape, wxSFTextShape ) )
        col = 3;
    else
        col = 1;
    return InsertToGrid( row, col, shape );
}
