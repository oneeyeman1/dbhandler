// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/fontenum.h"
#include "wx/bmpcbox.h"
#include "fontcombobox.h"

FontComboBox::FontComboBox(wxWindow *parent) : wxBitmapComboBox(parent, wxID_ANY)
{
    m_enumerator.EnumerateFacenames();
    if( m_enumerator.GotAny () )
    {
        int count = m_enumerator.GetFacenames().GetCount();
        for( int i = 0; i < count; ++i )
            Append( m_enumerator.GetFacenames().Item( i ) );
    }
    SetSelection( 0 );
}