#ifdef __GNUC__
#pragma implementation "dialogs.h"
#endif

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#if defined( __WXGTK__ ) || defined( __WXQT__ )
#include "pointer.h"
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
    if( !wxLoadUserResource( &data, &sizeSave, "pointer", RT_RCDATA, inst ) )
    {
        auto err = ::GetLastError();
        wxMessageBox( wxString::Format( "Error: %d!!", err ) );
    }
    else
    {
        m_pointer = wxBitmapBundle::FromSVG( (const char *) data, wxSize( 16, 16 ) );
    }
#elif __WXOSX__
    m_pointer = wxBitmapBundle::FromSVGResource( "pointer", wxSize( 16, 16 ) );
#else
    m_pointer = wxBitmapBundle::FromSVG( pointer, wxSize( 16, 16 ) );
#endif
    SetRowLabelSize( m_pointer.GetBitmapFor( this ).GetWidth() );
}

void MyTableDefGrid::DrawRowLabel(wxDC &dc, int row)
{
    wxGrid::DrawRowLabel( dc, row );
    if( row == m_newRow )
        dc.DrawBitmap( m_pointer.GetBitmapFor( this ), wxPoint( 0, GetRowTop( row ) ) );
}

