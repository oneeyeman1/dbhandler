#include "wx/wx.h"

#include <memory>
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/fontenum.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/bmpcbox.h"
#include "wx/listctrl.h"
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "database.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/CommonFcn.h"
#include "wxsf/RectShape.h"
#include "wxsf/LineShape.h"
#include "wxsf/RoundOrthoShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/GridShape.h"
#include "objectproperties.h"
#include "constraint.h"
#include "constraintsign.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "field.h"
#include "fieldwindow.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "MyErdTable.h"
#include "FieldShape.h"
#include "sortgroupbypage.h"
#include "wherehavingpage.h"
#include "syntaxproppage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
#include "designcanvas.h"
#include "databaseview.h"
#include "ErdLineShape.h"

using namespace wxSFCommonFcn;

ErdLineShape::ErdLineShape() : wxSFRoundOrthoLineShape()
{
    m_constraint = NULL;
    m_signConstraint = NULL;
    m_isEnabled = true;
    RemoveStyle( sfsSHOW_HANDLES );
    AcceptChild( "ConstraintSign" );
//    XS_SERIALIZE();
//    XS_SERIALIZE();
}

ErdLineShape::ErdLineShape(Constraint *pConstraint, ViewType type, const wxSFDiagramManager &pManager) : wxSFRoundOrthoLineShape()
{
    ShapeList listShapes;
    SerializableList sourceFields, targetFields;
    m_sourceTbl = m_targetTbl = NULL;
    m_type = type;
    AcceptChild( "ConstraintSign" );
    m_constraint = pConstraint;
    m_signConstraint = new ConstraintSign( type, 0 );
    if( m_signConstraint )
    {
        m_signConstraint->SetConstraint( pConstraint );
        m_signConstraint->SetVAlign( wxSFShapeBase::valignMIDDLE );
        m_signConstraint->SetStyle( wxSFShapeBase::sfsEMIT_EVENTS | wxSFShapeBase::sfsALWAYS_INSIDE | wxSFShapeBase::sfsPROCESS_DEL | wxSFShapeBase::sfsPROPAGATE_DRAGGING | wxSFShapeBase::sfsPROPAGATE_SELECTION | wxSFShapeBase::sfsLOCK_CHILDREN );
        m_signConstraint->AcceptChild( wxT( "wxSFTextShape" ) );
        m_signConstraint->Activate( true );
        RemoveStyle( sfsSHOW_HANDLES );
        SF_ADD_COMPONENT( m_signConstraint, wxT( "sign" ) );
    }
    const_cast<wxSFDiagramManager &>( pManager ).GetShapes( CLASSINFO( MyErdTable ), listShapes );
    bool found = false;
    for( ShapeList::iterator it = listShapes.begin(); it != listShapes.end() && !found; ++it )
    {
        if( dynamic_cast<MyErdTable *>( (*it) )->GetTableName() == const_cast<DatabaseTable *>( m_constraint->GetFKTable() )->GetTableName() )
            m_sourceTbl = dynamic_cast<MyErdTable *>( (*it) );
        if( dynamic_cast<MyErdTable *>( (*it) )->GetTableName() == m_constraint->GetRefTable() )
            m_targetTbl = dynamic_cast<MyErdTable *>( (*it) );
        if( m_sourceTbl && m_targetTbl )
            found = true;
    }
    m_sourceTbl->GetChildrenRecursively( CLASSINFO( FieldShape ), sourceFields );
    m_targetTbl->GetChildrenRecursively( CLASSINFO( FieldShape ), targetFields );
    wxString originalFKField, refFKField;
    originalFKField = m_constraint->GetLocalColumn();
    refFKField = m_constraint->GetRefColumn();
    found = false;
    for( SerializableList::iterator it = sourceFields.begin(); it != sourceFields.end() && !found; ++it )
    {
        if( dynamic_cast<FieldShape *>( (*it) )->GetField()->GetFieldName() == originalFKField )
        {
            m_sourceFld = dynamic_cast<FieldShape *>( (*it) );
            found = true;
        }
    }
    found = false;
    for( SerializableList::iterator it = targetFields.begin(); it != targetFields.end() && !found; ++it )
    {
        if( dynamic_cast<FieldShape *>( (*it) )->GetField()->GetFieldName() == refFKField )
        {
            m_targetFld = dynamic_cast<FieldShape *>( (*it) );
            found = true;
        }
    }
    RemoveStyle( sfsSHOW_HANDLES );
    m_isEnabled = true;
//    XS_SERIALIZE();
//    XS_SERIALIZE();
}

