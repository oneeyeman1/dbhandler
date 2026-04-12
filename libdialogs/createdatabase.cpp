//
//  createdatabase.cpp
//  libdialogs
//
//  Created by Igor Korot on 6/28/17.
//  Copyright (c) 2017 Igor Korot. All rights reserved.
//

#include "wx/wx.h"
#include "createdatabase.h"

CreateDatabase::CreateDatabase(wxWindow *parent, const std::wstring &type, const std::wstring &subtype) : wxDialog( parent, wxID_ANY, _( "Create Database" ) )
{
    auto main = new wxBoxSizer( wxHORIZONTAL );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    auto second = new wxBoxSizer( wxVERTICAL );
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    second->Add( 5, 5, 0, wxEXPAND, 0 );
    main->Add( second, 0, wxEXPAND, 0 );
    main->Add( 5, 5, 0, wxEXPAND, 0 );
    auto sizer = new wxStdDialogButtonSizer();
    sizer->Add( new wxButton( this, wxID_OK, _( "OK" ) ) );
    sizer->Add( new wxButton( this, wxID_CANCEL, _( "CANCEL" ) ) );
    sizer->Add( new wxButton( this, wxID_HELP, _( "Help" ) ) );
    sizer->Realize();
    main->Add( sizer, 0, wxEXPAND, 0 );
    SetSizer( main );
    Layout();
}

