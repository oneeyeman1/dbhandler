// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "addcolumnsdialog.h"

AddColumnsDialog::AddColumnsDialog(wxWindow *parent, int type) : wxDialog( parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 )
{
    // begin wxGlade: MyDialog::MyDialog
    m_panel = new wxPanel( this, wxID_ANY );
    m_fields = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    m_paste = new wxButton( m_panel, wxID_ANY, _( "Paste" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    set_properties();
    do_layout();
    // end wxGlade
}

AddColumnsDialog::~AddColumnsDialog(void)
{
}

void AddColumnsDialog::set_properties()
{
    m_paste->Enable( false );
    m_paste->SetDefault();
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
