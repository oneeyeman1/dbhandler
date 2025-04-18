/***************************************************************
 * Name:      OrthoShape.cpp
 * Purpose:   Implements orthogonal line shape class
 * Author:    Michal Bližňák (michal.bliznak@gmail.com)
 * Created:   2009-04-26
 * Copyright: Michal Bližňák
 * License:   wxWidgets license (www.wxwidgets.org)
 * Notes:
 **************************************************************/

#include "wx_pch.h"

#ifdef _DEBUG_MSVC
#define new DEBUG_NEW
#endif

#include <math.h>

#include "wxsf/OrthoShape.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/CommonFcn.h"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(wxSFOrthoLineShape, wxSFLineShape);

wxSFOrthoLineShape::wxSFOrthoLineShape() : wxSFLineShape()
{
}

wxSFOrthoLineShape::wxSFOrthoLineShape(long src, long trg, const wxXS::RealPointList& path, wxSFDiagramManager* manager)
: wxSFLineShape(src, trg, path, manager)
{
}

wxSFOrthoLineShape::wxSFOrthoLineShape(const wxSFOrthoLineShape& obj)
: wxSFLineShape(obj)
{
}

wxSFOrthoLineShape::~wxSFOrthoLineShape()
{

}

//----------------------------------------------------------------------------------//
// protected virtual functions
//----------------------------------------------------------------------------------//

