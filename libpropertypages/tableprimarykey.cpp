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
#include "wx/listctrl.h"
#include "wxsf/ShapeCanvas.h"
#include "database.h"
#include "field.h"
#include "fieldwindow.h"
#include "propertypagebase.h"
#include "tableprimarykey.h"

TablePrimaryKey::TablePrimaryKey(wxWindow *parent, const DatabaseTable *table) : PropertyPageBase( parent, wxID_ANY )
{
    m_isModified = false;
    m_table = table;
    m_foreignKeyColumnsFields = new FieldWindow( this/*, pt1, width1*/ );
    m_label = new wxStaticText( this, wxID_ANY, _( "Table Columns" ) );
    m_fields = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
    m_fields->AppendColumn( m_table->GetTableName() );
    int row = 0;
    for( std::vector<TableField *>::const_iterator it = m_table->GetFields().begin(); it < m_table->GetFields().end(); it++ )
    {
        m_fields->InsertItem( row++, (*it)->GetFieldName() );
        if( (*it)->IsPrimaryKey() )
        {
            m_fields->SetItemState( m_fields->FindItem( -1, (*it)->GetFieldName() ), wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED );
            m_foreignKeyColumnsFields->AddField( (*it)->GetFieldName() );
        }
    }
    do_layout();
}

void TablePrimaryKey::do_layout()
{
    auto main = new wxBoxSizer( wxHORIZONTAL );
    auto sizer1 = new wxBoxSizer( wxVERTICAL );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_foreignKeyColumnsFields, 0, wxEXPAND, 0 );
    sizer1->Add( 60, 60, 0, wxEXPAND, 0 );
    sizer1->Add( m_label, 0, wxEXPAND, 0 );
    sizer1->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer1->Add( m_fields, 0, wxEXPAND, 0 );
    main->Add( sizer1, 1, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    SetSizer( main );
}