ErdLineShape::~ErdLineShape()
{
}

wxRealPoint ErdLineShape::GetModSrcPoint()
{
    wxString constraintColumnQuery;
//    bool found = false;
    wxSFShapeBase* pSrcShape = m_sourceFld;
    if( !pSrcShape ) return wxRealPoint();
    wxRealPoint nModPoint;
    double y;
    if( m_nTrgOffset != sfdvLINESHAPE_OFFSET )
    {
        wxRect bbRct = pSrcShape->GetBoundingBox();
        nModPoint = pSrcShape->GetAbsolutePosition();
        nModPoint.x += (double) bbRct.GetWidth() * m_nTrgOffset.x;
        nModPoint.y += (double) bbRct.GetHeight() * m_nTrgOffset.y;
    }
    else
    {
        wxRect shpBB = pSrcShape->GetBoundingBox();
//        if( m_type == DatabaseView )
//        {
            y = m_sourceFld->GetBoundingBox().GetHeight() / 2;
//        }
/*        double y = shpBB.GetTop();
        y += dynamic_cast<MyErdTable *>( pSrcShape )->GetLabel()->GetBoundingBox().GetHeight();
        SerializableList::compatibility_iterator node = dynamic_cast<MyErdTable *>( pSrcShape )->GetFieldGrid()->GetFirstChildNode();
        while( node && !found )
        {
            wxSFTextShape *pColumn = wxDynamicCast( node->GetData(), wxSFTextShape );
            if( pColumn )
            {
                wxString columnText = pColumn->GetText();
                if( m_type == QueryView )
                {
                    constraintColumnQuery = m_constraint->GetLocalColumn();
                    if( columnText != constraintColumnQuery )
                        y += pColumn->GetBoundingBox().GetHeight();
                    if( columnText == constraintColumnQuery )
                    {
                        found = true;
                        y += pColumn->GetBoundingBox().GetHeight() / 2;
                    }
                }
            }
            node = node->GetNext();
        }*/
        nModPoint = wxRealPoint( shpBB.GetLeft() + shpBB.GetWidth() / 2, y );
    }
    return nModPoint;
}

wxRealPoint ErdLineShape::GetModTrgPoint()
{
//    bool found = false;
    wxSFShapeBase* pTrgShape = m_targetFld;
    if( !pTrgShape ) return wxRealPoint();
    wxRealPoint nModPoint;
    double y;
    if( m_nTrgOffset != sfdvLINESHAPE_OFFSET )
    {
        wxRect bbRct = pTrgShape->GetBoundingBox();
        nModPoint = pTrgShape->GetAbsolutePosition();
        nModPoint.x += (double) bbRct.GetWidth() * m_nTrgOffset.x;
        nModPoint.y += (double) bbRct.GetHeight() * m_nTrgOffset.y;
    }
    else
    {
        wxRect shpBB = pTrgShape->GetBoundingBox();
//        if( m_type == DatabaseView )
//        {
            y = m_targetFld->GetBoundingBox().GetHeight() / 2;
//        }
/*        double y = shpBB.GetTop();
        y += dynamic_cast<MyErdTable *>( pTrgShape )->GetLabel()->GetBoundingBox().GetHeight();
        SerializableList::compatibility_iterator node = dynamic_cast<MyErdTable *>( pTrgShape )->GetFieldGrid()->GetFirstChildNode();
        while( node && !found )
        {
            wxSFTextShape *pColumn = wxDynamicCast( node->GetData(), wxSFTextShape );
            if( pColumn )
            {
                if( m_type == DatabaseView )
                {
                    wxString columnText = pColumn->GetText();
                    wxString refCol;
                    refCol = m_constraint->GetRefColumn();
                    if( columnText == refCol )
                    {
                        found = true;
                        y = pColumn->GetBoundingBox().GetHeight() / 2;
                    }
                }
            }
            node = node->GetNext();
        }*/
        nModPoint = wxRealPoint( shpBB.GetLeft() + shpBB.GetWidth() / 2, y );
    }
    return nModPoint;
}

