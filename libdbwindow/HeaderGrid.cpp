#include "wxsf/GridShape.h"
#include "HeaderGrid.h"

HeaderGrid::HeaderGrid(void) : wxSFGridShape()
{
}

HeaderGrid::~HeaderGrid(void)
{
}

void HeaderGrid::DoChildrenLayout()
{
    if( !m_nCols || !m_nRows )
        return;
    wxSFShapeBase *pShape;
    wxRect currRect1, currRect2, maxRect0 = wxRect( 0, 0, 0, 0 ), maxRect1 = wxRect( 0, 0, 0, 0 );
    int nIndex = 0, nCol = 0, nRow = -1;
    wxRect maxRect;
    SerializableList::compatibility_iterator node = GetFirstChildNode();
    pShape = (wxSFShapeBase*) node->GetData();
    currRect1 = pShape->GetBoundingBox();
    if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect1.GetWidth() > maxRect0.GetWidth() ) )
        maxRect0.SetWidth( currRect1.GetWidth() );
    if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect1.GetHeight() > maxRect0.GetHeight() ) )
        maxRect0.SetHeight( currRect1.GetHeight() );
    node = node->GetNext();
    if( node )
    {
        pShape = (wxSFShapeBase*) node->GetData();
        currRect2 = pShape->GetBoundingBox();
        if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect2.GetWidth() > maxRect1.GetWidth() ) )
            maxRect1.SetWidth( currRect2.GetWidth() );
        if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect2.GetHeight() > maxRect1.GetHeight() ) )
            maxRect1.SetHeight( currRect2.GetHeight() );
    }
    for( size_t i = 0; i < m_arrCells.GetCount(); i++ )
    {
        pShape = (wxSFShapeBase*) GetChild( m_arrCells[i] );
        if( pShape )
        {
            if( nIndex++ % m_nCols == 0 )
            {
                nCol = 0; nRow++;
            }
            else
                nCol++;
            if( nCol == 0 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect0.GetWidth() + ( nCol + 1 ) * m_nCellSpace, nRow * maxRect0.GetHeight() + ( nRow + 1 ) * m_nCellSpace,
                                            maxRect0.GetWidth(), maxRect0.GetHeight() ) );
            if( nCol == 1 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect0.GetWidth() + ( nCol + 1 ) * m_nCellSpace, nRow * maxRect0.GetHeight() + ( nRow + 1 ) * m_nCellSpace,
                                            maxRect1.GetWidth(), maxRect1.GetHeight() ) );
        }
    }
}

void HeaderGrid::ShowComments( bool show )
{
    ClearGrid();
    if( show )
        SetDimensions( 1, 2 );
    else
        SetDimensions( 1, 1 );
}
