#include <string>
#include <algorithm>
#include "database.h"
#include "wxsf/TextShape.h"
#include "propertieshandlerbase.h"
#include "guiobjectsproperties.h"
#include "FieldShape.h"
#include "commentfieldshape.h"
#include "fieldtypeshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(FieldTypeShape, wxSFTextShape);

FieldTypeShape::FieldTypeShape() : wxSFTextShape()
{
    m_field = nullptr;
}

FieldTypeShape::FieldTypeShape(TableField *field) : wxSFTextShape()
{
    m_field = field;
}

FieldTypeShape::FieldTypeShape(FieldTypeShape &shape)
{
    m_field = shape.m_field;
}

FieldTypeShape::~FieldTypeShape()
{
}

const TableField *FieldTypeShape::GetFieldForComment()
{
    return m_field;
}

void FieldTypeShape::SetField(TableField *field)
{
    m_field = field;
}

void FieldTypeShape::DrawNormal(wxDC &dc)
{
    auto rect = GetBoundingBox();
    auto tableRect = GetParentShape()->GetParentShape()->GetBoundingBox();
    auto fieldRect = m_fieldShape->GetBoundingBox();
    if( m_comment && m_comment->GetText().IsEmpty() )
    {
        rect.width = tableRect.GetWidth() - rect.GetLeft();
    }
    wxString line;
    int i = 0;
    dc.SetTextForeground( m_TextColor );
    if( this->m_fSelected )
    {
        m_backColour = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
        dc.SetBrush( m_backColour );
    }
    else
    {
        m_backColour = wxColour( 210, 225, 245 );
        dc.SetBrush( m_Fill );
        dc.SetPen( m_Border );
        dc.SetBackgroundMode( wxTRANSPARENT );
    }
    dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );
    dc.SetFont( m_Font );
    wxRealPoint pos = GetAbsolutePosition();
    // draw all text lines
    wxStringTokenizer tokens( m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY );
    while( tokens.HasMoreTokens() )
    {
        line = tokens.GetNextToken();
        dc.DrawText( line, (int)pos.x, (int)pos.y + i * 12 );
        i++;
    }
    dc.SetPen( wxNullPen );
    dc.SetFont( wxNullFont );
    dc.SetBrush( wxNullBrush );
}

void FieldTypeShape::DrawSelected(wxDC& dc)
{
    DrawNormal( dc );
}
