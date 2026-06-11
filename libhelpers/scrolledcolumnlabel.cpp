// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <vector>
#include "wx/scrolwin.h"
#include "scrolledcolumnlabel.h"

ScrolledColumnLabel::ScrolledColumnLabel(wxScrolled<wxWindow> *parent, const std::vector<wxString> &labels) : wxWindow( parent, wxID_ANY )
{
    m_owner = parent;
    m_labels = labels;
    Bind( wxEVT_PAINT, &ScrolledColumnLabel::OnPaint, this );
}

void ScrolledColumnLabel::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc( this );

    // This is wrong..  it will translate both x and y if the
    // window is scrolled, the label windows are active in one
    // direction only.  Do the action below instead -- RL.
    //m_owner->PrepareDC( dc );

    int xScrollUnits, xOrigin;

    m_owner->GetViewStart( &xOrigin, nullptr );
    m_owner->GetScrollPixelsPerUnit( &xScrollUnits, nullptr );
    dc.SetDeviceOrigin( -xOrigin * xScrollUnits, 0 );
    int x = 5;
    for( auto label : m_labels )
    {
        dc.DrawText( label, x, 5 );
        x += 100;
    }
}
