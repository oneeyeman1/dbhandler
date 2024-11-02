// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include <string>
#include <algorithm>
#include "propertypagebase.h"
#include "guiojectsproperties.h"
#include "fieldheader.h"

FieldHeader::FieldHeader(wxWindow *parent, const FieldTableHeadingProperties &prop) : PropertyPageBase( parent, wxID_ANY )
{
    m_layout = parent->GetLayoutDirection();
    m_label1 = new wxStaticText( this, wxID_ANY, _( "&Label:" ) );
    m_label = new wxTextCtrl( this, wxID_ANY, prop.m_label );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "&Position: " ) );
    wxString choices[2];
    if( m_layout == wxLayout_RightToLeft )
    {
        choices[0] = _( "right" );
        choices[0] = _( "left" );
        m_labelPos = new wxComboBox( this, wxID_ANY, choices[prop.m_labelAlignment], wxDefaultPosition, wxDefaultSize, 2, choices, wxCB_DROPDOWN | wxCB_READONLY );
    }
    else
    {
        choices[0] = _( "left" );
        m_labelPos = new wxComboBox( this, wxID_ANY, choices[prop.m_labelAlignment], wxDefaultPosition, wxDefaultSize, 1, choices, wxCB_DROPDOWN | wxCB_READONLY );
    }
    m_label3 = new wxStaticText( this, wxID_ANY, _( "&Heading:" ) );
    m_heading = new wxTextCtrl( this, wxID_ANY, prop.m_heading, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE );
    m_label4 = new wxStaticText( this, wxID_ANY, wxEmptyString );
    wxString choices2[] =
    {
        _( "left" ),
        _( "right" ),
        _( "center" )
    };
    m_headingPos = new wxComboBox( this, wxID_ANY, choices2[prop.m_headingAlignment], wxDefaultPosition, wxDefaultSize, 3, choices2, wxCB_DROPDOWN | wxCB_READONLY );
    set_properties();
    do_layout();
}

FieldHeader::~FieldHeader(void)
{
}

void FieldHeader::do_layout()
{
    wxBoxSizer *sizer1 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer2 = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer *sizer3 = new wxFlexGridSizer( 2, 2, 15, 25 );
    wxBoxSizer *sizer4 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer5 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer6 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer *sizer7 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_label1, 0, wxEXPAND, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_label, 0, wxEXPAND, 0 );
    sizer3->Add( sizer4, 0, wxEXPAND, 0 );
    sizer5->Add( m_label2, 0, wxEXPAND, 0 );
    sizer5->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer5->Add( m_labelPos, 0, wxEXPAND, 0 );
    sizer3->Add( sizer5, 0, wxEXPAND, 0 );
    sizer6->Add( m_label3, 0, wxEXPAND, 0 );
    sizer6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer6->Add( m_heading, 0, wxEXPAND, 0 );
    sizer3->Add( sizer6, 0, wxEXPAND, 0 );
    sizer7->Add( m_label4, 0, wxEXPAND | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0 );
    sizer7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer7->Add( m_headingPos, 0, wxEXPAND, 0 );
    sizer3->Add( sizer7, 0, wxEXPAND, 0 );
    sizer2->Add( sizer3, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( sizer1 );
//    Layout();
}

void FieldHeader::set_properties()
{
    m_label->SetValue( m_labelText );
    m_heading->SetValue( m_headingText );
    if( ( m_layout == wxLayout_LeftToRight || m_layout == wxLayout_Default ) && m_labelAlignment == 1 )
        m_labelAlignment = 0;
    m_labelPos->SetSelection( m_labelAlignment );
    m_headingPos->SetSelection( m_headingAlignment );
    m_label4->Hide();
}

wxTextCtrl *FieldHeader::GetLabelCtrl()
{
    return m_label;
}

wxTextCtrl *FieldHeader::GetHeadingCtrl()
{
    return m_heading;
}

wxComboBox *FieldHeader::GetLabeAlignmentCtrl ()
{
    return m_labelPos;
}

wxComboBox *FieldHeader::GetHeaderAlignmentCtrl ()
{
    return m_headingPos;
}
