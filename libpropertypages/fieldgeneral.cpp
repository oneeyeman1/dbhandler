// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "fieldgeneral.h"

FieldGeneral::FieldGeneral(wxWindow *parent) : wxPanel(parent, wxID_ANY)
{
    m_label1 = new wxStaticText( this, wxID_ANY, _( "&Column comment:" ) );
    m_comment = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_log = new wxCheckBox( this, wxID_ANY, _( "&Log Only" ) );
}

FieldGeneral::~FieldGeneral()
{
}

void FieldGeneral::do_layout()
{
}