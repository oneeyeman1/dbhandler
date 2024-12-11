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
#include "wxsf/DiagramManager.h"
#include "wx/image.h"
#include "database.h"
#include "guiobjectsproperties.h"
#include "propertieshandlerbase.h"
#include "divider.h"

XS_IMPLEMENT_CLONABLE_CLASS(Divider, wxSFRectShape);

Divider::Divider() : wxSFRectShape()
{
    m_object = DividerPropertiesType;
    m_Fill = wxBrush( *wxGREY_BRUSH );
    BandProperties props = m_any.As<BandProperties>();
    props.m_general.m_color = "Transparent";
    props.m_general.m_height = 200;
    props.m_cursorFile = wxEmptyString;
    props.m_stockCursor = -1;
    props.m_general.m_autosize = false;
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    if( m_grid )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
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
            font.SetFaceName( "SmallFont" );
            font.SetPointSize( 8 );
            m_text->SetFont( font );
            m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
            if( m_grid->AppendToGrid( m_text ) )
                m_text->SetText( "" );
            else
                delete m_text;
        }
    }
    XS_SERIALIZE( props.m_general.m_color, "color" );
    XS_SERIALIZE( props.m_cursorFile, "cursor-file" );
    XS_SERIALIZE( props.m_general.m_height, "height" );
    XS_SERIALIZE( props.m_stockCursor, "stock-cursor" );
    XS_SERIALIZE_BOOL( props.m_general.m_autosize, "auto-size" );
}

Divider::Divider(const wxString &text, const wxString &cursorFile, int stockCursor, wxSFDiagramManager *manager) : wxSFRectShape( wxRealPoint( 1, 1 ), wxRealPoint( 5000, -1 ), manager )
{
    BandProperties props = m_any.As<BandProperties>();
    m_object = DividerPropertiesType;
    props.m_type = text;
    props.m_general.m_color = "Transparent";
    props.m_general.m_height = 200;
    props.m_cursorFile = cursorFile;
    props.m_stockCursor = stockCursor;
    props.m_general.m_autosize = false;
    wxString upArrow( L"\x2191" );
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    m_text = new wxSFTextShape;
    m_arrow = new wxSFTextShape;
    if( m_grid && m_text && m_arrow )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
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
        font.SetFaceName( "SmallFont" );
        font.SetPointSize( 8 );
        m_text->SetFont( font );
        m_arrow->SetFont( font );
        m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_arrow->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
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
    m_grid->ClearGrid();
    m_grid->RemoveChildren();
    m_grid->SetDimensions( 1, 2 );
    Refresh();
    m_text = new wxSFTextShape;
    m_arrow = new wxSFTextShape;
    m_text->SetHAlign( wxSFShapeBase::halignLEFT );
    m_text->SetId( 1000 );
    m_arrow->SetHAlign( wxSFShapeBase::halignLEFT );
    m_arrow->SetId( 1001 );
    m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
    m_arrow->SetVAlign( wxSFShapeBase::valignMIDDLE );
    auto font = m_text->GetFont();
    font.SetWeight( wxFONTWEIGHT_BOLD );
    font.SetFaceName( "SmallFont" );
    font.SetPointSize( 8 );
    m_text->SetFont( font );
    m_arrow->SetFont( font );
    m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
    m_arrow->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
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
    m_arrow->RemoveStyle( sfsSHOW_HANDLES );
    m_text->RemoveStyle( sfsSHOW_HANDLES );
    RemoveStyle( sfsSHOW_HANDLES );
    m_grid->Update();
    Update();
    m_nRectSize.y = GetBoundingBox().GetHeight();
    m_nRectSize.x = 5000;
    m_Fill = *wxGREY_BRUSH;
    XS_SERIALIZE( props.m_general.m_color, "color" );
    XS_SERIALIZE( props.m_cursorFile, "cursor-file" );
    XS_SERIALIZE( props.m_general.m_height, "height" );
    XS_SERIALIZE( props.m_stockCursor, "stock-cursor" );
    XS_SERIALIZE_BOOL( props.m_general.m_autosize, "auto-size" );
}

Divider::~Divider()
{
}

void Divider::DrawNormal(wxDC &dc)
{
    wxRect rect = GetBoundingBox();
    dc.SetBrush( *wxGREY_BRUSH );
    dc.DrawRectangle( 1, rect.y, 5000, m_nRectSize.y );
    dc.SetBrush( wxNullBrush );
}

void Divider::DrawSelected(wxDC &dc)
{
    DrawNormal( dc );
}

void Divider::DrawHover(wxDC &dc)
{
    DrawNormal( dc );
}

wxRect Divider::GetBoundingBox()
{
    wxRect rect = wxSFRectShape::GetBoundingBox();
    rect.SetWidth( 5000 );
    return rect;
}

void Divider::OnDragging(const wxPoint& pos)
{
    MoveTo( 1, pos.y );
}

const wxString &Divider::GetDividerType() const
{
    return GetDividerProperties().m_type;
}

BandProperties Divider::GetDividerProperties() const
{
    return m_any.As<BandProperties>();
}

int Divider::ApplyProperties(const wxAny &any, bool logOnly)
{
    return 0;
}

wxAny &Divider::GetProperties()
{
    return m_any;
}
