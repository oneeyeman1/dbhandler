/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/stockitem.h"
#endif

#include "tablegeneral.h"

TableGeneralProperty::TableGeneralProperty(wxWindow *parent) : wxPanel( parent )
{
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Pwner" ) );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Table" ) );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "&Table comment:" ) );
    m_owner = new wxTextCtrl( this, wxID_ANY );
    m_table = new wxTextCtrl( this, wxID_ANY );
    m_comment = new wxTextCtrl( this, wxID_ANY );
}

TableGeneralProperty::~TableGeneralProperty()
{
}

void TableGeneralProperty::set_properties()
{
}

void TableGeneralProperty::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );

    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
}
