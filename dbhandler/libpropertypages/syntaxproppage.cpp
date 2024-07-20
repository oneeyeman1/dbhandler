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
    m_syntax->SetBackgroundColour( GetBackgroundColour() );
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

wxTextCtrl *SyntaxPropPage::GetSyntaxCtrl() const
{
    return m_syntax;
}

void SyntaxPropPage::RemoveTableSort(const wxString tbl)
{
    auto query = m_syntax->GetValue();
    auto pos = query.find( "ORDER BY");
    auto subp = query.substr( 0, pos );
    if( pos != wxNOT_FOUND )
    {
        wxString replacer = "ORDER BY ";
        auto stringToReplace = query.substr( pos );
        auto tablePos = stringToReplace.find( "," );
        while( tablePos != wxNOT_FOUND )
        {
            auto str = stringToReplace.substr( 8, tablePos );
            auto tablecheck = str.find( tbl );
            if( tablecheck == wxNOT_FOUND )
                replacer += str;
            stringToReplace = stringToReplace.substr( tablePos + 1 );
            tablePos = stringToReplace.find( "," );
        }
        auto tablecheck = stringToReplace.find( tbl );
        if( tablecheck == wxNOT_FOUND )
            replacer += stringToReplace + ";";
        else
        {
            auto coma = replacer.rfind( "," );
            replacer = replacer.substr( 0, coma );
            replacer += ";";
        }
        query = subp + replacer;
        m_syntax->SetValue( query );
    }
}

void SyntaxPropPage::RemoveTableFromQuery(const wxString &tbl)
{
    wxString query = m_syntax->GetValue();
    size_t ret = query.Replace( ",\n     \"" + tbl + "\"" + "", "", false );
    if( ret == 0 )
        ret = query.Replace( ",\n     \"" + tbl + "\"" + "", "", false );
    m_syntax->SetValue( query );
}

void SyntaxPropPage::ClearQuery()
{
    m_syntax->Clear();
}
