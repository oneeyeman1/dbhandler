#include "wxsf/GridShape.h"
#include "HeaderGrid.h"

HeaderGrid::HeaderGrid(void)
{
}

HeaderGrid::~HeaderGrid(void)
{
}

void HeaderGrid::DoChildrenLayout()
{
    wxSFShapeBase *pShape;
    int nIndex = 0, nCol = 0, nRow = -1;
    wxRect maxRect;
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
            maxRect = pShape->GetBoundingBox();
            FitShapeToRect( pShape, wxRect( nCol * maxRect.GetWidth() + ( nCol + 1 ) * m_nCellSpace,
                                            nRow * maxRect.GetHeight() + ( nRow + 1 ) * m_nCellSpace,
                                            maxRect.GetWidth(), maxRect.GetHeight() ) );
        }
    }
}
