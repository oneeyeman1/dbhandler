/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wxsf/RectShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/BitmapShape.h"
#include "constraint.h"
#include "constraintsign.h"
#include "res/gui/key-f.xpm"

ConstraintSign::ConstraintSign(ViewType type)
{
    m_type = type;
    m_sign = NULL;
    m_fKey = NULL;
    m_nRectSize = wxRealPoint( 20, 20 );
//    SetRectSize( 20, 20 );
    AcceptChild( "GridShape" );
    m_grid = new wxSFFlexGridShape;
    if( m_grid )
    {
        m_grid->SetRectSize( 20, 20 );
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetVAlign( wxSFShapeBase::valignTOP );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN);
        m_grid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_grid->AcceptChild( "wxSFBitmapShape" );
        m_grid->AcceptChild( "wxSFTextShape" );
        SF_ADD_COMPONENT( m_grid, wxT( "main_grid" ) );
        if( type == DatabaseView )
        {
            m_fKey = new wxSFBitmapShape;
            if( m_fKey )
            {
                m_fKey->SetId( 1000 );
                m_fKey->SetVAlign( wxSFShapeBase::valignMIDDLE );
                m_fKey->SetHAlign( wxSFShapeBase::halignCENTER );
                m_fKey->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
				if( m_grid->InsertToGrid( 1, 0, m_fKey ) )
                    m_fKey->CreateFromXPM( key_f_xpm );
                else
                    delete m_fKey;
            }
        }
    }
}

ConstraintSign::~ConstraintSign()
{
}

void ConstraintSign::SetSign(const wxString &sign)
{
    m_sign->SetText( sign );
}

const wxString &ConstraintSign::GetSign()
{
    return m_sign->GetText();
}
