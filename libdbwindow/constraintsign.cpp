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
#include "database.h"
#include "constraint.h"
#include "constraintsign.h"
#include "res/gui/key-f.xpm"

XS_IMPLEMENT_CLONABLE_CLASS(ConstraintSign, wxSFRectShape);

ConstraintSign::ConstraintSign() : wxSFRectShape()
{
    m_joinType = 0;
}

ConstraintSign::ConstraintSign(ViewType type, int joinType) : wxSFRectShape()
{
    m_type = type;
    m_sign = NULL;
    m_fKey = NULL;
    m_joinType = joinType;
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
        if( type == QueryView )
        {
            m_sign = new wxSFTextShape;
            if( m_sign )
            {
                m_sign->SetId( 1000 );
                m_sign->SetVAlign( wxSFShapeBase::valignMIDDLE );
                m_sign->SetHAlign( wxSFShapeBase::halignCENTER );
                m_sign->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
                if( m_grid->InsertToGrid( 1, 0, m_sign ) )
                    m_sign->SetText( "=" );
                else
                    delete m_sign;
            }
        }
    }
}

ConstraintSign::~ConstraintSign()
{
    delete m_constraint;
    m_constraint = NULL;
}

void ConstraintSign::initSerializable()
{
    XS_SERIALIZE( m_joinType, "m_joinType" );
}

void ConstraintSign::SetSign(const wxString &sign)
{
    m_sign->SetText( sign );
}

const wxString &ConstraintSign::GetSign()
{
    return m_sign->GetText();
}

void ConstraintSign::DrawSelected(wxDC &dc)
{
//    dc.SetBackground( *wxBLACK_BRUSH );
    DrawNormal( dc );
}

void ConstraintSign::DrawNormal(wxDC &dc)
{
    if( this->m_fSelected )
    {
        if( m_sign )
            m_sign->SetTextColour( *wxWHITE );
        SetFill( *wxBLACK_BRUSH );
    }
    else
    {
        if( m_sign )
            m_sign->SetTextColour( *wxBLACK );
        SetFill( *wxWHITE_BRUSH );
    }
    wxSFRectShape::DrawNormal( dc );
}


void ConstraintSign::SetConstraint(const Constraint *constraint)
{
    m_constraint = const_cast<Constraint *>( constraint );
}

Constraint *ConstraintSign::GetConstraint() const
{
    return m_constraint;
}

void ConstraintSign::DeleteConstraint()
{
    delete m_constraint;
    m_constraint = NULL;
}
