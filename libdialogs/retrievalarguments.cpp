/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <list>
#include "dialogs.h"
#include "arguments.c"
#include "typecombobox.h"
#include "retrievalarguments.h"

RetrievalArguments::RetrievalArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType) : wxDialog( parent, wxID_ANY, _( "" ) )
{
    m_currentLine = 0;
    m_type = dbType;
    m_subType = subType;
    sizer = new wxBoxSizer( wxHORIZONTAL );
    wxBoxSizer *sizer_6 = new wxBoxSizer( wxVERTICAL );
    m_panel = new wxPanel( this, wxID_ANY );
    m_panel->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_add = new wxButton( m_panel, wxID_ANY, _( "Add" ) );
    m_insert = new wxButton( m_panel, wxID_ANY, _( "Insert" ) );
    m_remove = new wxButton( m_panel, wxID_ANY, _( "Delete" ) );
    wxStaticBoxSizer *main_sizer = new wxStaticBoxSizer( wxVERTICAL, m_panel, _( "Arguments" ) );

    argPanel = new wxPanel( main_sizer->GetStaticBox(), wxID_ANY );
    argPanel->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    m_labe11 = new wxStaticText( argPanel, wxID_ANY, _( "Position" ), wxPoint( 4, 4 )  );
    m_labe11->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    m_label2 = new wxStaticText( argPanel, wxID_ANY, _( "Name" ), wxPoint( 40, 4 ) );
    m_label2->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    m_label3 = new wxStaticText( argPanel, wxID_ANY, _( "Type" ), wxPoint( 48, 4 ) );
    m_label3->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );

    main_sizer->Add( argPanel, 0, wxEXPAND | wxBOTTOM, 4 );

    scroller = new wxScrolledWindow( main_sizer->GetStaticBox(), wxID_ANY );
    scroller->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );

    bmp = wxBitmap::NewFromPNGData( arguments_pointer_png, WXSIZEOF( arguments_pointer_png ) );

    fgs = new wxFlexGridSizer( 4, 0, 0 );
    dummy_1 = new wxPanel( scroller, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_1->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    wxLogDebug( "dummy_1 is %p", dummy_1 );
    dummy_2 = new wxPanel( scroller, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_2->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    wxLogDebug( "dummy_2 is %p", dummy_2 );
    dummy_3 = new wxPanel( scroller, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_3->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    wxLogDebug( "dummy_3 is %p", dummy_3 );
    dummy_4 = new wxPanel( scroller, wxID_ANY, wxDefaultPosition, wxSize( 1, 1 ) );
    dummy_4->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    wxLogDebug( "dummy_4 is %p", dummy_4 );
    fgs->Add( dummy_1 );
    fgs->Add( dummy_2 );
    fgs->Add( dummy_3 );
    fgs->Add( dummy_4 );

    for( std::vector<QueryArguments>::iterator it = arguments.begin(); it < arguments.end(); ++it )
    {
        AddArgumentsLine( (*it) );
    }
    fgs->AddGrowableCol( 2 );

    scroller->SetSizer( fgs );
    scroller->SetScrollRate( 15, 15 );

    wxSize minsize = fgs->CalcMin();
    minsize.x += wxSystemSettings::GetMetric( wxSYS_HSCROLL_Y );
    minsize.y = -1;
    scroller->SetMinClientSize( minsize );;

    main_sizer->Add( scroller, 1, wxEXPAND, 0 );
    sizer->Add( main_sizer, 1, wxEXPAND, 0 );
    sizer->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_help, 0, wxEXPAND, 0 );
    sizer_6->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_6->Add( m_add, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_insert, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_remove, 0, wxEXPAND, 0 );
    sizer->Add( sizer_6, 0, wxEXPAND | wxRIGHT, 0 );
    m_panel->SetSizer( sizer );

    //sizer->SetSizeHints( this );
    const wxSize size = sizer->ComputeFittingClientSize( this );
    SetMinClientSize( size );
    SetClientSize( size );

    Layout();
    m_panel->Bind( wxEVT_SIZE, &RetrievalArguments::OnSize, this );
    m_add->Bind( wxEVT_BUTTON, &RetrievalArguments::OnAddArgument, this );
    m_insert->Bind( wxEVT_BUTTON, &RetrievalArguments::OnInsertArgument, this );
    m_remove->Bind( wxEVT_BUTTON, &RetrievalArguments::OnRemoveArgument, this );
    m_remove->Bind( wxEVT_UPDATE_UI, &RetrievalArguments::OnRemoveUpdateUI, this );
    m_insert->Bind( wxEVT_UPDATE_UI, &RetrievalArguments::OnRemoveUpdateUI, this );
    m_remove->Bind( wxEVT_KILL_FOCUS, &RetrievalArguments::OnKillFocus, this );
    Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
    CallAfter( &RetrievalArguments::UpdateHeader );
    set_properties();
}

RetrievalArguments::~RetrievalArguments(void)
{
}

void RetrievalArguments::set_properties()
{
    SetTitle( _( "Specify Retrieval Arguments" ) );
}

void RetrievalArguments::OnSize(wxSizeEvent &event)
{
    UpdateHeader();
    event.Skip();
}

void RetrievalArguments::OnAddArgument(wxCommandEvent &WXUNUSED(event))
{
    AddArgumentsLine( QueryArguments( m_lines.size() + 1, "", "" ) );
}

void RetrievalArguments::OnInsertArgument(wxCommandEvent &WXUNUSED(event))
{
    Freeze();
    std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
    unsigned long pos = m_currentLine * 4;
    int position = wxAtoi( (*it).m_number->GetLabel() );
    wxStaticBitmap *statBmp = new wxStaticBitmap( scroller, wxID_ANY, bmp );
    wxStaticText *number = new wxStaticText( scroller, wxID_ANY, wxString::Format( "%lu", m_currentLine ), wxDefaultPosition, wxSize( 30, -1 ), wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN );
    wxTextCtrl *name = new wxTextCtrl( scroller, wxID_ANY, "" );
    name->Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
    name->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    TypeComboBox *type = new TypeComboBox( scroller, m_type.ToStdWstring(), m_subType.ToStdWstring(), "" );
    type->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    type->Bind( wxEVT_KILL_FOCUS, &RetrievalArguments::OnKillFocus, this );
    // (*it).m_pointer->SetBitmap( wxNullBitmap );
    fgs->Insert( pos, statBmp, 0, wxEXPAND | wxRIGHT | wxLEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 0 );
    fgs->Insert( pos + 1, number, 0, wxRIGHT, 8 );
    fgs->Insert( pos + 2, name, 1, wxEXPAND | wxRIGHT, 8 );
    fgs->Insert( pos + 3, type, 0, wxEXPAND | wxRIGHT, 8 );
    m_lines.insert( it, QueryLines( statBmp, number, name, type ) );
    name->SetFocus();
    for( ; it != m_lines.end(); ++it )
    {
        position++;
        (*it).m_number->SetLabel( wxString::Format( "%d", position ) );
    }
    fgs->Layout();
    Thaw();
}

void RetrievalArguments::OnRemoveArgument(wxCommandEvent &WXUNUSED(event))
{
    Freeze();
    unsigned long counter = m_currentLine;
    std::list<QueryLines>::iterator it = std::next( m_lines.begin(), m_currentLine - 1 );
    (*it).m_pointer->Destroy();
    (*it).m_name->Destroy();
    (*it).m_number->Destroy();
    (*it).m_type->Destroy();
    m_lines.erase( it );
    fgs->Layout();
    it = std::next( m_lines.begin(), m_currentLine - 1 );
    if( m_lines.size() > 0 )
    {
        if( it == m_lines.end() )
        {
            it--;
            counter--;
            m_currentLine--;
        }

        for( ; it != m_lines.end(); ++it )
        {
            (*it).m_number->SetLabel( wxString::Format( "%lu", counter ) );
            counter++;
        }

        // (*it).m_pointer->SetBitmap( pointerBitmap );
        // (*it).m_name->SetFocus();
    }
    SetActiveLine( m_currentLine );
    Thaw();
}

void RetrievalArguments::UpdateHeader()
{
    m_label2->SetPosition( wxPoint( dummy_3->GetPosition().x, -1 ) );
    m_label3->SetPosition( wxPoint( dummy_4->GetPosition().x, -1 ) );
}

void RetrievalArguments::OnKeyDown(wxKeyEvent &event)
{
    if( event.GetKeyCode() == WXK_UP )
    {
        // range check happens inside method anyway
        SetActiveLine( m_currentLine - 1 );
    }
    else if( event.GetKeyCode() == WXK_DOWN )
    {
        // range check happens inside method anyway
        SetActiveLine( m_currentLine + 1 );
    }
    event.Skip();
}

void RetrievalArguments::OnRemoveUpdateUI(wxUpdateUIEvent &event)
{
    if( m_lines.empty() )
        event.Enable( false );
    else
        event.Enable( true );
}

std::list<QueryLines> &RetrievalArguments::GetArgumentLines()
{
    return m_lines;
}

int RetrievalArguments::FindLineNumberFromControl( wxWindow *win )
{
    if( win == NULL ) return -1;

    int lineNumber = 0;
    std::list<QueryLines>::iterator it;
    for( it = m_lines.begin(); it != m_lines.end(); ++it, lineNumber++ )
    {
        const QueryLines &line = (*it);
        if( line.m_name == win || line.m_type == win )
        {
            wxLogDebug( "FindLineNumberFromControl: %d", lineNumber );
            return lineNumber + 1;
        }
    }
    return -1;
}

void RetrievalArguments::SetActiveLine( int line )
{
    int index = line - 1;
    if( index < 0 || index >= m_lines.size() )
      return;

    // just set focus to the first control in that line
    // focus handler will do the rest
    std::list<QueryLines>::iterator it = std::next( m_lines.begin(), index );
    (*it).m_name->SetFocus();
}

void RetrievalArguments::SetIndicatorLine( int activeLine )
{
    int activeIndex = activeLine - 1;
    int lineNumber = 0;
    std::list<QueryLines>::iterator it;
    for( it = m_lines.begin(); it != m_lines.end(); ++it, lineNumber++ )
    {
        const QueryLines &line = (*it);
        line.m_pointer->Show( activeIndex == lineNumber );
    }
    m_currentLine = activeLine;
}

void RetrievalArguments::OnSetFocus(wxFocusEvent &event)
{
    wxLogDebug( "OnSetFocus %p", event.GetEventObject() );

    int focus_line = FindLineNumberFromControl( wxDynamicCast( event.GetEventObject(), wxWindow ) );
    if( focus_line >= 1 && focus_line <= m_lines.size() )
    {
        SetIndicatorLine( focus_line );
    }

    event.Skip();
}

void RetrievalArguments::OnKillFocus(wxFocusEvent &event)
{
    wxLogDebug( "OnKillFocus %p", event.GetEventObject() );

    if( dynamic_cast<wxTextCtrl *>( event.GetEventObject () ) && dynamic_cast<wxPanel *>( event.GetWindow() ) )
    {
        m_remove->SetFocus();
    }
    if( dynamic_cast<wxButton *>( event.GetEventObject () ) && dynamic_cast<wxPanel *>( event.GetWindow () ) )
    {
        if( m_lines.size() > 0 )
            m_lines.begin()->m_name->SetFocus();
    }
    event.Skip();
}

void RetrievalArguments::AddArgumentsLine(const QueryArguments &args)
{
    Freeze();
    // if( !m_lines.empty() )
    // (*it).m_pointer->SetBitmap( wxNullBitmap );
    wxStaticBitmap *statBmp = new wxStaticBitmap( scroller, wxID_ANY, bmp );
    wxStaticText *number = new wxStaticText( scroller, wxID_ANY, wxString::Format( "%lu", args.m_pos ), wxDefaultPosition, wxSize( 30, -1 ), wxALIGN_CENTRE_HORIZONTAL | wxBORDER_SUNKEN );
    wxTextCtrl *name = new wxTextCtrl( scroller, wxID_ANY, args.m_name );
    name->Bind( wxEVT_KEY_DOWN, &RetrievalArguments::OnKeyDown, this );
    name->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    TypeComboBox *type = new TypeComboBox( scroller, m_type.ToStdWstring(), m_subType.ToStdWstring(), "" );
    type->Bind( wxEVT_SET_FOCUS, &RetrievalArguments::OnSetFocus, this );
    type->Bind( wxEVT_KILL_FOCUS, &RetrievalArguments::OnKillFocus, this );
    fgs->Add( statBmp, 0, wxEXPAND | wxRIGHT | wxLEFT | wxRESERVE_SPACE_EVEN_IF_HIDDEN, 8 );
    fgs->Add( number, 0, wxRIGHT, 8 );
    fgs->Add( name, 1, wxEXPAND | wxRIGHT, 8 );
    fgs->Add( type, 0, wxEXPAND | wxRIGHT, 8 );
    m_lines.push_back( QueryLines( statBmp, number, name, type ) );
    m_lines.back().m_name->SetFocus();
    // m_currentLine = numArgs;   // Let SetFocus handle that
    fgs->Layout();
    Thaw();
}

void RetrievalArguments::OnTextEntered(wxCommandEvent &event)
{
    auto object = dynamic_cast<wxTextCtrl *>( event.GetEventObject() );
    if( object->GetValue().IsEmpty() && m_lines.size() == 1 )
        m_lines.clear();
}