void wxSFOrthoLineShape::DrawCompleteLine(wxDC& dc)
{
    if(!m_pParentManager)return;

    size_t i;
	wxRealPoint src, trg;
	
	wxSFConnectionPoint *cpSrc = NULL, *cpTrg = NULL;
	
	wxSFShapeBase* pShape = GetShapeManager()->FindShape(m_nSrcShapeId);
	if( pShape ) {
		cpSrc = pShape->GetNearestConnectionPoint( GetModSrcPoint() ); 
	}
	pShape = GetShapeManager()->FindShape(m_nTrgShapeId);
	if( pShape ) {
		cpTrg = pShape->GetNearestConnectionPoint( GetModTrgPoint() ); 
	}

    switch(m_nMode)
    {

    case modeREADY:
        {
            // draw basic line parts
            for(i = 0; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				this->DrawLineSegment( dc, src, trg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
			}

            // draw target arrow
            if(m_pTrgArrow)
			{
				wxRealPoint asrc, atrg;
				GetLastSubsegment( src, trg, asrc, atrg, GetUsedConnectionPoints( cpSrc, cpTrg, i - 1 ) );
				m_pTrgArrow->Draw( asrc, atrg, dc );
			}
			
            // draw source arrow
			if(m_pSrcArrow)
			{
				wxRealPoint asrc, atrg;
				GetLineSegment( 0, src, trg );
				GetFirstSubsegment( src, trg, asrc, atrg, GetUsedConnectionPoints( cpSrc, cpTrg, 0 ) );
				m_pSrcArrow->Draw( atrg, asrc, dc );
			}
        }
        break;

    case modeUNDERCONSTRUCTION:
        {
            // draw basic line parts
            for(i = 0; i < m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				this->DrawLineSegment( dc, src, trg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
			}

            // draw unfinished line segment if any (for interactive line creation)
            dc.SetPen( wxPen(*wxBLACK, 1, wxPENSTYLE_DOT) );
			
			if( i )
			{
				this->DrawLineSegment( dc, trg, Conv2RealPoint(m_nUnfinishedPoint), GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
			}
			else
			{
				wxSFShapeBase* pSrcShape = GetShapeManager()->FindShape(m_nSrcShapeId);
				if( pSrcShape )
				{
					if( pSrcShape->GetConnectionPoints().IsEmpty() )
					{
						this->DrawLineSegment( dc, pSrcShape->GetBorderPoint(pSrcShape->GetCenter(), Conv2RealPoint(m_nUnfinishedPoint)), Conv2RealPoint(m_nUnfinishedPoint), GetUsedConnectionPoints( cpSrc, cpTrg, 0 ) );
					}
					else
						this->DrawLineSegment( dc, GetModSrcPoint(), Conv2RealPoint(m_nUnfinishedPoint), GetUsedConnectionPoints( cpSrc, cpTrg, 0 ) );
				}
			}
				
            dc.SetPen(wxNullPen);
        }
        break;

    case modeSRCCHANGE:
        {
            // draw basic line parts
            for(i = 1; i <= m_lstPoints.GetCount(); i++)
			{
				GetLineSegment( i, src, trg );
				this->DrawLineSegment( dc, src, trg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
			}

            // draw linesegment being updated
			GetLineSegment( 0, src, trg );
			
            dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
			this->DrawLineSegment( dc, Conv2RealPoint(m_nUnfinishedPoint), trg, GetUsedConnectionPoints( cpSrc, cpTrg, 0 ) );
            dc.SetPen(wxNullPen);
        }
        break;

    case modeTRGCHANGE:
        {
            // draw basic line parts
			if( !m_lstPoints.IsEmpty() )
			{
				for(i = 0; i < m_lstPoints.GetCount(); i++)
				{
					GetLineSegment( i, src, trg );
					this->DrawLineSegment( dc, src, trg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
				}
			}
			else
				trg = GetSrcPoint();
			
            // draw linesegment being updated
            dc.SetPen(wxPen(*wxBLACK, 1, wxPENSTYLE_DOT));
			this->DrawLineSegment( dc, trg, Conv2RealPoint(m_nUnfinishedPoint), GetUsedConnectionPoints( cpSrc, cpTrg, m_lstPoints.GetCount() ) );
            dc.SetPen(wxNullPen);
        }
        break;

    }
}

int wxSFOrthoLineShape::GetHitLinesegment(const wxPoint& pos)
{
    if( !GetBoundingBox().Inflate(5, 5).Contains(pos) )return -1;

    wxRealPoint ptSrc, ptTrg, ptSSrc, ptSTrg;
    wxRect rctBB;
	
	wxSFConnectionPoint *cpSrc = NULL, *cpTrg = NULL;
	
	wxSFShapeBase* pShape = GetShapeManager()->FindShape(m_nSrcShapeId);
	if( pShape ) {
		cpSrc = pShape->GetNearestConnectionPoint( GetModSrcPoint() ); 
	}
	pShape = GetShapeManager()->FindShape(m_nTrgShapeId);
	if( pShape ) {
		cpTrg = pShape->GetNearestConnectionPoint( GetModTrgPoint() ); 
	}


    // Get all polyline segments
	for(size_t i = 0; i <= m_lstPoints.GetCount(); i++)
	{
		GetLineSegment( i, ptSrc, ptTrg );
		
		// test first subsegment
		GetFirstSubsegment( ptSrc, ptTrg, ptSSrc, ptSTrg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
		rctBB = wxRect(Conv2Point(ptSSrc), Conv2Point(ptSTrg));
		rctBB.Inflate(5);
		
		if( rctBB.Contains(pos) ) return (int)i;
		
		// test middle subsegment
		GetMiddleSubsegment( ptSrc, ptTrg, ptSSrc, ptSTrg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
		rctBB = wxRect(Conv2Point(ptSSrc), Conv2Point(ptSTrg));
		rctBB.Inflate(5);
		
		if( rctBB.Contains(pos) ) return (int)i;
		
		// test last subsegment
		GetLastSubsegment( ptSrc, ptTrg, ptSSrc, ptSTrg, GetUsedConnectionPoints( cpSrc, cpTrg, i ) );
		rctBB = wxRect(Conv2Point(ptSSrc), Conv2Point(ptSTrg));
		rctBB.Inflate(5);
		
		if( rctBB.Contains(pos) ) return (int)i;
	}

    return -1;	
}

//----------------------------------------------------------------------------------//
// protected functions
//----------------------------------------------------------------------------------//

void wxSFOrthoLineShape::DrawLineSegment(wxDC& dc, const wxRealPoint& src, const wxRealPoint& trg, const SEGMENTCPS& cps )
{
	double nDirection = 0;
	
	if( (trg.x == src.x) || ( trg.y == src.y ) )
	{
		dc.DrawLine( src.x, src.y, trg.x, trg.y );
		return;
	}
	else
		//nDirection = fabs( trg.y - src.y ) / fabs( trg.x - src.x );
		nDirection = GetSegmentDirection( src, trg, cps );
	
	if( IsTwoSegment( cps ) ) {
	
		if( nDirection < 1 ) {
			dc.DrawLine( src.x, src.y, trg.x, src.y );
			dc.DrawLine( trg.x, src.y, trg.x, trg.y );
			
		} else {
			dc.DrawLine( src.x, src.y, src.x, trg.y );
			dc.DrawLine( src.x, trg.y, trg.x, trg.y );
		}
		
	} else {
		wxRealPoint ptCenter( (src.x + trg.x)/2, (src.y + trg.y)/2 );
		
		if( nDirection < 1 ) {
			dc.DrawLine( src.x, src.y, ptCenter.x, src.y );
			dc.DrawLine( ptCenter.x, src.y, ptCenter.x, trg.y );
			dc.DrawLine( ptCenter.x, trg.y, trg.x, trg.y );
			
		} else {
			dc.DrawLine( src.x, src.y, src.x, ptCenter.y );
			dc.DrawLine( src.x, ptCenter.y, trg.x, ptCenter.y );
			dc.DrawLine( trg.x, ptCenter.y, trg.x, trg.y );
		}
	}
}

void wxSFOrthoLineShape::GetFirstSubsegment(const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps )
{
	double nDirection = GetSegmentDirection( src, trg, cps );
	
	if( IsTwoSegment( cps ) ) {
		if( nDirection < 1 )
		{
			subsrc = src;
			subtrg = wxRealPoint( trg.x, src.y );
		}
		else
		{
			subsrc = src;
			subtrg = wxRealPoint( src.x, trg.y );
		}		
	} else {
		wxRealPoint ptCenter( (src.x + trg.x)/2, (src.y + trg.y)/2 );
		if( nDirection < 1 )
		{
			subsrc = src;
			subtrg = wxRealPoint( ptCenter.x, src.y );
		}
		else
		{
			subsrc = src;
			subtrg = wxRealPoint( src.x, ptCenter.y );
		}
	}
}

void wxSFOrthoLineShape::GetLastSubsegment(const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps )
{
	double nDirection = GetSegmentDirection( src, trg, cps);
	
	if( IsTwoSegment( cps ) ) {
		if( nDirection < 1 )
		{
			subsrc = wxRealPoint( trg.x, src.y);
			subtrg = trg;
		}
		else
		{
			subsrc = wxRealPoint( src.x, trg.y );
			subtrg = trg;
		}
	} else {
		wxRealPoint ptCenter( (src.x + trg.x)/2, (src.y + trg.y)/2 );
		if( nDirection < 1 )
		{
			subsrc = wxRealPoint( ptCenter.x, trg.y );
			subtrg = trg;
		}
		else
		{
			subsrc = wxRealPoint( trg.x, ptCenter.y );
			subtrg = trg;
		}
	}
}

void wxSFOrthoLineShape::GetMiddleSubsegment(const wxRealPoint& src, const wxRealPoint& trg, wxRealPoint& subsrc, wxRealPoint& subtrg, const SEGMENTCPS& cps )
{
	double nDirection = GetSegmentDirection( src, trg, cps );

	if( IsTwoSegment( cps ) ) {
		if( nDirection < 1 )
		{
			subsrc = src;
			subtrg = wxRealPoint( trg.x, src.y );
		}
		else
		{
			subsrc = src;
			subtrg = wxRealPoint( src.x, trg.y );
		}			
	} else {
		wxRealPoint ptCenter( (src.x + trg.x)/2, (src.y + trg.y)/2 );
		if( nDirection < 1 )
		{
			subsrc = wxRealPoint( ptCenter.x, src.y);
			subtrg = wxRealPoint( ptCenter.x, trg.y );
		}
		else
		{
			subsrc = wxRealPoint( src.x, ptCenter.y );
			subtrg = wxRealPoint( trg.x, ptCenter.y );
		}
	}
}

double wxSFOrthoLineShape::GetSegmentDirection(const wxRealPoint& src, const wxRealPoint& trg, const SEGMENTCPS& cps )
{
	double nDirection = 0;
	
	if( trg.x == src.x )
		nDirection = 1;
		
	else {
		nDirection =  fabs( trg.y - src.y ) / fabs( trg.x - src.x );
		
		const wxSFConnectionPoint *cp = NULL;
		if( cps.cpSrc && ! cps.cpTrg ) cp = cps.cpSrc;
		else if( ! cps.cpSrc && cps.cpTrg ) cp = cps.cpTrg;
		else if( cps.cpSrc && cps.cpTrg ) cp = cps.cpSrc;
	
		if( cp ) {
			switch( cp->GetOrthoDirection() ) {
				case wxSFConnectionPoint::cpdVERTICAL:
					nDirection = 1;
					break;
					
				case wxSFConnectionPoint::cpdHORIZONTAL:
					nDirection = 0;
					break;
					
				default:
					break;
			}
		}
	}
	
	return nDirection;
}

wxSFOrthoLineShape::SEGMENTCPS wxSFOrthoLineShape::GetUsedConnectionPoints(const wxSFConnectionPoint* src, const wxSFConnectionPoint* trg, size_t i) const
{
	SEGMENTCPS cps = {NULL, NULL};
	
	if( m_lstPoints.IsEmpty() ) {
		cps.cpSrc = src;
		cps.cpTrg = trg;
	} else if( i == 0 ) {
		cps.cpSrc = src;
		cps.cpTrg = NULL;
	} else if( i == m_lstPoints.GetCount() ) {
		cps.cpSrc = NULL;
		cps.cpTrg = trg;
	}
	
	return cps;
}

bool wxSFOrthoLineShape::IsTwoSegment(const SEGMENTCPS& cps)
{
	return cps.cpSrc && cps.cpTrg && ( cps.cpSrc->GetOrthoDirection() != cps.cpTrg->GetOrthoDirection() );
}
