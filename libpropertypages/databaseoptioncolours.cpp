// -*- C++ -*-
//
// generated by wxGlade 0.9.2 on Wed Jul 22 22:08:25 2020
//
// Example for compiling a single file project under Linux using g++:
//  g++ MyApp.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp
//
// Example for compiling a multi file project under Linux using g++:
//  g++ main.cpp $(wx-config --libs) $(wx-config --cxxflags) -o MyApp Dialog1.cpp Frame1.cpp
//
#include <wx/wx.h>

#include <map>
#include "wx/bmpcbox.h"
#include "wx/gdicmn.h"
#include "configuration.h"
#include "colorcombobox.h"
#include "propertypagebase.h"
#include "databaseoptioncolours.h"

DatabaseptionsColours::DatabaseptionsColours(wxWindow *parent, const DatabaseOptionColors &colorst, bool isDatabase) : PropertyPageBase( parent )
{
    m_isModified = false;
    m_defaults.m_background = wxTheColourDatabase->Find( "WHITE" );
    m_defaults.m_tableCol = wxColour( 182, 192, 192 );
    m_defaults.m_indexLine = wxColour( 255, 0, 0 );
    m_defaults.m_tableHeader = wxColour( 0x808080 );
    m_defaults.m_tableColText = wxColour( 0, 0, 0 );
    m_defaults.m_primaryKeyLine = wxColour( 0, 255, 0 );
    m_defaults.m_tableHeaderText = wxColour( 0, 0, 0 );
    m_defaults.m_tableCommentText = wxColour( 0x000080 );
    m_defaults.m_foreignKeyLine = wxColour( 0, 0, 255 );
    m_label1 = new wxStaticText( this, wxID_ANY, _( "Backgrund" ) );
    m_label2 = new wxStaticText( this, wxID_ANY, _( "Table Columns" ) );
    m_label3 = new wxStaticText( this, wxID_ANY, _( "Index Key Line" ) );
    m_label4 = new wxStaticText( this, wxID_ANY, _( "Table Header" ) );
    m_label5 = new wxStaticText( this, wxID_ANY, _( "Table Column Text" ) );
    m_label6 = new wxStaticText( this, wxID_ANY, _( "Primary Key Line" ) );
    m_label7 = new wxStaticText( this, wxID_ANY, _( "Table Header Text" ) );
    m_label8 = new wxStaticText( this, wxID_ANY, _( "Table Comment Text" ) );
    m_label9 = new wxStaticText( this, wxID_ANY, _( "Foreign Key Line"  ) );
    m_background = new CColorComboBox( this, wxID_ANY );
    m_background->SetColourValue( colorst.m_background );
    if( !isDatabase )
        m_background->Disable();
    m_textCol = new CColorComboBox( this, wxID_ANY );
    m_textCol->SetColourValue( colorst.m_tableCol );
    m_indexLine = new CColorComboBox( this, wxID_ANY );
    m_indexLine->SetColourValue( colorst.m_indexLine );
    m_tableHeader = new CColorComboBox( this, wxID_ANY );
    m_tableHeader->SetColourValue( colorst.m_tableHeader );
    m_tableColumnText = new CColorComboBox( this, wxID_ANY );
    m_tableColumnText->SetColourValue( colorst.m_tableColText );
    m_primaryKeyLine = new CColorComboBox( this, wxID_ANY );
    m_primaryKeyLine->SetColourValue( colorst.m_primaryKeyLine );
    m_tableHeaderText = new CColorComboBox( this, wxID_ANY );
    m_tableHeaderText->SetColourValue( colorst.m_tableHeaderText );
    m_tableCommentText = new CColorComboBox( this, wxID_ANY );
    m_tableCommentText->SetColourValue( colorst.m_tableCommentText );
    m_foreignKey = new CColorComboBox( this, wxID_ANY );
    m_foreignKey->SetColourValue( colorst.m_foreignKeyLine );
    restore = new wxButton( this, wxID_ANY, _( "&Default" ) );
    auto mainSizer = new wxBoxSizer( wxVERTICAL );
    mainSizer->Add( 5, 5, 0, wxEXPAND, 0 );
    auto controls = new wxBoxSizer( wxHORIZONTAL );
    controls->Add( 5, 5, 0, wxEXPAND, 0 );
    auto grid = new wxFlexGridSizer( 6, 3, 5, 5 );
    grid->Add( m_label1, 0, wxEXPAND, 0 );
    grid->Add( m_label2, 0, wxEXPAND, 0 );
    grid->Add( m_label3, 0, wxEXPAND, 0 );
    grid->Add( m_background, 0, wxEXPAND, 0 );
    grid->Add( m_textCol, 0, wxEXPAND, 0 );
    grid->Add( m_indexLine, 0, wxEXPAND, 0 );
    grid->Add( m_label4, 0, wxEXPAND, 0 );
    grid->Add( m_label5, 0, wxEXPAND, 0 );
    grid->Add( m_label6, 0, wxEXPAND, 0 );
    grid->Add( m_tableHeader, 0, wxEXPAND, 0 );
    grid->Add( m_tableColumnText, 0, wxEXPAND, 0 );
    grid->Add( m_primaryKeyLine, 0, wxEXPAND, 0 );
    grid->Add( m_label7, 0, wxEXPAND, 0 );
    grid->Add( m_label8, 0, wxEXPAND, 0 );
    grid->Add( m_label9, 0, wxEXPAND, 0 );
    grid->Add( m_tableHeaderText, 0, wxEXPAND, 0 );
    grid->Add( m_tableCommentText, 0, wxEXPAND, 0 );
    grid->Add( m_foreignKey, 0, wxEXPAND, 0 );
    controls->Add( grid, 0, wxEXPAND, 0 );
    auto buttinSizer = new wxBoxSizer( wxVERTICAL );
    controls->Add( 5, 5, 0, wxEXPAND, 0 );
    buttinSizer->Add( restore, 0, wxEXPAND, 0 );
    controls->Add( buttinSizer, 0, wxEXPAND, 0 );
    controls->Add( 5, 5, 0, wxEXPAND, 0 );
    mainSizer->Add( controls, 0, wxEXPAND, 0 );
    mainSizer->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( mainSizer );
    m_background->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_textCol->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_indexLine->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_tableHeader->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_tableColumnText->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_primaryKeyLine->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_tableHeaderText->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_tableCommentText->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    m_foreignKey->Bind( wxEVT_COMBOBOX, &DatabaseptionsColours::OnColorChanged, this );
    restore->Bind( wxEVT_BUTTON, &DatabaseptionsColours::OnRestore, this );
}

