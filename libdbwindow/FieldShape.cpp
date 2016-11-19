#include "wxsf/textshape.h"
#include "FieldShape.h"

FieldShape::FieldShape(void)
{
    m_backColour = wxColour( 210, 225, 245 );
}

FieldShape::~FieldShape(void)
{
}

void FieldShape::DrawNormal(wxDC &dc)
{
//    wxSFRectShape::DrawNormal(dc);
    wxRect rect = this->GetBoundingBox();
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
    }
    dc.DrawRectangle( m_parentRect.x, rect.y, m_parentRect.width, rect.height );
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
    dc.SetFont( wxNullFont );
    dc.SetBrush( wxNullBrush );
}

void FieldShape::SetParentRect(const wxRect &rect)
{
    m_parentRect = rect;
}
