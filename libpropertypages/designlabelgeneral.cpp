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
#include "designlabelgeneral.h"

DesignLabelGeneral::DesignLabelGeneral(wxWindow *parent, const Properties *prop) : PropertyPageBase( parent )
{
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Name" ) );
    m_name = new wxTextCtrl( this, wxID_ANY, prop->m_name );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Tag" ) );
    m_tag = new wxTextCtrl( this, wxID_ANY, prop->m_tag );
    m_suppressPrint = new wxCheckBox( this, wxID_ANY, _( "Supress Print After First Newspaper Column" ) );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Border" ) );
    const wxString choices1[] =
    {
        _( "None" ),
        _( "Underline" ),
        _( "Box" ),
        _( "Resize" ),
        _( "Shadow box" ),
        _( "3D Raised" ),
        _( "3D Lowered" )
    };
    m_border = new wxComboBox( this, wxID_ANY, _( "None" ), wxDefaultPosition, wxDefaultSize, 7, choices1 );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "Alignment" ) );
    const wxString choices2[] =
    {
        _( "Left" ),
        _( "Right" ),
        _( "Center" ),
        _( "Justify" )
    };
    m_alignment = new wxComboBox( this, wxID_ANY, _( "Center" ), wxDefaultPosition, wxDefaultSize, 4, choices2 );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "Text" ) );
    m_text = new wxTextCtrl( this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    if( prop->m_supressPrint )
        m_suppressPrint->SetValue( true );
    m_border->SetSelection( prop->m_border );
    m_alignment->SetSelection( prop->m_alignment );
    wxString text = prop->m_text;
    text.Replace( "_", " " );
    m_text->SetValue( text );
    do_layout();
    m_name->Bind( wxEVT_TEXT, &DesignLabelGeneral::DataChange, this );
    m_tag->Bind( wxEVT_TEXT, &DesignLabelGeneral::DataChange, this );
    m_suppressPrint->Bind( wxEVT_CHECKBOX, &DesignLabelGeneral::DataChange, this );
    m_border->Bind( wxEVT_COMBOBOX, &DesignLabelGeneral::DataChange, this );
    m_border->Bind( wxEVT_TEXT, &DesignLabelGeneral::DataChange, this );
    m_alignment->Bind( wxEVT_COMBOBOX, &DesignLabelGeneral::DataChange, this );
    m_alignment->Bind( wxEVT_TEXT, &DesignLabelGeneral::DataChange, this );
    m_text->Bind( wxEVT_TEXT, &DesignLabelGeneral::DataChange, this );
}

DesignLabelGeneral::~DesignLabelGeneral()
{
}

void DesignLabelGeneral::do_layout()
{
    auto mainSizer = new wxBoxSizer( wxHORIZONTAL );
    auto sizer1 = new wxBoxSizer( wxVERTICAL );
    auto sizer2 = new wxBoxSizer( wxVERTICAL );
    auto sizer3 = new wxBoxSizer( wxVERTICAL );
    auto sizer4 = new wxBoxSizer( wxVERTICAL );
    auto sizer5 = new wxBoxSizer( wxVERTICAL );
    auto sizer6 = new wxBoxSizer( wxHORIZONTAL );
    auto sizer7 = new wxBoxSizer( wxVERTICAL );
    mainSizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_label1, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( m_name, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_label2, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_tag, 0, wxEXPAND, 0 );
    sizer1->Add( sizer3, 0, wxEXPAND, 0 );
    sizer1->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer4->Add( m_suppressPrint, 0, wxEXPAND, 0 );
    sizer1->Add( sizer4, 0, wxEXPAND, 0 );
    sizer1->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer6->Add( m_label3, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_border, 0, wxEXPAND, 0 );
    sizer6->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer6->Add( m_label4, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_alignment, 0, wxEXPAND, 0 );
    sizer5->Add( sizer6, 0, wxEXPAND, 0 );
    sizer1->Add( sizer5, 0, wxEXPAND, 0 );
    sizer1->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer7->Add( m_label5, 0, wxEXPAND, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer7->Add( m_text, 0, wxEXPAND, 0 );
    sizer1->Add( sizer7, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    mainSizer->Add( sizer1, 0, wxEXPAND, 0 );
    mainSizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( mainSizer );
}

void DesignLabelGeneral::DataChange(wxCommandEvent &WXUNUSED(event))
{
    this->m_isModified = true;
}