void DatabaseptionsColours::OnColorChanged(wxCommandEvent &event)
{
    if( event.GetEventObject() == m_background )
        m_current.m_background = wxColour( m_background->GetValue() );
    else if( event.GetEventObject() == m_textCol )
        m_current.m_tableCol = wxColor( m_textCol->GetValue() );
    else if( event.GetEventObject() == m_indexLine )
        m_current.m_indexLine = wxColor( m_indexLine->GetValue() );
    else if( event.GetEventObject() == m_tableHeader )
        m_current.m_tableHeader = wxColor( m_tableHeader->GetValue() );
    else if( event.GetEventObject() == m_tableColumnText )
        m_current.m_tableColText = wxColor( m_tableColumnText->GetValue() );
    else if( event.GetEventObject() == m_primaryKeyLine )
        m_current.m_primaryKeyLine = wxColor( m_primaryKeyLine->GetValue() );
    else if( event.GetEventObject() == m_tableHeaderText )
        m_current.m_tableHeaderText = wxColor( m_tableHeaderText->GetValue() );
    else if( event.GetEventObject() == m_tableCommentText )
        m_current.m_tableCommentText = wxColor( m_tableCommentText->GetValue() );
    else if( event.GetEventObject() == m_foreignKey )
        m_current.m_foreignKeyLine = wxColor( m_foreignKey->GetValue() );
    m_isModified = true;
}

void DatabaseptionsColours::OnRestore(wxCommandEvent &WXUNUSED(event))
{
    if( m_isModified )
    {
        m_current = m_defaults;
        m_background->SetColourValue( m_current.m_background );
        m_textCol->SetColourValue( m_current.m_tableCol );
        m_indexLine->SetColourValue( m_current.m_indexLine );
        m_tableHeader->SetColourValue( m_current.m_tableHeader );
        m_tableColumnText->SetColourValue( m_current.m_tableColText );
        m_primaryKeyLine->SetColourValue( m_current.m_primaryKeyLine );
        m_tableHeaderText->SetColourValue( m_current.m_tableHeaderText );
        m_tableCommentText->SetColourValue( m_current.m_tableCommentText );
        m_foreignKey->SetColourValue( m_current.m_foreignKeyLine );
    }
}