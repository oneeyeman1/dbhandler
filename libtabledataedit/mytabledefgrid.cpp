#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

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
    Bind( wxEVT_GRID_SELECT_CELL, &MyTableDefGrid::OnCellClicked, this );
}

void MyTableDefGrid::DrawRowLabel(wxDC &dc, int row)
{
    wxGrid::DrawRowLabel( dc, row );
    wxGridCellAttrProvider *const  attrProvider = m_table ? m_table->GetAttrProvider() : nullptr;
//    const wxGridRowHeaderRenderer &rend = attrProvider ? attrProvider->GetRowHeaderRenderer( row ) : static_cast<const wxGridRowHeaderRenderer &>( gs_defaultHeaderRenderers.rowRenderer );
    attrProvider->GetRowHeaderRenderer( row );
    if( row == m_newRow )
        dc.DrawBitmap( m_pointer.GetBitmapFor( this ), wxPoint( 0, GetRowTop( row ) ) );
}

void MyTableDefGrid::OnCellClicked(wxGridEvent &event)
{
    auto row = event.GetRow();
    if( row != m_newRow )
    {
        m_oldRow = m_newRow;
        m_newRow = row;
        GetGridRowLabelWindow()->Refresh();
    }
}

