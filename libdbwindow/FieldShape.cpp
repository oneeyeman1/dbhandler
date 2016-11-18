#include "wxsf/textshape.h"
#include "FieldShape.h"

FieldShape::FieldShape(void)
{
}

FieldShape::~FieldShape(void)
{
}

void FieldShape::DrawNormal(wxDC &dc)
{
    wxSFRectShape::DrawNormal(dc);
    wxString line;
    int i = 0;
    dc.SetBrush( m_Fill );
//    dc.SetBackgroundMode( wxTRANSPARENT );
    dc.SetTextForeground( m_TextColor );
    if( this->m_fSelected )
    {
		dc.SetTextBackground( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
        dc.SetBackgroundMode( wxSOLID );
    }
    else
        dc.SetBackgroundMode( wxTRANSPARENT );
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
