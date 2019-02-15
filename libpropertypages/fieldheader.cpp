// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "propertypagebase.h"
#include "fieldheader.h"

FieldHeader::FieldHeader(wxWindow *parent) : PropertyPageBase( parent, wxID_ANY )
{
    m_label1 = new wxStaticText( this, wxID_ANY, _( "&Label:" ) );
    m_label = new wxTextCtrl( this, wxID_ANY, "" );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "&Position: " ) );
    m_labelPos = new wxComboBox( this, wxID_ANY );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "&Heading:" ) );
    m_heading = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_headingPos = new wxComboBox( this, wxID_ANY );
}

FieldHeader::~FieldHeader(void)
{
}

void FieldHeader::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
}