wxRect ErdLineShape::GetBoundingBox()
{
    wxASSERT( m_pParentManager );
    wxRect lineRct( 0, 0, 0, 0 );
    // calculate control points area if they exist
    if( !m_lstPoints.IsEmpty() )
    {
        wxRealPoint prevPt = GetSourcePoint();
        wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
        while( node )
        {
            if( lineRct.IsEmpty() )
                lineRct = wxRect( Conv2Point( prevPt ), Conv2Point( *node->GetData() ) );
            else
                lineRct.Union( wxRect( Conv2Point( prevPt ), Conv2Point( *node->GetData() ) ) );
            prevPt = *node->GetData();
            node = node->GetNext();
        }
        lineRct.Union( wxRect( Conv2Point( prevPt ), Conv2Point( GetTrgPoint() ) ) );
    }
    else
    {
        wxRealPoint pt;
        // include starting point
        pt = GetSourcePoint();
        if( !lineRct.IsEmpty() )
        {
            lineRct.Union( wxRect( (int)pt.x, (int)pt.y, 1, 1 ) );
        }
        else
            lineRct = wxRect( (int)pt.x, (int)pt.y, 1, 1 );
        // include ending point
        pt = GetTargetPoint();
        if( !lineRct.IsEmpty() )
            lineRct.Union(wxRect((int)pt.x, (int)pt.y, 1, 1));
        else
            lineRct = wxRect((int)pt.x, (int)pt.y, 1, 1);
    }
    // include unfinished point if the line is under construction
    if( ( m_nMode == modeUNDERCONSTRUCTION ) || ( m_nMode == modeSRCCHANGE ) || ( m_nMode == modeTRGCHANGE ) )
    {
        if( !lineRct.IsEmpty() )
            lineRct.Union( wxRect( m_nUnfinishedPoint.x, m_nUnfinishedPoint.y, 1, 1 ) );
        else
            lineRct = wxRect(m_nUnfinishedPoint.x, m_nUnfinishedPoint.y, 1, 1);
    }
    return lineRct;
}

wxRealPoint ErdLineShape::GetSourcePoint()
{
    if( m_fStandAlone )
    {
        return m_nSrcPoint;
    }
    else
    {
        wxRealPoint pt1, pt2;
        wxSFShapeBase* pSrcShape = m_sourceFld;
        if( pSrcShape && !m_lstPoints.IsEmpty() )
        {
            if( pSrcShape->GetConnectionPoints().IsEmpty() )
            {
                wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetFirst();
                if( node )
                {	
                    pt1 = *node->GetData();
                    return pSrcShape->GetBorderPoint( GetModSrcPoint(), pt1 );
                }
            }
            else
                return GetModSrcPoint();
        }
        else
        {
            if( m_nMode != modeUNDERCONSTRUCTION )
                GetDirectionalLine( pt1, pt2 );
            else
                pt1 = GetModSrcPoint();
            return pt1;
        }
        return wxRealPoint();
    }
}

wxRealPoint ErdLineShape::GetTargetPoint()
{
    if( m_fStandAlone )
    {
        return m_nTrgPoint;
    }
    else
    {
        wxRealPoint pt1, pt2;
        wxSFShapeBase* pTrgShape = m_targetFld;
        if( pTrgShape && !m_lstPoints.IsEmpty() )
        {
            if( pTrgShape->GetConnectionPoints().IsEmpty() )
            {
                wxXS::RealPointList::compatibility_iterator node = m_lstPoints.GetLast();
                if( node )
                {
                    pt2 = *node->GetData();
                    return pTrgShape->GetBorderPoint( GetModTrgPoint(), pt2 );
                }
            }
            else
               return GetModTrgPoint();
        }
        else
        {
            if( m_nMode != modeUNDERCONSTRUCTION )
                GetDirectionalLine( pt1, pt2 );
            else
                pt2 = Conv2RealPoint( m_nUnfinishedPoint );
            return pt2;
        }
        return wxRealPoint();
    }
}

