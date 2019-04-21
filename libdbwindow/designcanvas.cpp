// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RectShape.h"
#include "wxsf/GridShape.h"
#include "database.h"
#include "divider.h"
#include "designlabel.h"
#include "designfield.h"
#include "designcanvas.h"

DesignCanvas::DesignCanvas(wxView *view)
{
    m_view = view;
    startPoint.x = 1;
    startPoint.y = 1;
    m_pManager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_pManager );
    Create( view->GetFrame(), wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxALWAYS_SHOW_SB );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 10, 10 );
    m_mode = modeDESIGN;
    SetCanvasColour( *wxWHITE );
}

DesignCanvas::~DesignCanvas()
{
}

void DesignCanvas::SetQuickQueryFields(const std::vector<wxString> &fields)
{
    m_quickQueryFields = fields;
}

void DesignCanvas::AddFieldToCanvas(const wxFont labelFont, const wxFont *dataFont, const Field *label)
{
    wxRect rectLabel, rectField;
    auto labelShape = new DesignLabel( labelFont, const_cast<Field *>( label )->GetLabel() );
    m_pManager.AddShape( labelShape, NULL, wxPoint( startPoint.x, startPoint.y ), false );
    if( startPoint.x == 1 && startPoint.y == 1 )
    {
        rectLabel = labelShape->GetBoundingBox();
        startPoint.y += rectLabel.GetHeight() + 2;
        auto divider = new Divider( _( "Header" ) );
        m_pManager.AddShape( divider, NULL, wxPoint( startPoint.x, startPoint.y ), false );
    }
    Refresh();
}
