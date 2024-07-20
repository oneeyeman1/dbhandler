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
#include "wxsf/RectShape.h"
#include "wxsf/GridShape.h"
#include "dbview.h"
#include "taborder.h"
#include "designfield.h"

XS_IMPLEMENT_CLONABLE_CLASS(DesignField, wxSFRectShape);

DesignField::DesignField() : wxSFRectShape()
{
    SetHAlign( wxSFShapeBase::halignCENTER );
    SetVAlign( wxSFShapeBase::valignMIDDLE );
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSIZE_CHANGE );
    AcceptChild( "GridShape" );
    AcceptChild( "TabOrder" );
	m_text = new wxSFTextShape;
    m_grid = new wxSFGridShape;
    m_tabOrder = new TabOrder;
    if( m_grid )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN | sfsSHOW_HANDLES );
        m_grid->SetDimensions( 0, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( true );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
    }
    m_label = "";
}

DesignField::DesignField(const wxFont font, const wxString &label, int alignment) : wxSFRectShape()
{
    m_properties.m_font = font;
    m_label = label;
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSIZE_CHANGE );
    AcceptChild( "GridShape" );
	m_text = new wxSFTextShape;
    m_grid = new wxSFGridShape;
    if( m_grid && m_text )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN | sfsSHOW_HANDLES );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( true );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
        if( m_grid->InsertToGrid( 0, 0, m_text ) )
        {
            m_text->SetHAlign( wxSFShapeBase::halignCENTER );
            m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
            m_text->SetFont( font );
            m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
            m_text->SetText( m_label );
        }
        else
            delete m_text;
    }
    m_grid->RemoveChildren();
    m_grid->ClearGrid();
    m_grid->SetDimensions( 1, 1 );
    Refresh();
    SetRectSize( GetRectSize().x, 0 );
    m_text = new wxSFTextShape;
    if( m_text )
    {
        m_text->SetHAlign( wxSFShapeBase::halignCENTER );
        m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
        m_text->SetFont( font );
        m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        if( m_grid->InsertToGrid( 0, 0, m_text ) )
            m_text->SetText( m_label );
        else
            delete m_text;
    }
    m_grid->Update();
    Update();
}

DesignField::~DesignField()
{
}

DesignFieldProperties DesignField::GetProperties()
{
    return m_properties;
}
