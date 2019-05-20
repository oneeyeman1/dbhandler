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

#include "wxsf/TextShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/GridShape.h"
#include "wxsf/RectShape.h"
#include "divider.h"

Divider::Divider() : wxSFRectShape()
{
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    if( m_grid )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_grid->Activate( false );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
        m_text = new wxSFTextShape;
        if( m_text )
        {
            m_text->SetHAlign( wxSFShapeBase::halignCENTER );
            m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
            auto font = m_text->GetFont();
            font.SetWeight( wxFONTWEIGHT_BOLD );
            m_text->SetFont( font );
            m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
            if( m_grid->AppendToGrid( m_text ) )
                m_text->SetText( "" );
            else
                delete m_text;
        }
    }
}

Divider::Divider(const wxString &text) : wxSFRectShape()
{
    wxString upArrow( L"\x2191" );
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    m_text = new wxSFTextShape;
    m_arrow = new wxSFTextShape;
    if( m_grid && m_text && m_arrow )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_grid->SetDimensions( 1, 2 );
        m_grid->SetCellSpace( 2 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( false );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
        m_text->SetHAlign( wxSFShapeBase::halignLEFT );
        m_text->SetId( 1000 );
        m_arrow->SetHAlign( wxSFShapeBase::halignLEFT );
        m_arrow->SetId( 1001 );
        m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
        m_arrow->SetVAlign( wxSFShapeBase::valignMIDDLE );
        auto font = m_text->GetFont();
        font.SetWeight( wxFONTWEIGHT_BOLD );
        m_text->SetFont( font );
        m_arrow->SetFont( font );
        m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_arrow->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        if( m_grid->AppendToGrid( m_text ) )
        {
            m_text->SetText( text );
            if( m_grid->AppendToGrid( m_arrow ) )
                m_arrow->SetText( upArrow );
            else
                delete m_arrow;
        }
        else
            delete m_text;
    }
    SetRectSize( 1000, -1 );
    this->SetUserData( m_text );
}

Divider::~Divider()
{
}

void Divider::DrawNormal (wxDC &dc)
{
    wxRect rect = GetBoundingBox();
    dc.SetBrush( *wxGREY_BRUSH );
    dc.DrawRectangle( 1, rect.y, 5000, rect.y );
}

void Divider::DrawSelected(wxDC &dc)
{
    DrawNormal( dc );
}

void Divider::DrawHover(wxDC &dc)
{
    DrawNormal( dc );
}