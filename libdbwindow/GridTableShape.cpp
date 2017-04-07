#include "wxsf/GridShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/ShapeBase.h"
#include "database.h"
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
    int row = (int) m_arrCells.GetCount() / m_nCols;
    if( wxDynamicCast( shape, FieldShape ) )
        col = 1;
    else if( wxDynamicCast( shape, wxSFTextShape ) )
        col = 2;
    else
        col = 0;
    return InsertToGrid( row, col, shape );
}

void GridTableShape::DoChildrenLayout()
{
    if( !m_nCols || !m_nRows )
        return;
    wxSFShapeBase *pShape;
    int nIndex, nRow, nCol;
    wxRect currRect, maxRect0 = wxRect( 0, 0, 0, 0 ), maRect1 = wxRect( 0, 0, 0, 0 ), wxRect2 = wxRect( 0, 0, 0, 0 );
    SerializableList::compatibility_iterator node = GetFirstChildNode();
    while( node )
    {
        pShape = (wxSFShapeBase*) node->GetData();
        currRect = pShape->GetBoundingBox();
        if( pShape->IsKindOf( wxSFBitmapShape ) || pShape->IsKindOf( ) )
        {
            if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect0.GetWidth() ) )
                maxRect0.SetWidth( currRect.GetWidth() );
            if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect0.GetHeight() ) )
                maxRect0.SetHeight( currRect.GetHeight() );
        }
        if( pShape->IsKindOf( FieldShape ) )
        {
            if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect1.GetWidth() ) )
                maxRect1.SetWidth( currRect.GetWidth() );
            if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect1.GetHeight() ) )
                maxRect1.SetHeight( currRect.GetHeight() );
        }
        if( pShape->IsKindOf( wxSFTextShape ) )
        {
            if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect2.GetWidth() ) )
                maxRect2.SetWidth( currRect.GetWidth() );
            if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect2.GetHeight() ) )
                maxRect2.SetHeight( currRect.GetHeight() );
        }
        node = node->GetNext();
    }
    nIndex = nCol = 0;
    nRow = -1;
    for( size_t i = 0; i < m_arrCells.GetCount(); i++ )
    {
        pShape = (wxSFShapeBase*) GetChild( m_arrCells[i] );
        if( pShape )
        {
            if( nIndex++ % m_nCols == 0 )
            {
                nCol = 0;
                nRow++;
            }
            else
                nCol++;
            if( nCol == 0 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect0.GetWidth() + (nCol + 1 ) * m_nCellSpace,	nRow * maxRect0.GetHeight() + (nRow + 1) * m_nCellSpace,
                    maxRect0.GetWidth(), maxRect0.GetHeight() ) );
            if( nCol == 1 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect1.GetWidth() + (nCol + 1) * m_nCellSpace, nRow * maxRect1.GetHeight() + (nRow + 1) * m_nCellSpace,
                    maxRect1.GetWidth(), maxRect1.GetHeight() ) );
            if( nCol == 2 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect2.GetWidth() + (nCol + 1) * m_nCellSpace, nRow * maxRect2.GetHeight() + (nRow + 1) * m_nCellSpace,
                    maxRect2.GetWidth(), maxRect2.GetHeight() ) );
		}
	}