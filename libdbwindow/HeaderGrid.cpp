#include <memory>
#include "wxsf/GridShape.h"
#include "wxsf/TextShape.h"
#include "wx/uilocale.h"
#include "database.h"
#include "commenttableshape.h"
#include "nametableshape.h"
#include "HeaderGrid.h"

HeaderGrid::HeaderGrid(void) : wxSFGridShape()
{
}

HeaderGrid::~HeaderGrid(void)
{
}

bool HeaderGrid::InsertToTableGrid( wxSFShapeBase *shape )
{
    int col = -1;
    int row = (int) m_arrCells.GetCount() / m_nCols;
    if( wxUILocale::GetCurrent().GetLayoutDirection() == wxLayout_Default ||
        wxUILocale::GetCurrent().GetLayoutDirection() == wxLayout_LeftToRight )
    {
        if( wxDynamicCast( shape, NameTableShape ) )
            col = 0;
        else if( wxDynamicCast( shape, CommentTableShape ) && m_showComments )
            col = 1;
    }
    else
    {
        if( wxDynamicCast( shape, NameTableShape ) )
            col = 1;
        else if( wxDynamicCast( shape, CommentTableShape ) && m_showComments )
            col = 0;
    }
    return InsertToGrid( row, col, shape );
}

void HeaderGrid::DoChildrenLayout()
{
    if( !m_nCols || !m_nRows )
        return;
    wxSFShapeBase *pShape;
    wxRect currRect2, maxRect0 = wxRect( 0, 0, 0, 0 ), maxRect1 = wxRect( 0, 0, 0, 0 );
    int nIndex = 0, nCol = 0, nRow = -1;
    SerializableList::compatibility_iterator node = GetFirstChildNode();
    if( node )
    {
        pShape = (wxSFShapeBase*) node->GetData();
        currRect2 = pShape->GetBoundingBox();
        NameTableShape *temp1 = wxDynamicCast( pShape, NameTableShape );
        if( temp1 )
        {
            if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect2.GetWidth() > maxRect0.GetWidth() ) )
                maxRect0.SetWidth( currRect2.GetWidth() );
            if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect2.GetHeight() > maxRect0.GetHeight() ) )
                maxRect0.SetHeight( currRect2.GetHeight() );
        }
        else
        {
            CommentTableShape *temp2 = wxDynamicCast( pShape, CommentTableShape );
            if( temp2 )
            {
                if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect2.GetWidth() > maxRect1.GetWidth() ) )
                    maxRect1.SetWidth( currRect2.GetWidth() );
                if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect2.GetHeight() > maxRect1.GetHeight() ) )
                    maxRect1.SetHeight( currRect2.GetHeight() );
            }
        }
        node = node->GetNext();
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
                FitShapeToRect( pShape, wxRect( nCol * maxRect0.GetWidth() + ( nCol + 1 ) * m_nCellSpace, nRow * maxRect0.GetHeight() + ( nRow + 1 ),
                                            maxRect0.GetWidth(), maxRect0.GetHeight() ) );
            if( nCol == 1 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect1.GetWidth() + ( nCol + 1 ) * m_nCellSpace, nRow * maxRect1.GetHeight() + ( nRow + 1 ),
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
