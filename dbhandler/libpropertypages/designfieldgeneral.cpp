// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "propertypagebase.h"
#include "designfieldgeneral.h"

DesignFieldGeneral::DesignFieldGeneral(wxWindow *parent) : wxPanel( parent, wxID_ANY )
{
    auto horizontal = new wxBoxSizer( wxHORIZONTAL );
    horizontal->Add( 5, 5, 0, wxEXPAND, 0 );
    auto vertical = new wxBoxSizer( wxVERTICAL );
    vertical->Add( 5, 5, 0, wxEXPAND, 0 );
    auto main = new wxBoxSizer( wxVERTICAL );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Name" ) );
    main->Add( m_label1, 0, wxEXPAND, 0 );
    m_name = new wxTextCtrl( this, wxID_ANY );
    main->Add( m_name, 0, wxEXPAND, 0 );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Tag" ) );
    main->Add( m_label2, 0, wxEXPAND, 0 );
    m_tag = new wxTextCtrl( this, wxID_ANY );
    main->Add( m_tag, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    m_suppressPrint = new wxCheckBox( this, wxID_ANY, _( "&Suppress Print After First Newspaper Column" ) );
    main->Add( m_suppressPrint, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    auto paramSizer = new wxBoxSizer( wxHORIZONTAL );
    auto border = new wxBoxSizer( wxHORIZONTAL );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Brder" ) );
    border->Add( m_label3, 0, wxEXPAND, 0 );
    border->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString borderChoices[] = {
        _( "None" ),
        _( "Underline" ),
        _( "Box" ),
        _( "Resize" ),
        _( "Shadow Box" ),
        _( "3D Raised" ),
        _( "3D Lowered" )
    };
    m_border = new wxComboBox( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 4, borderChoices );
    border->Add( m_border, 0, wxEXPAND, 0 );
    paramSizer->Add( border, 0, wxEXPAND, 0 );
    paramSizer->Add( 5, 5, 0, wxEXPAND, 0 );
    auto alignment = new wxBoxSizer( wxHORIZONTAL );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "Alignment" ) );
    alignment->Add( m_label4, 0, wxEXPAND, 0 );
    alignment->Add( 5, 5, 0, wxEXPAND, 0 );
    const wxString alignmentChoices[] = {
        _( "Left" ),
        _( "Right" ),
        _( "Center" ),
        _( "Justify")
    };
    m_alignment = new wxComboBox( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0, alignmentChoices );
    alignment->Add( m_alignment, 0, wxEXPAND, 0 );
    paramSizer->Add( alignment, 0, wxEXPAND, 0 );
    main->Add( paramSizer, 0, wxEXPAND, 0 );
    vertical->Add( 5, 5, 0, wxEXPAND, 0 );
    auto criteria = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _( "Query Criteria" ) ), wxHORIZONTAL  );
    m_equality = new wxCheckBox( this, wxID_ANY, _( "&Equality Required" ) );
    criteria->Add( m_equality, 0, wxEXPAND, 0 );
    criteria->Add( 5, 5, 0, wxEXPAND, 0 );
    m_override = new wxCheckBox( this, wxID_ANY, _( "&Override Edit" ) );
    criteria->Add( m_override, 0, wxEXPAND, 0 );
    main->Add( criteria, 0, wxEXPAND, 0 );
    vertical->Add( 5, 5, 0, wxEXPAND, 0 );
    auto pict_rtl = new wxBoxSizer( wxHORIZONTAL );
    m_picture = new wxCheckBox( this, wxID_ANY, _( "Display as Picture" ) );
    pict_rtl->Add( m_picture, 0, wxEXPAND, 0 );
    pict_rtl->Add( 5, 5, 0, wxEXPAND, 0 );
    m_rtl = new wxCheckBox( this, wxID_ANY, _( "&Right To Left" ) );
    pict_rtl->Add( m_rtl, 0, wxEXPAND, 0 );
    main->Add( pict_rtl, 0, wxEXPAND, 0 );
    vertical->Add( main, 0, wxEXPAND, 0 );
    vertical->Add( 5, 5, 0, wxEXPAND, 0 );
    horizontal->Add( vertical, 0, wxEXPAND, 0 );
    horizontal->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( horizontal );
}

