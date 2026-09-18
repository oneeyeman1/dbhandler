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
#include "dialogs.h"
#include "functionlistbox.h"
#include "addcolumnsdialog.h"

AddColumnsDialog::AddColumnsDialog(wxWindow *parent, int type, const wxPoint &pos, const std::vector<std::wstring> &fields, const wxString &dbType, const wxString &dbSubtype, const std::vector<QueryArguments> &args) : wxDialog( parent, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 0 )
{
    m_pos = pos;
    m_dbType = dbType;
    m_dbSubtype = dbSubtype;
    m_type = type;
    m_allFields = fields;
    m_args = args;
    // begin wxGlade: MyDialog::MyDialog
    m_panel = new wxPanel( this, wxID_ANY );
    m_fields = new FunctionListBox( m_panel, m_dbType.ToStdWstring(), m_dbSubtype.ToStdWstring(), m_type );
//    m_fields = new wxListBox( m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0, NULL, wxLB_SINGLE );
    m_paste = new wxButton( m_panel, wxID_OK, _( "Paste" ) );
    m_paste->SetDefault();
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    set_properties();
    do_layout();
    // end wxGlade
    m_paste->Bind( wxEVT_UPDATE_UI, &AddColumnsDialog::OnPasteUpdateUI, this );
    m_fields->Bind( wxEVT_LISTBOX_DCLICK, &AddColumnsDialog::OnFieldsDoubleClick, this );
}

AddColumnsDialog::~AddColumnsDialog(void)
{
}

void AddColumnsDialog::set_properties()
{
    m_paste->Enable( false );
    m_paste->SetDefault();
    if( m_type == 1 )
    {
        for( std::vector<std::wstring>::iterator it = m_allFields.begin(); it < m_allFields.end(); it++ )
        {
            m_fields->Append( (*it) );
        }
    }
    if( m_type == 3 )
    {
        for( std::vector<QueryArguments>::iterator it = m_args.begin (); it < m_args.end (); ++it )
        {
            if( ( m_dbType == "ODBC" && m_dbSubtype == "Microsft SQL Sertver" ) || m_dbType == "Microsoft SQL Server" )
                m_fields->Append( "@" + (*it).m_name );
            else
                m_fields->Append( ":" + (*it).m_name );
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
    if( m_fields->GetSelection() != wxNOT_FOUND )
        event.Enable( true );
    else
        event.Enable( false );
}

wxListBox *AddColumnsDialog::GetFieldsControl() const
{
    return m_fields;
}

void AddColumnsDialog::OnFieldsDoubleClick(wxCommandEvent &WXUNUSED(event))
{
    EndModal( wxID_OK );
}

int AddColumnsDialog::ShowModal()
{
    wxSize size = GetSize();
    SetPosition( wxPoint( m_pos.x, m_pos.y - size.y ) );
    return wxDialog::ShowModal();
}