void ErdLineShape::GetDirectionalLine(wxRealPoint& src, wxRealPoint& trg)
{
    if( m_fStandAlone )
    {
        src = m_nSrcPoint;
        trg = m_nTrgPoint;
    }
    else
    {
        wxSFShapeBase* pSrcShape = m_sourceFld;
        wxSFShapeBase* pTrgShape = m_targetFld;
        if( pSrcShape && pTrgShape )
        {
            wxRealPoint trgCenter = GetModTrgPoint();
            wxRealPoint srcCenter = GetModSrcPoint();
            wxRect trgParent = pTrgShape->GetParentShape()->GetBoundingBox();
            wxRect srcParent = pSrcShape->GetParentShape()->GetBoundingBox();
            wxRect srcRect = pSrcShape->GetBoundingBox(), trgRect = pTrgShape->GetBoundingBox();
            if( trgCenter.x < srcCenter.x )
            {
                trgCenter.x = trgParent.GetLeft() + trgParent.GetWidth();
                srcCenter.x = srcParent.GetLeft();
            }
            if( trgCenter.x > srcCenter.x )
            {
                trgCenter.x = trgParent.GetLeft();
                srcCenter.x = srcParent.GetLeft() + srcParent.GetWidth();
            }
            if( ( pSrcShape->GetParent() == pTrgShape ) || ( pTrgShape->GetParent() == pSrcShape ) )
            {
                wxRect trgBB = pTrgShape->GetBoundingBox();
                wxRect srcBB = pSrcShape->GetBoundingBox();
                if( trgBB.Contains( (int)srcCenter.x, (int)srcCenter.y ) )
                {
                    if( srcCenter.y > trgCenter.y )
                    {
                        src = wxRealPoint( srcCenter.x, srcBB.GetBottom() );
                        trg = wxRealPoint( srcCenter.x, trgBB.GetBottom() );
                    }
                    else
                    {
                        src = wxRealPoint(srcCenter.x, srcBB.GetTop());
                        trg = wxRealPoint(srcCenter.x, trgBB.GetTop());
                    }
                    return;
                }
                else if( srcBB.Contains( (int)trgCenter.x, (int)trgCenter.y) )
                {
                    if( trgCenter.y > srcCenter.y )
                    {
                        src = wxRealPoint( trgCenter.x, srcBB.GetBottom() );
                        trg = wxRealPoint( trgCenter.x, trgBB.GetBottom() );
                    }
                    else
                    {
                        src = wxRealPoint( trgCenter.x, srcBB.GetTop() );
                        trg = wxRealPoint( trgCenter.x, trgBB.GetTop() );
                    }
                    return;
                }
            }			
            if( pSrcShape->GetConnectionPoints().IsEmpty() )
            {
                src = wxRealPoint( srcCenter.x, ( ( srcRect.GetBottom() - srcRect.GetTop() ) / 2 ) + srcRect.GetTop() );
            }
            else
                src = srcCenter;
            if( pTrgShape->GetConnectionPoints().IsEmpty() )
            {
                trg = wxRealPoint( trgCenter.x, ( ( trgRect.GetBottom() - trgRect.GetTop() ) / 2 ) + trgRect.GetTop() );
            }
            else
                trg = trgCenter;
        }
    }
}

void ErdLineShape::DrawNormal(wxDC& dc)
{
    if( m_isEnabled )
    {
        dc.SetPen( m_Pen );
        DrawCompleteLine( dc );
        dc.SetPen( wxNullPen );
    }
}

