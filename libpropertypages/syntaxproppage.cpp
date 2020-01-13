// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/mdi.h"
#endif

#if defined __WXMSW__ && defined __MEMORYLEAKS__
#include <vld.h>
#endif

#include "syntaxproppage.h"

SyntaxPropPage::SyntaxPropPage(wxWindow *parent) : wxPanel( parent )
{
    m_syntax = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY );
    m_syntax->Enable( false );
    wxFont font = m_syntax->GetFont();
    font.SetFamily( wxFONTFAMILY_TELETYPE );
    m_syntax->SetFont( font );
    do_layout();
}

SyntaxPropPage::~SyntaxPropPage(void)
{
}

void SyntaxPropPage::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( m_syntax, 1, wxEXPAND, 0 );
    SetSizer( sizer1 );
}

void SyntaxPropPage::SetSyntaxText(const wxString &text)
{
    m_syntax->SetValue( text );
}

const wxTextCtrl *SyntaxPropPage::GetSyntaxCtrl()
{
    return m_syntax;
}
