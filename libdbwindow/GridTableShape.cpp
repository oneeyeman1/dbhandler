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
        col = 1;
    else if( wxDynamicCast( shape, wxSFTextShape ) )
        col = 2;
    else
        col = 0;
    return InsertToGrid( row, col, shape );
}
