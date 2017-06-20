// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include <vector>
#include <string>
#include "addcolumnsdialog.h"

AddColumnsDialog::AddColumnsDialog(wxWindow *parent, int type, const std::vector<std::wstring> &fields) : wxDialog( parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 )
{
    m_type = type;
    m_allFields = fields;
    // begin wxGlade: MyDialog::MyDialog
    m_panel = new wxPanel( this, wxID_ANY );
    m_fields = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    m_paste = new wxButton( m_panel, wxID_ANY, _( "Paste" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    set_properties();
    do_layout();
    // end wxGlade
    m_paste->Bind( wxEVT_UPDATE_UI, &AddColumnsDialog::OnPasteUpdateUI, this );
}

AddColumnsDialog::~AddColumnsDialog(void)
{
}

void AddColumnsDialog::set_properties()
{
    m_paste->Enable( false );
    m_paste->SetDefault();
    if( m_type == 2 )
    {
        m_fields->Append( "abs()" );
        m_fields->Append( "avg()" );
        m_fields->Append( "count()" );
        m_fields->Append( "date()" );
        m_fields->Append( "dateformat()" );
        m_fields->Append( "datetime()" );
        m_fields->Append( "day()" );
        m_fields->Append( "days()" );
        m_fields->Append( "dow()" );
        m_fields->Append( "hour()" );
        m_fields->Append( "hours()" );
        m_fields->Append( "ifnull()" );
        m_fields->Append( "isnull()" );
        m_fields->Append( "length()" );
        m_fields->Append( "list()" );
        m_fields->Append( "max()" );
        m_fields->Append( "min()" );
        m_fields->Append( "minute()" );
        m_fields->Append( "minutes()" );
        m_fields->Append( "mod()" );
        m_fields->Append( "month()" );
        m_fields->Append( "months()" );
        m_fields->Append( "now(*)" );
        m_fields->Append( "number(*)" );
        m_fields->Append( "remainder()" );
        m_fields->Append( "second()" );
        m_fields->Append( "seconds()" );
        m_fields->Append( "similar()" );
        m_fields->Append( "soundex()" );
        m_fields->Append( "string()" );
        m_fields->Append( "substr()" );
        m_fields->Append( "sum()" );
        m_fields->Append( "today(*)" );
        m_fields->Append( "weeks()" );
        m_fields->Append( "year()" );
        m_fields->Append( "years()" );
        m_fields->Append( "ymd()" );
    }
    else
    {
        for( std::vector<std::wstring>::iterator it = m_allFields.begin(); it < m_allFields.end(); it++ )
        {
            m_fields->Append( (*it) );
        }
    }
}

void AddColumnsDialog::do_layout()
{
    // begin wxGlade: MyDialog::do_layout
    wxBoxSizer* sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer1 = new wxBoxSizer( wxVERTICAL );
    wxBoxSizer* sizer2 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer3 = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer* sizer4 = new wxBoxSizer( wxVERTICAL );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer3->Add( m_fields, 0, wxEXPAND, 0 );
    sizer3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_paste, 0, 0, 0 );
    sizer4->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer4->Add( m_cancel, 0, 0, 0 );
    sizer3->Add( sizer4, 0, 0, 0 );
    sizer2->Add( sizer3, 0, 0, 0 );
    sizer2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( sizer2, 0, 0, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer1 );
    sizer->Add( m_panel, 1, 0, 0 );
    SetSizer( sizer );
    sizer->Fit( this );
    Layout();
    // end wxGlade
}

void AddColumnsDialog::OnPasteUpdateUI(wxUpdateUIEvent &event)
{
    if( m_fields->GetSelection() )
        event.Enable( true );
	else
        event.Enable( false );
}
