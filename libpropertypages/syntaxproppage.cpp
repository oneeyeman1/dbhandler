// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#ifdef __WXMSW__
#include <vld.h>
#endif

#include "syntaxproppage.h"

SyntaxPropPage::SyntaxPropPage(wxWindow *parent) : wxPanel( parent )
{
    m_syntax = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
    m_syntax->Enable( false );
    do_layout();
}

SyntaxPropPage::~SyntaxPropPage(void)
{
}

void SyntaxPropPage::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( m_syntax, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
}

void SyntaxPropPage::SetSyntaxText(const wxString &text)
{
    m_syntax->SetLabel( text );
}
