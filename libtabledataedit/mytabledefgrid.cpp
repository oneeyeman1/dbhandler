#include "wx/grid.h"
#include "wx/bmpbndl.h"
#include "wx/dynlib.h"
#include <wx/msgdlg.h>
#include "mytabledefgrid.h"

MyTableDefGrid::MyTableDefGrid(wxWindow *parent, wxWindowID id) : wxGrid( parent, id), m_newRow( 0 ), m_oldRow( -1 )
{
#ifdef __WXMSW__
    HANDLE gs_wxMainThread = NULL;
    const HINSTANCE inst = wxDynamicLibrary::MSWGetModuleHandle( "tabledataedit", &gs_wxMainThread );
    const void* data = nullptr;
    size_t sizeSave = 0;
    if( !wxLoadUserResource( &data, &sizeSave, "save", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        m_pointer = wxBitmapBundle::FromSVG( (const char *) data, wxSize( 16, 16 ) );
    }
#elif __WXOSX__
    m_pointer = wxBitmapBundle::FromSVGResource( "save", wxSize( 16, 16 ) );
#else
    m_pointer = wxArtProvider::GetBitmapBundle( wxART_FLOPPY, wxART_TOOLBAR );
#endif
    SetRowLabelSize( m_pointer.GetBitmapFor( this ).GetWidth() );
//    Bind( wxEVT_GRID_CELL_LEFT_CLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_LEFT_DCLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_RIGHT_CLICK, &MyTableDefGrid::OnCellClicked, this );
//    Bind( wxEVT_GRID_CELL_RIGHT_DCLICK, &MyTableDefGrid::OnCellClicked, this );
    Bind( wxEVT_GRID_SELECT_CELL, &MyTableDefGrid::OnCellClicked, this );
}

void MyTableDefGrid::DrawRowLabel(wxDC &dc, int row)
{
    wxGrid::DrawRowLabel( dc, row );
//    if( row == m_newRow )
//        dc.DrawBitmap( m_pointer.GetBitmapFor( this ), );
}

void MyTableDefGrid::OnCellClicked(wxGridEvent &event)
{
    auto row = event.GetRow();
}
