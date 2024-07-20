#include "wxsf/RectShape.h"
#include "wxsf/GridShape.h"
#include "wxsf/TextShape.h"
#include "taborder.h"

TabOrder::TabOrder() : wxSFRectShape()
{
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    m_text = new wxSFTextShape;
    if( m_grid )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxRED_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( false );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
    }
}

TabOrder::~TabOrder()
{
}
