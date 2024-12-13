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
#include "database.h"
#include "propertieshandlerbase.h"
#include "guiobjectsproperties.h"
#include "dbview.h"
#include "designlabel.h"

XS_IMPLEMENT_CLONABLE_CLASS(DesignLabel, wxSFRectShape);

DesignLabel::DesignLabel() : wxSFRectShape()
{
    m_object = DesignLabelPropertiesType;
    auto prop = m_any.As<DesignLabelProperties>();
    SetHAlign( wxSFShapeBase::halignCENTER );
    SetVAlign( wxSFShapeBase::valignMIDDLE );
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSIZE_CHANGE );
    AcceptChild( "GridShape" );
    m_grid = new wxSFGridShape;
    if( m_grid )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN | sfsSHOW_HANDLES );
        m_grid->SetDimensions( 1, 1 );
        m_grid->SetFill( *wxTRANSPARENT_BRUSH );
        m_grid->SetBorder( *wxTRANSPARENT_PEN );
        m_grid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_grid->Activate( true );
        SF_ADD_COMPONENT( m_grid, wxT( "grid" ) );
    }
    prop.m_font = wxNullFont;
    m_label = "";
    InitSerializable();
}

DesignLabel::DesignLabel(const wxFont font, const wxString &label, int alignment) : wxSFRectShape()
{
    auto prop = m_any.As<DesignLabelProperties>();
    m_object = DesignLabelPropertiesType;
    prop.m_general.m_name = label + "_t";
    prop.m_general.m_tag = "";
    prop.m_general.m_supressPrint = false;
    prop.m_general.m_border = 0;
    prop.m_general.m_alignment = alignment;
    prop.m_general.m_text = label;
    prop.m_general.m_text.Replace( "_", " " );
    prop.m_font = font;
    m_label = label;
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSIZE_CHANGE );
    AcceptChild( "GridShape" );
    m_text = new wxSFTextShape;
    m_grid = new wxSFGridShape;
    if( m_grid && m_text )
    {
        m_grid->SetRelativePosition( 0, 1 );
        m_grid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN | sfsSHOW_HANDLES );
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
    prop.m_position.m_position.x = GetBoundingBox().GetX();
    prop.m_position.m_position.y = GetBoundingBox().GetY();
    prop.m_position.m_size.SetWidth( GetBoundingBox().GetWidth() );
    prop.m_position.m_size.SetHeight( GetBoundingBox().GetHeight() );
    InitSerializable();
}

DesignLabel::~DesignLabel()
{
}

void DesignLabel::InitSerializable()
{
    auto prop = m_any.As<DesignLabelProperties>();
    XS_SERIALIZE( prop.m_general.m_name, "LabelName" );
    XS_SERIALIZE( prop.m_general.m_tag, "Tag" );
    XS_SERIALIZE( prop.m_general.m_supressPrint, "SuppressPrint" );
    XS_SERIALIZE( prop.m_general.m_border, "Border" );
    XS_SERIALIZE( prop.m_general.m_alignment, "Alignment" );
    XS_SERIALIZE( prop.m_general.m_text, "Text" );
    XS_SERIALIZE( prop.m_font, "LabelFont" );
    XS_SERIALIZE( prop.m_position.m_position, "LabelPosition" );
    XS_SERIALIZE( prop.m_position.m_size, "LabelSize" );
}

wxAny &DesignLabel::GetProperties()
{
    return m_any;
}

void DesignLabel::Select(bool selected)
{
    m_fSelected = selected;
    wxSFRectShape::Select( selected );
}

int DesignLabel::ApplyProperties(const wxAny &any, bool logOnly, std::wstring &command)
{
    return 0;
}