void ErdLineShape::DrawCompleteLine(wxDC& dc)
{
    if( !m_pParentManager ) return;
    size_t i;
    wxRealPoint src, trg;
    switch( m_nMode )
    {
        case modeREADY:
        {
            // draw basic line parts
            for( i = 0; i <= m_lstPoints.GetCount(); i++ )
            {
                GetLineSegment( i, src, trg );
                dc.DrawLine( Conv2Point( src ), Conv2Point( trg ) );
            }
            // draw target arrow
            if( m_pTrgArrow ) m_pTrgArrow->Draw( src, trg, dc );
            // draw source arrow
            if( m_pSrcArrow )
            {
                GetLineSegment( 0, src, trg );
                m_pSrcArrow->Draw( trg, src, dc );
            }
        }
        break;
        case modeUNDERCONSTRUCTION:
        {
            // draw basic line parts
            for( i = 0; i < m_lstPoints.GetCount(); i++ )
            {
                GetLineSegment( i, src, trg );
                dc.DrawLine( Conv2Point( src ), Conv2Point( trg ) );
            }
            // draw unfinished line segment if any (for interactive line creation)
            dc.SetPen( wxPen( *wxBLACK, 1, wxPENSTYLE_DOT ) );
            if( i )
            {
                dc.DrawLine( Conv2Point( trg ), m_nUnfinishedPoint );
            }
            else
            {
                wxSFShapeBase* pSrcShape = m_sourceFld;
                if( pSrcShape )
                {
                    if( pSrcShape->GetConnectionPoints().IsEmpty() )
                        dc.DrawLine( Conv2Point(pSrcShape->GetBorderPoint(pSrcShape->GetCenter(), Conv2RealPoint(m_nUnfinishedPoint))), m_nUnfinishedPoint );
                    else
                        dc.DrawLine( Conv2Point( GetModSrcPoint() ), m_nUnfinishedPoint );
                }
            }
            dc.SetPen(wxNullPen);
        }
        break;
        case modeSRCCHANGE:
        {
            // draw basic line parts
            for( i = 1; i <= m_lstPoints.GetCount(); i++ )
            {
                GetLineSegment( i, src, trg );
                dc.DrawLine( Conv2Point( src ), Conv2Point( trg ) );
            }
            // draw linesegment being updated
            GetLineSegment( 0, src, trg );
            if( !m_fStandAlone ) dc.SetPen( wxPen( *wxBLACK, 1, wxPENSTYLE_DOT ) );
            dc.DrawLine( m_nUnfinishedPoint, Conv2Point( trg ) );
            dc.SetPen( wxNullPen );
        }
        break;
        case modeTRGCHANGE:
        {
            // draw basic line parts
            if( !m_lstPoints.IsEmpty() )
            {
                for( i = 0; i < m_lstPoints.GetCount(); i++ )
                {
                    GetLineSegment( i, src, trg );
                    dc.DrawLine( Conv2Point( src ), Conv2Point( trg ) );
                }
            }
            else
                trg = GetSrcPoint();
            // draw linesegment being updated
            if( !m_fStandAlone ) dc.SetPen( wxPen( *wxBLACK, 1, wxPENSTYLE_DOT ) );
            dc.DrawLine( Conv2Point( trg ), m_nUnfinishedPoint );
            dc.SetPen( wxNullPen );
        }
        break;
    }
}

bool ErdLineShape::GetLineSegment(size_t index, wxRealPoint& src, wxRealPoint& trg)
{
    if( !m_lstPoints.IsEmpty() )
    {
        if( index == 0 )
        {
            src = GetSrcPoint();
            trg = *m_lstPoints.GetFirst()->GetData();
            return true;
        }
        else if( index == m_lstPoints.GetCount() )
        {
            src = *m_lstPoints.GetLast()->GetData();
            trg = GetTrgPoint();
            return true;
        }
        else if( (index > 0) && (index < m_lstPoints.GetCount()) )
        {
            wxXS::RealPointList::compatibility_iterator node = m_lstPoints.Item(index);	
            src = *node->GetPrevious()->GetData();
            trg = *node->GetData();
            return true;
        }
        return false;
    }
    else
    {
        if( index == 0 )
        {
            GetDirectionalLine( src, trg );
            return true;
        }
        else
            return false;
    }
}

void ErdLineShape::EnableDisableFK(bool enable)
{
    m_isEnabled = enable;
}

void ErdLineShape::DrawSelected(wxDC &dc)
{
    DrawNormal( dc );
}

void ErdLineShape::OnLeftDoubleClick (const wxPoint& WXUNUSED(pos))
{
}
