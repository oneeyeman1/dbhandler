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

#include "database.h"
#include "guiobjectsproperties.h"
#include "configuration.h"
#include "propertypagebase.h"
#include "tablegeneral.h"

TableGeneralProperty::TableGeneralProperty(wxWindow *parent, const wxString &name, const wxString &owner, const wxString &comment, bool isTable) : PropertyPageBase( parent )
{
    m_isLogOnly = false;
    m_commentText = comment;
    m_nameText = name;
    m_ownerText = owner;
    m_isTable = isTable;
    m_isModified = false;
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Owner" ) );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Table" ) );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "&Table comment:" ) );
    m_owner = new wxTextCtrl( this, wxID_ANY, owner, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_tableName = new wxTextCtrl( this, wxID_ANY, name, wxDefaultPosition, wxDefaultSize, wxTE_READONLY );
    m_comment = new wxTextCtrl( this, wxID_ANY, comment, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_PROCESS_ENTER | wxTE_NO_VSCROLL );
    m_comment->SetMaxLength( 215 );
    m_log = new wxCheckBox( this, wxID_ANY, _( "Log Only" ) );
    set_properties();
    do_layout();
    wxButton *ok = dynamic_cast<wxButton *>( FindWindowById( wxID_OK ) );
    if( ok )
        ok->SetDefault();
    m_comment->Bind( wxEVT_CHAR, &TableGeneralProperty::OnCommentKeyEntered, this );
    m_comment->Bind( wxEVT_TEXT, &TableGeneralProperty::OnEditComment, this );
    m_log->Bind( wxEVT_CHECKBOX, &TableGeneralProperty::OnLogOnly, this );
    if( !m_isTable )
    {
        m_owner->Hide();
        m_tableName->Hide();
        m_label1->Hide();
        m_label2->Hide();
    }
    m_comment->SetFocus();
    m_comment->SetSelection( 0, -1 );
}

TableGeneralProperty::~TableGeneralProperty()
{
}

void TableGeneralProperty::set_properties()
{
    if( m_isTable )
    {
        m_owner->Enable( false );
        m_owner->SetValue( m_ownerText );
        m_tableName->Enable( false );
        m_tableName->SetValue( m_nameText );
        m_comment->SetValue( m_commentText );
    }
}

void TableGeneralProperty::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer3 = new wxFlexGridSizer( 2, 2, 5, 10 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_label1, 0, wxEXPAND, 0 );
    sizer3->Add( m_owner, 0, wxEXPAND, 0 );
    sizer3->Add( m_label2, 0, wxEXPAND, 0 );
    sizer3->Add( m_tableName, 0, wxEXPAND, 0 );
    sizer2->Add( sizer3, 1, wxEXPAND, 0 );
    sizer2->Add( m_label3, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_comment, 1, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_log, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 1, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
}

void TableGeneralProperty::OnCommentKeyEntered(wxKeyEvent &event)
{
    if( event.GetKeyCode() != WXK_RETURN )
    {
        dynamic_cast<wxButton *>( GetParent()->GetParent()->FindWindowById( wxID_APPLY ) )->Enable( true );
        event.Skip();
    }
}

void TableGeneralProperty::OnEditComment(wxCommandEvent &WXUNUSED(event))
{
    m_isModified = true;
}

const std::wstring &TableGeneralProperty::GetComment()
{
    return m_comment->GetValue().ToStdWstring();
}

wxTextCtrl *TableGeneralProperty::GetCommentCtrl()
{
    return m_comment;
}

void TableGeneralProperty::OnLogOnly(wxCommandEvent &event)
{
    m_isLogOnly = event.GetInt();
}

bool TableGeneralProperty::IsLogOnly() const
{
    return m_isLogOnly;
}
