#include <string>
#include <algorithm>
#include "database.h"
#include "wxsf/TextShape.h"
#include "propertieshandlerbase.h"
#include "guiobjectsproperties.h"
#include "commentfieldshape.h"
#include "FieldShape.h"
#include "fieldtypeshape.h"
#include "commentfieldshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(CommentFieldShape, wxSFTextShape);

CommentFieldShape::CommentFieldShape() : wxSFTextShape()
{
    m_tableField = NULL;
    m_backColor = wxColour( 210, 225, 245 );
}

CommentFieldShape::CommentFieldShape(TableField *field) : wxSFTextShape()
{
    m_tableField = field;
    m_backColor = wxColour( 210, 225, 245 );
}

CommentFieldShape::CommentFieldShape(CommentFieldShape &shape)
{
    m_tableField = shape.m_tableField;
}

CommentFieldShape::~CommentFieldShape(void)
{
}

const TableField *CommentFieldShape::GetFieldForComment()
{
    return m_tableField;
}

void CommentFieldShape::SetField(TableField *field)
{
    m_tableField = field;
}

void CommentFieldShape::DrawNormal(wxDC &dc)
{
    wxRect rect = this->GetBoundingBox();
    rect.x = m_fieldShape->GetBoundingBox().GetLeft();
    rect.width += m_fieldShape->GetBoundingBox().GetWidth();
    if( m_typeShape )
        rect.width += m_typeShape->GetBoundingBox().GetWidth();
    if( !GetText().IsEmpty() )
    {
        wxString line;
        int i = 0;
        if( this->m_fSelected )
        {
            m_backColor = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
            dc.SetBrush( m_backColor );
            if( m_typeShape )
                m_typeShape->Select( true );
            if( m_fieldShape )
                m_fieldShape->Select( true );
        }
        else
        {
            m_backColor = wxColour( 210, 225, 245 );
            dc.SetBrush( m_Fill );
            dc.SetPen( m_Border );
            dc.SetBackgroundMode( wxTRANSPARENT );
            if( m_typeShape )
                m_typeShape->Select( false );
            if( m_fieldShape )
                m_fieldShape->Select( false );
        }
        dc.DrawRectangle( rect.x, rect.y, rect.width, rect.height );
        dc.SetFont( m_Font );
        wxRealPoint pos = GetAbsolutePosition();
        // draw all text lines
        wxStringTokenizer tokens( m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY );
        while( tokens.HasMoreTokens() )
        {
            line = tokens.GetNextToken();
            dc.DrawText( line, (int )pos.x, (int) pos.y + i * 12 );
            i++;
        }
        dc.SetPen( wxNullPen );
        dc.SetFont( wxNullFont );
        dc.SetBrush( wxNullBrush );
    }
}

void CommentFieldShape::DrawSelected(wxDC &dc)
{
    DrawNormal( dc );
}
