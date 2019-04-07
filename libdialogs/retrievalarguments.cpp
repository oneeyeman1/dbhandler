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

#include "wx/artprov.h"
#include "retrievalarguments.h"

MySubColLabels::MySubColLabels(wxScrolled<wxWindow> *parent) : wxWindow(parent, wxID_ANY)
{
    m_owner = parent;
    Bind( wxEVT_PAINT, &MySubColLabels::OnPaint, this );
}

void MySubColLabels::OnPaint(wxPaintEvent &WXUNUSED (event))
{
    wxPaintDC dc( this );
    int xScrollUnits, xOrigin;
    m_owner->GetViewStart( &xOrigin, 0 );
    m_owner->GetScrollPixelsPerUnit( &xScrollUnits, 0 );
    dc.SetDeviceOrigin( -xOrigin * xScrollUnits, 0 );
    dc.DrawText( _( "Position" ), 5, 5 );
    dc.DrawText( _( "Name" ), 50, 5 );
    dc.DrawText( _( "Type" ), 200, 5 );
}

MySubCanvas::MySubCanvas(wxScrolled<wxWindow> *parent, wxWindow *cols, const wxString &dbType, const wxString &subType, std::vector<QueryArguments> &arguments) : wxPanel( parent, wxID_ANY )
{
    m_owner = parent;
    m_colLabels = cols;
    wxArrayString fieldTypes;
    if( dbType == L"SQLite" )
    {
        fieldTypes.Add( "blob" );
        fieldTypes.Add( "integer" );
        fieldTypes.Add( "numeric" );
        fieldTypes.Add( "real" );
        fieldTypes.Add( "text" );
    }
    else
    {
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "JSON" );
        fieldTypes.Add( "bigint" );
        fieldTypes.Add( "binary" );
        fieldTypes.Add( "bit" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "bool" );
        fieldTypes.Add( "char" );
        fieldTypes.Add( "cursor" );
        fieldTypes.Add( "date" );
        fieldTypes.Add( "datetime" );
        fieldTypes.Add( "datetime2" );
        fieldTypes.Add( "datetimeoffset" );
        fieldTypes.Add( "decimal" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
        {
            fieldTypes.Add( "double" );
            fieldTypes.Add( "double precision" );
            fieldTypes.Add( "enum" );
        }
        fieldTypes.Add( "float" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
        {
            fieldTypes.Add( "geometry" );
            fieldTypes.Add( "geometrycollection" );
        }
        fieldTypes.Add( "hierarchyid" );
        fieldTypes.Add( "image" );
        fieldTypes.Add( "int" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
        {
            fieldTypes.Add( "linestring" );
            fieldTypes.Add( "longblob" );
            fieldTypes.Add( "longtext" );
            fieldTypes.Add( "mediumblob" );
            fieldTypes.Add("mediumint");
            fieldTypes.Add( "mediumtext" );
        }
        fieldTypes.Add( "money" );
        if( ( dbType == L"ODBC" && subType == L"MyODBC" ) || dbType == L"MyODBC" )
        {
            fieldTypes.Add( "multilinestring" );
            fieldTypes.Add( "multipoint" );
            fieldTypes.Add( "multipolygon" );
        }
        fieldTypes.Add( "nchar" );
        fieldTypes.Add( "ntext" );
        fieldTypes.Add( "numeric" );
        fieldTypes.Add( "nvarchar" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
        {
            fieldTypes.Add( "point" );
            fieldTypes.Add( "polygon" );
        }
        fieldTypes.Add( "real" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "set" );
        fieldTypes.Add( "smalldatetime" );
        fieldTypes.Add( "smallint" );
        fieldTypes.Add( "smallmoney" );
        fieldTypes.Add( "sql_variant" );
        fieldTypes.Add( "table" );
        fieldTypes.Add( "text" );
        fieldTypes.Add( "time" );
        fieldTypes.Add( "timestamp" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "tinyblob" );
        fieldTypes.Add( "tinyint" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "tinytext" );
        fieldTypes.Add("uniqueidentifier");
        fieldTypes.Add( "varbinary" );
        fieldTypes.Add( "varchar" );
        fieldTypes.Add( "xml" );
        if( ( dbType == L"ODBC" && subType == L"MySQL" ) || dbType == L"MySQL" )
            fieldTypes.Add( "year" );
    }
    auto *sizer = new wxFlexGridSizer( 0, 4, 5, 5 );
    auto counter = 1;
    for( std::vector<QueryArguments>::iterator it = arguments.begin(); it < arguments.end(); ++it )
    {
        sizer->AddGrowableRow( 1 );
        m_lines.push_back( QueryLines( new wxStaticBitmap( this, wxID_ANY, wxArtProvider::GetIcon( wxART_GO_FORWARD ) ), new wxStaticText( this, wxID_ANY, wxString::Format( "%d", counter ), wxDefaultPosition, wxDefaultSize, wxBORDER_RAISED ), new wxTextCtrl( this, wxID_ANY, (*it).m_name ), new wxComboBox( this, wxID_ANY, (*it).m_type, wxDefaultPosition, wxDefaultSize, fieldTypes ) ) );
        sizer->Add( m_lines[counter - 1].m_pointer );
        sizer->Add( m_lines[counter - 1].m_number );
        sizer->Add( m_lines[counter - 1].m_name );
        sizer->Add( m_lines[counter - 1].m_type );
        counter++;
    }
    Bind( wxEVT_PAINT, &MySubCanvas::OnPaint, this );
    SetSizer( sizer );
    sizer->Fit( this );
//    Layout();
}

void MySubCanvas::ScrollWindow(int dx, int dy, const wxRect *rect)
{
    wxPanel::ScrollWindow( dx, dy, rect );
    m_colLabels->ScrollWindow( dx, 0, rect );
}

void MySubCanvas::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc( this );
    m_owner->PrepareDC( dc );
    dc.SetPen( *wxBLACK_PEN );
    auto scroll_x = 0, scroll_y = 0;
    m_owner->CalcUnscrolledPosition( scroll_x, scroll_y, &scroll_x, &scroll_y );
    auto size_x = 0, size_y = 0;
    GetClientSize( &size_x, &size_y );
}

MySubScrolledWindow::MySubScrolledWindow(wxWindow *parent, const wxString &dbType, const wxString &subType, std::vector<QueryArguments> &arguments) : wxScrolled<wxWindow>( parent, wxID_ANY )
{
    auto *cols = new MySubColLabels( this );
    m_canvas = new MySubCanvas( this, cols, dbType, subType, arguments );
    auto *sizer = new wxFlexGridSizer( 1 );
    sizer->Add( 5, 5 );
    sizer->Add( cols, wxSizerFlags().Expand() );
    sizer->Add( m_canvas, wxSizerFlags().Expand() );
    sizer->AddGrowableRow( 1 );
    SetSizer( sizer );
    SetTargetWindow( m_canvas );
    SetScrollbars( 10, 10, 50, 50 );
    Bind( wxEVT_SIZE, &MySubScrolledWindow::OnSize, this );
}

wxSize MySubScrolledWindow::GetSizeAvailableForScrollTarget(const wxSize &size)
{
    wxSize sizeCanvas( size );
    sizeCanvas.x -= 5;
    sizeCanvas.y -= 5;
    return sizeCanvas;
}

void MySubScrolledWindow::OnSize(wxSizeEvent &event)
{
    Layout();
    AdjustScrollbars();
}

ColumnLabels::ColumnLabels(wxScrolled<wxWindow> *parent ) : wxWindow( parent, wxID_ANY )
{
    m_parent = parent;
    Bind( wxEVT_PAINT, &ColumnLabels::OnPaint, this );
}

void ColumnLabels::OnPaint(wxPaintEvent &WXUNUSED(event))
{
    wxPaintDC dc( this );
    int scrollunits, origin;
    m_parent->GetViewStart( &origin, 0 );
    m_parent->GetScrollPixelsPerUnit( &scrollunits, 0 );
    dc.SetDeviceOrigin( -origin * scrollunits, 0 );
    dc.DrawText( _( "Position" ), 5, 5 );
    dc.DrawText( _( "Name" ), 105, 5 );
    dc.DrawText( _( "Type" ), 205, 5 );
}

RetrievalArguments::RetrievalArguments(wxWindow *parent, std::vector<QueryArguments> &arguments, const wxString &dbType, const wxString &subType) : wxDialog( parent, wxID_ANY, _( "" ) )
{
    m_panel = new wxPanel( this );
    m_arguments = new MySubScrolledWindow( this, dbType, subType, arguments );
    m_ok = new wxButton( m_panel, wxID_OK, _( "OK" ) );
    m_cancel = new wxButton( m_panel, wxID_CANCEL, _( "Cancel" ) );
    m_help = new wxButton( m_panel, wxID_HELP, _( "Help" ) );
    m_add = new wxButton( m_panel, wxID_ANY, _( "Add" ) );
    m_insert = new wxButton( m_panel, wxID_ANY, _( "Insert" ) );
    m_remove = new wxButton( m_panel, wxID_ANY, _( "Delete" ) );
    set_properties();
    do_layout();
}

RetrievalArguments::~RetrievalArguments(void)
{
}

void RetrievalArguments::set_properties()
{
}

void RetrievalArguments::do_layout()
{
    auto *sizer_1 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer_2 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer_3 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_4 = new wxBoxSizer( wxHORIZONTAL );
    auto *sizer_5 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_7 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_6 = new wxBoxSizer( wxVERTICAL );
    auto *sizer_8 = new wxStaticBoxSizer(new wxStaticBox( m_panel, wxID_ANY, ( "Argument" ) ), wxHORIZONTAL );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_8->Add( m_arguments, 1, wxEXPAND, 0 );
    sizer_4->Add( sizer_8, 0, wxEXPAND, 0 );
    sizer_4->Add( 10, 10, 0, wxEXPAND, 0 );
    sizer_6->Add( m_ok, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_cancel, 0, wxEXPAND, 0 );
    sizer_6->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_6->Add( m_help, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_6, 0, wxEXPAND, 0 );
    sizer_5->Add( 20, 20, 0, wxEXPAND, 0 );
    sizer_7->Add( m_add, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_insert, 0, wxEXPAND, 0 );
    sizer_7->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_7->Add( m_remove, 0, wxEXPAND, 0 );
    sizer_5->Add( sizer_7, 0, wxEXPAND, 0 );
    sizer_4->Add( sizer_5, 0, wxEXPAND, 0 );
    sizer_3->Add( sizer_4, 0, wxEXPAND, 0 );
    sizer_3->Add( 5, 5, 0, wxEXPAND, 0 );
    sizer_2->Add( sizer_3, 0, wxEXPAND, 0 );
    sizer_2->Add( 5, 5, 0, wxEXPAND, 0 );
    m_panel->SetSizer( sizer_2 );
    sizer_1->Add( m_panel, 0, wxEXPAND, 0 );
    SetSizer( sizer_1 );
    sizer_1->Fit( this );
    Layout();
}
