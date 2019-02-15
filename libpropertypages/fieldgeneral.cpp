// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "propertypagebase.h"
#include "fieldgeneral.h"

FieldGeneral::FieldGeneral(wxWindow *parent) : PropertyPageBase(parent, wxID_ANY)
{
    m_label = new wxStaticText( this, wxID_ANY, _( "&Column comment:" ) );
    m_comment = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_log = new wxCheckBox( this, wxID_ANY, _( "&Log Only" ) );
    do_layout();
}

FieldGeneral::~FieldGeneral()
{
}

void FieldGeneral::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer3 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_label, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_comment, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_log, 0, wxEXPAND, 0 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
}

bool FieldGeneral::IsLogOnly ()
{
    return m_log->GetValue();
}