// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>
#include "wx/docview.h"
#include "wx/fontenum.h"
#include "wx/docmdi.h"
#include "wx/dynlib.h"
#include "wx/bmpcbox.h"
#include "wx/notebook.h"
#include "wx/fdrepdlg.h"
#include "wx/grid.h"
#include "wx/stc/stc.h"
#include "wx/listctrl.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RectShape.h"
#include "wxsf/GridShape.h"
#include "wxsf/DiagramManager.h"
#include "wxsf/FlexGridShape.h"
//#include "XmlSerializer.h"
#include "database.h"
#include "constraint.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "fieldwindow.h"
#include "constraintsign.h"
#include "FieldShape.h"
#include "MyErdTable.h"
#include "sortgroupbypage.h"
#include "syntaxproppage.h"
#include "wherehavingpage.h"
#include "databasecanvas.h"
#include "databasedoc.h"
#include "taborder.h"
#include "divider.h"
#include "designlabel.h"
#include "designfield.h"
#include "designcanvas.h"
#include "databaseview.h"

typedef int (*CREATEPROPERTIESDIALOG)(wxWindow *parent, Database *, int type, void *object, wxString &, bool, const wxString &, const wxString &, const wxString &, wxCriticalSection &);

DesignCanvas::DesignCanvas(wxView *view, const wxPoint &point) : wxSFShapeCanvas()
{
    m_view = view;
    startPoint.x = 1;
    startPoint.y = 1;
    m_pManager.SetRootItem( new xsSerializable() );
    SetDiagramManager( &m_pManager );
    Create( view->GetFrame(), wxID_ANY, point, wxDefaultSize, wxHSCROLL | wxVSCROLL | wxALWAYS_SHOW_SB );
    SetVirtualSize( 1000, 1000 );
    SetScrollRate( 10, 10 );
    m_mode = modeDESIGN;
    SetCanvasColour( *wxWHITE );
    Bind( wxEVT_MENU, &DesignCanvas::OnProperties, this, wxID_PROPERTIES );
    m_menuShape = NULL;
}

DesignCanvas::~DesignCanvas()
{
}

void DesignCanvas::SetQuickQueryFields(const std::vector<wxString> &fields)
{
    m_quickQueryFields = fields;
}

void DesignCanvas::AddFieldLabelToCanvas(const wxFont labelFont, const Field *label)
{
    wxRect rectLabel, rectField;
    auto labelShape = new DesignLabel( labelFont, const_cast<Field *>( label )->GetLabel() );
    m_pManager.AddShape( labelShape, NULL, wxPoint( startPoint.x, startPoint.y ), sfINITIALIZE, sfDONT_SAVE_STATE );
    rectLabel = labelShape->GetBoundingBox();
    startPoint.x += rectLabel.GetWidth() + 2;
    Refresh();
}

