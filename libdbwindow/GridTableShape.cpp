#include "wxsf/BitmapShape.h"
#include "wxsf/GridShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/ShapeBase.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "propertieshandlerbase.h"
#include "configuration.h"
#include "FieldShape.h"
#include "fieldtypeshape.h"
#include "commentfieldshape.h"
#include "GridTableShape.h"

#define DISPLAYTYPES 1

GridTableShape::GridTableShape(ViewType type) : wxSFGridShape()
{
    m_type = type;
    m_showdatatypes = m_showcomments = true;
    m_columns = 3;
}

GridTableShape::~GridTableShape(void)
{
}

bool GridTableShape::InsertToTableGrid(wxSFShapeBase *shape)
{
    int col;
    int row = (int) m_arrCells.GetCount() / m_nCols;
    if( m_type == DatabaseView )
    {
        if( wxDynamicCast( shape, FieldShape ) )
            col = 1;
        else if( wxDynamicCast( shape, wxSFTextShape ) )
            col = 2;
        else
            col = 0;
    }
    else
    {
        if( wxDynamicCast( shape, CommentFieldShape ) )
        {
            if( m_showdatatypes )
                col = 2;
            else
                col = 1;
        }
        else if( wxDynamicCast( shape, FieldShape ) )
            col = 0;
		else
            col = 1;
    }
    return InsertToGrid( row, col, shape );
}

void GridTableShape::DoChildrenLayout()
{
    if( !m_nCols || !m_nRows )
        return;
    bool isKeyPresent = false;
    wxSFShapeBase *pShape;
    int nIndex, nRow, nCol;
    wxRect currRect, maxRect0 = wxRect( 0, 0, 0, 0 ), maxRect1 = wxRect( 0, 0, 0, 0 ), maxRect2 = wxRect( 0, 0, 0, 0 );
    SerializableList::compatibility_iterator node = GetFirstChildNode();
    while( node )
    {
        pShape = (wxSFShapeBase*) node->GetData();
        currRect = pShape->GetBoundingBox();
        if( m_type == DatabaseView )
        {
            wxSFBitmapShape *temp = wxDynamicCast( pShape, wxSFBitmapShape );
            if( temp )
            {
                isKeyPresent = true;
                if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect0.GetWidth() ) )
                    maxRect0.SetWidth( currRect.GetWidth() );
                if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect0.GetHeight() ) )
                    maxRect0.SetHeight( currRect.GetHeight() );
            }
            else
            {
                FieldShape *temp2 = wxDynamicCast( pShape, FieldShape );
                if( temp2 )
                {
                    if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect1.GetWidth() ) )
                        maxRect1.SetWidth( currRect.GetWidth() );
                    if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect1.GetHeight() ) )
                        maxRect1.SetHeight( currRect.GetHeight() );
                }
                else
                {
                    CommentFieldShape *temp3 = wxDynamicCast( pShape, CommentFieldShape );
                    if( temp3 )
                    {
                        if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect2.GetWidth() ) )
                            maxRect2.SetWidth( currRect.GetWidth() );
                        if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect2.GetHeight() ) )
                            maxRect2.SetHeight( currRect.GetHeight() );
                    }
                    else
                    {
                        wxSFShapeBase *temp4 = wxDynamicCast( pShape, wxSFShapeBase );
                        if( temp4 && !isKeyPresent )
                        {
//                            if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect0.GetWidth() ) )
                                maxRect0.SetWidth( 2 );
//                            if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect0.GetHeight() ) )
                                maxRect0.SetHeight( 14 );
                        }
                    }
                }
            }
            node = node->GetNext();
        }
        else
        {
            FieldShape *temp = wxDynamicCast( pShape, FieldShape );
            if( temp )
            {
                if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect0.GetWidth() ) )
                    maxRect0.SetWidth( currRect.GetWidth() );
                if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect0.GetHeight() ) )
                    maxRect0.SetHeight( currRect.GetHeight() );
            }
            else
            {
                FieldTypeShape *temp2 = wxDynamicCast( pShape, FieldTypeShape );
                if( temp2 )
                {
                    if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect1.GetWidth() ) )
                        maxRect1.SetWidth( currRect.GetWidth() );
                    if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect1.GetHeight() ) )
                        maxRect1.SetHeight( currRect.GetHeight() );
                }
                else
                {
                    CommentFieldShape *temp3 = wxDynamicCast( pShape, CommentFieldShape );
                    if( temp3 )
                    {
                        if( ( pShape->GetHAlign() != halignEXPAND ) && ( currRect.GetWidth() > maxRect2.GetWidth() ) )
                            maxRect2.SetWidth( currRect.GetWidth() );
                        if( ( pShape->GetVAlign() != valignEXPAND ) && ( currRect.GetHeight() > maxRect2.GetHeight() ) )
                            maxRect2.SetHeight( currRect.GetHeight() );
                    }
                }
            }
            node = node->GetNext();
        }
    }
    nCol = -1;
    nIndex = 0;
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
                FitShapeToRect( pShape, wxRect( nCol * maxRect0.GetWidth() + (nCol + 1 ) * m_nCellSpace, nRow * maxRect0.GetHeight() + (nRow + 1) * m_nCellSpace,
                    maxRect0.GetWidth(), maxRect0.GetHeight() ) );
            if( nCol == 1 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect1.GetWidth() + (nCol + 1) * m_nCellSpace, nRow * maxRect1.GetHeight() + (nRow + 1) * m_nCellSpace,
                    maxRect1.GetWidth(), maxRect1.GetHeight() ) );
            if( nCol == 2 )
                FitShapeToRect( pShape, wxRect( nCol * maxRect2.GetWidth() + (nCol + 1) * m_nCellSpace, nRow * maxRect2.GetHeight() + (nRow + 1) * m_nCellSpace,
                                               maxRect2.GetWidth(), maxRect2.GetHeight() ) );
		}
	}
}

void GridTableShape::ShowDataTypes(bool show, int type)
{
    int rows, cols;
    if( type == DISPLAYTYPES )
        m_showdatatypes = show;
    else
        m_showcomments = show;
    if( !m_showdatatypes && !m_showcomments )
        m_columns = 1;
    else if( ( !m_showcomments && m_showdatatypes  ) || ( !m_showdatatypes && m_showcomments ) )
        m_columns = 2;
    else
        m_columns = 3;
    GetDimensions( &rows, &cols );
    ClearGrid();
    SetDimensions( rows, m_columns );

}
