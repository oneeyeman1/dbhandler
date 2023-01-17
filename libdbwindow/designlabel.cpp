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
#include "XmlSerializer.h"
#include "designlabel.h"

XS_IMPLEMENT_CLONABLE_CLASS(DesignLabel, wxSFRectShape);

DesignLabel::DesignLabel() : wxSFRectShape()
{
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
        m_grid->Activate( true );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
    }
    m_prop.m_font = wxNullFont;
    m_label = "";
}

DesignLabel::DesignLabel(const wxFont font, const wxString &label, int alignment) : wxSFRectShape()
{
    m_prop.m_name = label + "_t";
    m_prop.m_tag = "";
    m_prop.m_supressPrint = false;
    m_prop.m_border = 0;
    m_prop.m_alignment = alignment;
    m_prop.m_text = label;
    m_prop.m_text.Replace( "_", " " );
    m_prop.m_font = font;
    m_label = label;
    AddStyle( sfsLOCK_CHILDREN );
    AcceptChild( "GridShape" );
    m_text = new wxSFTextShape;
    m_grid = new wxSFGridShape;
    if( m_grid && m_text )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
//        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( true );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
        if( m_grid->InsertToGrid( 0, 0, m_text ) )
        {
            switch( alignment )
            {
                case 2:
                    m_text->SetHAlign( wxSFShapeBase::halignCENTER );
                    break;
                case 0:
                    m_text->SetHAlign( wxSFShapeBase::halignLEFT );
                    break;
                case 1:
                    m_text->SetHAlign( wxSFShapeBase::halignRIGHT );
                    break;
            }
            m_text->SetVAlign( wxSFShapeBase::valignMIDDLE );
            m_text->SetFont( font );
            m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN | sfsEMIT_EVENTS );
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
    m_text->RemoveStyle( sfsSHOW_HANDLES );
    RemoveStyle( sfsSHOW_HANDLES );
    m_grid->Update();
    Update();
    m_prop.m_position.x = GetBoundingBox().GetX();
    m_prop.m_position.y = GetBoundingBox().GetY();
    m_prop.m_size.SetWidth( GetBoundingBox().GetWidth() );
    m_prop.m_size.SetHeight( GetBoundingBox().GetHeight() );
}

DesignLabel::~DesignLabel()
{
}

void DesignLabel::InitSerializable()
{
    XS_SERIALIZE( m_prop.m_name, "LabelName" );
    XS_SERIALIZE( m_prop.m_tag, "Tag" );
    XS_SERIALIZE( m_prop.m_supressPrint, "SuppressPrint" );
    XS_SERIALIZE( m_prop.m_border, "Border" );
    XS_SERIALIZE( m_prop.m_alignment, "Alignment" );
    XS_SERIALIZE( m_prop.m_text, "Text" );
    XS_SERIALIZE( m_prop.m_font, "LabelFont" );
    XS_SERIALIZE( m_prop.m_position, "LabelPosition" );
    XS_SERIALIZE( m_prop.m_size, "LabelSize" );
}

Properties DesignLabel::GetProperties()
{
    return m_prop;
}