void DesignCanvas::AddFieldToCanvas(const wxFont dataFont, const Field *label)
{
    ShapeList list;
    int ypos = 0;
    bool found = false;
    m_pManager.GetShapes( CLASSINFO( Divider ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end() && !found; ++it )
    {
        wxString temp = dynamic_cast<Divider *>( ( *it ) )->GetDividerType();
        if( temp.Trim() == _ ("Header") )
        {
            found = true;
            ypos = (*it)->GetBoundingBox().GetHeight() + (*it)->GetBoundingBox().GetTop() + 2;
        }
    }
    startPoint.y = ypos;
    wxRect rectLabel, rectField;
    auto labelShape = new DesignField( dataFont, const_cast<Field *>( label )->GetLabel() );
    m_pManager.AddShape( labelShape, NULL, wxPoint( startPoint.x, startPoint.y ), sfINITIALIZE, sfDONT_SAVE_STATE );
    rectLabel = labelShape->GetBoundingBox();
    startPoint.x += rectLabel.GetWidth() + 2;
    Refresh();
}

void DesignCanvas::AddHeaderDivider()
{
    int ypos = 1;
    ShapeList list;
    m_pManager.GetShapes( CLASSINFO( DesignLabel ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        int temp = (*it)->GetBoundingBox().GetHeight();
        if( temp + 2 > ypos )
            ypos = temp;
    }
    auto dividerShape = new Divider( _( "Header " ), &m_pManager );
    m_pManager.AddShape( dividerShape, NULL, wxPoint( 1, ypos ), sfINITIALIZE, sfDONT_SAVE_STATE );
    startPoint.x = 1;
    Refresh();
}

void DesignCanvas::AddDataDivider()
{
    int ypos = 1;
    ShapeList list;
    m_pManager.GetShapes( CLASSINFO( DesignField ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        int temp = (*it)->GetBoundingBox().GetHeight() + (*it)->GetBoundingBox().GetTop();
        if( temp + 2 > ypos )
            ypos = temp;
    }
    auto dividerShape = new Divider( _( "Data " ), &m_pManager );
    m_pManager.AddShape( dividerShape, NULL, wxPoint( 1, ypos ), sfINITIALIZE, sfDONT_SAVE_STATE );
    Refresh();
}

void DesignCanvas::OnRightDown(wxMouseEvent &event)
{
    wxPoint pt = event.GetPosition();
    ShapeList list;
    GetShapesAtPosition( pt, list );
    DesignLabel *label = NULL;
    Divider *divider = NULL;
    bool found = false;
    for( ShapeList::iterator it = list.begin(); it != list.end() && !found; ++it )
    {
        label = wxDynamicCast( (*it), DesignLabel );
        if( !label )
        {
            divider = wxDynamicCast( (*it), Divider );
            if( divider )
            {
                m_menuShape = divider;
                found = true;
            }
        }
        else
        {
            m_menuShape = label;
            found = true;
        }
    }
    wxMenu menu;
    if( label )
    {
        menu.Append( wxID_PROPERTIES, _( "Properties..." ), "", false );
    }
    else if( divider )
    {
        menu.Append( wxID_PROPERTIES, _( "Properties..." ), "Table Properties", false );
    }
    int rc = GetPopupMenuSelectionFromUser( menu, pt );
    if( rc != wxID_NONE )
    {
        wxCommandEvent evt( wxEVT_MENU, rc );
        GetEventHandler()->ProcessEvent( evt );
    }
}

void DesignCanvas::OnProperties(wxCommandEvent &WXUNUSED(event))
{
    wxCriticalSection pcs;
    wxString command = "";
    int type;
    if( wxDynamicCast( m_menuShape, DesignLabel ) )
        type = 2;
    if( wxDynamicCast( m_menuShape, Divider ) )
        type = 3;
    else
        type = -1;
    wxDynamicLibrary lib;
#ifdef __WXMSW__
    lib.Load( "dialogs" );
#elif __WXMAC__
    lib.Load( "liblibdialogs.dylib" );
#else
    lib.Load( "libdialogs" );
#endif
    int res = 0;
    if( lib.IsLoaded() )
    {
        CREATEPROPERTIESDIALOG func = (CREATEPROPERTIESDIALOG) lib.GetSymbol( "CreatePropertiesDialog" );
        if( type == 2 )
		{
			Properties prop = dynamic_cast<DesignLabel *>( m_menuShape )->GetProperties();
            res = func( m_view->GetFrame(), ((DrawingDocument *) m_view->GetDocument())->GetDatabase(), type, &prop, command, false, wxEmptyString, wxEmptyString, wxEmptyString, pcs );
		}
        if( type == 3 )
            res = func( m_view->GetFrame(), ((DrawingDocument *) m_view->GetDocument())->GetDatabase(), type, m_menuShape, command, false, wxEmptyString, wxEmptyString, wxEmptyString, pcs );
        if( res != wxID_CANCEL )
        {
/*            m_text->AppendText( command );
            m_text->AppendText( "\n\r\n\r" );
            if( !m_log->IsShown() )
                m_log->Show();*/
        }
    }
    m_menuShape = NULL;
}

void DesignCanvas::OnLeftDown(wxMouseEvent &event)
{
    wxPoint pt = event.GetPosition();
    ShapeList list;
    GetShapesAtPosition( pt, list );
    DesignLabel *label = nullptr;
    DesignField *field = nullptr;
    Divider *divider = nullptr;
    if( !wxGetKeyState (WXK_CONTROL) )
    {
        ShapeList selShapes;
        this->GetSelectedShapes( selShapes );
        for( ShapeList::iterator it = selShapes.begin(); it != selShapes.end(); ++it )
        {
            (*it)->Select( false );
        }
    }
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        label = wxDynamicCast( (*it), DesignLabel );
        if( !label )
        {
            field = wxDynamicCast( (*it), DesignField );
            if( !field )
            {
                divider = wxDynamicCast( (*it), Divider );
                if( !divider )
                    continue;
            }
            else
            {
                field->Select( true );
                dynamic_cast<DrawingView *>( m_view )->ChangeFontEement();
            }
        }
        else
        {
            label->Select( true );
            dynamic_cast<DrawingView *>( m_view )->ChangeFontEement();
        }
    }
}

void DesignCanvas::InitialFieldSizing ()
{
    ShapeList labels, fields;
    m_pManager.GetShapes( CLASSINFO( DesignLabel ), labels );
    m_pManager.GetShapes( CLASSINFO( DesignField ), fields );
    int i = 0;
    for( ShapeList::iterator it = labels.begin(); it != labels.end(); ++it )
    {
        wxRect labelWidth = (*it)->GetBoundingBox();
        wxRect fieldWidth = fields[i]->GetBoundingBox();
        if( labelWidth.GetWidth() > fieldWidth.GetWidth() )
        {
            dynamic_cast<wxSFRectShape *>( fields[i] )->MoveTo( (*it)->GetAbsolutePosition().x, fields[i]->GetAbsolutePosition().y );
            dynamic_cast<wxSFRectShape *>( fields[i] )->SetRectSize( labelWidth.GetWidth(), fieldWidth.GetHeight() );
        }
        if( labelWidth.GetWidth() < fieldWidth.GetWidth() )
        {
            dynamic_cast<wxSFRectShape *>( (*it) )->MoveTo( fields[i]->GetAbsolutePosition().x, (*it)->GetAbsolutePosition().y );
            dynamic_cast<wxSFRectShape *>( (*it) )->SetRectSize( fieldWidth.GetWidth(), labelWidth.GetHeight() );
        }
        i++;
    }
}
