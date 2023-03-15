// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include <vector>
#include <memory>
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
#include "wx/dataview.h"
#include "wx/renderer.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/RectShape.h"
#include "wxsf/GridShape.h"
#include "wxsf/DiagramManager.h"
#include "wxsf/FlexGridShape.h"
//#include "XmlSerializer.h"
#include "database.h"
#include "dbview.h"
#include "objectproperties.h"
#include "constraint.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "fontcombobox.h"
#include "field.h"
#include "fieldwindow.h"
#include "constraintsign.h"
#include "FieldShape.h"
#include "MyErdTable.h"
#include "sortgroupbypage.h"
#include "syntaxproppage.h"
#include "wherehavingpage.h"
#include "databasedoc.h"
#include "databasecanvas.h"
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
    m_options.colorBackground = *wxWHITE;
    m_options.customMove = true;
    m_options.interval = 0;
    m_options.mouseSelect = true;
    m_options.rowResize = false;
    m_options.units = 3;
    m_options.cursor = -1;
    m_options.display = 2;
    m_options.cursorName = wxEmptyString;
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

void DesignCanvas::AddFieldLabelToCanvas(const wxFont labelFont, const TableField *label)
{
    wxRect rectLabel, rectField;
    auto labelShape = new DesignLabel( labelFont, const_cast<TableField *>( label )->GetFieldProperties().m_heading, const_cast<TableField *>( label )->GetFieldProperties().m_headingPosition );
    m_pManager.AddShape( labelShape, NULL, wxPoint( startPoint.x, startPoint.y ), sfINITIALIZE, sfDONT_SAVE_STATE );
    rectLabel = labelShape->GetBoundingBox();
    startPoint.x += rectLabel.GetWidth() + 2;
    Refresh();
}

void DesignCanvas::AddFieldToCanvas(const wxFont dataFont, const TableField *label)
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
    auto labelShape = new DesignField( dataFont, const_cast<TableField *>( label )->GetFieldProperties().m_label, const_cast<TableField *>( label )->GetFieldProperties().m_labelPosition );
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
    auto dividerShape = new Divider( _( "Header " ), "", -1, &m_pManager );
    m_pManager.AddShape( dividerShape, NULL, wxPoint( 1, ypos ), sfINITIALIZE, sfDONT_SAVE_STATE );
    startPoint.x = 1;
    Refresh();
}

void DesignCanvas::AddDataDivider()
{
    int ypos = 1;
    ShapeList list;
    m_pManager.GetShapes( CLASSINFO( DesignField ), list );
    if( list.size() > 0 )
    {
        for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
        {
            int temp = (*it)->GetBoundingBox().GetHeight() + (*it)->GetBoundingBox().GetTop();
            if( temp + 2 > ypos )
                ypos = temp;
        }
    }
    else
    {
        m_pManager.GetShapes( CLASSINFO( Divider ), list );
        for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
            ypos += (*it)->GetBoundingBox().GetHeight() + 2;
    }
    auto dividerShape = new Divider( _( "Data " ), "", -1, &m_pManager );
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
        if( divider )
            evt.SetEventObject( divider );
        if( label )
            evt.SetEventObject( label );
        m_view->ProcessEvent( evt );
    }
}
/**
void DesignCanvas::OnMouseMove(wxMouseEvent &event)
{
    wxSFShapeBase *shape = GetShapeUnderCursor();
    Divider *divider = wxDynamicCast( shape, Divider );
    if( divider )
        SetCursor( wxCURSOR_SIZENS );
    else
        SetCursor( *wxSTANDARD_CURSOR );
    wxSFShapeCanvas::OnMouseMove( event );
}
*/
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
    auto multiple = false;
    ShapeList list;
    wxSFShapeBase *shape;
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
    else
        multiple = true;
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        shape = wxDynamicCast( (*it), DesignLabel );
        if( !shape )
        {
            shape = wxDynamicCast( (*it), DesignField );
            if( !shape )
            {
                shape = wxDynamicCast( (*it), Divider );
                if( !shape )
                    continue;
                else
                    divider = wxDynamicCast( shape, Divider );
            }
            else
            {
                field = wxDynamicCast( shape, DesignField );
                field->Select( true );
                dynamic_cast<DrawingView *>( m_view )->ChangeFontEement();
            }
        }
        else
        {
            label = wxDynamicCast( shape, DesignLabel );
            label->Select( true );
            dynamic_cast<DrawingView *>( m_view )->ChangeFontEement();
        }
    }
    if( !multiple )
    {
        if( label || field )
        {
            if( label )
            {
                dynamic_cast<DrawingView *>( m_view )->GetFieldTextCtrl()->Enable();
                dynamic_cast<DrawingView *>( m_view )->GetFieldTextCtrl()->SetValue( label->GetProperties().m_name );
            }
            else
            {
                dynamic_cast<DrawingView *>( m_view )->GetFieldTextCtrl()->Disable();
                dynamic_cast<DrawingView *>( m_view )->GetFieldTextCtrl()->SetValue( "" );
            }
            m_selectedFont = label ? label->GetProperties().m_font : field->GetProperties().m_font;
            dynamic_cast<DrawingView *>( m_view )->GetFontName()->SetValue( m_selectedFont.GetFaceName() );
            dynamic_cast<DrawingView *>( m_view )->GetFontSize()->SetValue( wxString::Format( "%d", m_selectedFont.GetPointSize() ) );
            if( m_selectedFont.GetStyle() <= wxFONTSTYLE_NORMAL )
                dynamic_cast<DrawingView *>( m_view )->SetFontBold( false );
            else
                dynamic_cast<DrawingView *>( m_view )->SetFontBold( true );
            if( m_selectedFont.GetStyle() == wxFONTSTYLE_ITALIC )
                dynamic_cast<DrawingView *>( m_view )->SetFontItalic( true );
            else
                dynamic_cast<DrawingView *>( m_view )->SetFontItalic( false );
#ifndef __WXOSX_COCOA__
            dynamic_cast<DrawingView *>( m_view )->SetFontUnderline( m_selectedFont.wxFontBase::GetUnderlined() );
#endif
        }
    }
    Refresh();
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

void DesignCanvas::PopulateQueryCanvas(const std::vector<TableField *> &queryFields, const std::vector<GroupFields> &groupByFields)
{
    startPoint.x = 1;
    startPoint.y = 1;
    wxBeginBusyCursor();
    wxFont fontUsed;
    wxClientDC dc( this );
    auto tables = ((DrawingDocument *) m_view->GetDocument() )->GetDatabase()->GetTableVector().m_tables.begin()->second;
    auto type = ((DrawingDocument *) m_view->GetDocument() )->GetDatabase()->GetTableVector().m_type;
    bool found = false;
    if( queryFields.size() > 0 )
    {
/*    for( std::vector<wxString>::const_iterator it = groupByFields.begin(); it < groupByFields.end(); ++it )
    {
        wxFont font;
        wxString tableName = (*it).substr( 0, (*it).find( L'.' ) );
        for( std::vector<DatabaseTable *>::iterator it1 = tables.begin(); it1 < tables.end() && !found; ++it1 )
        {
            if( (*it1)->GetTableName() == tableName )
            {
                found = true;
            }
            wxFontStyle dataStyle = (*it1)->GetTableProperties().m_dataFontItalic  == 0 ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC;
            wxFontWeight dataWeight = (*it1)->GetTableProperties().m_dataFontWeight == 0 ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD;
            font.SetPointSize( (*it1)->GetTableProperties().m_dataFontSize );
            font.SetStyle( dataStyle );
            font.SetWeight( dataWeight );
            font.SetUnderlined( (*it1)->GetTableProperties().m_dataFontUnderline );
            font.SetStrikethrough( (*it1)->GetTableProperties().m_dataFontStrikethrough );
            font.SetFaceName( (*it1)->GetTableProperties().m_dataFontName );
            font.SetFamily( wxFONTFAMILY_DEFAULT );
        }
 //       AddFieldToCanvas( font, (*it) );
    }*/
        found = false;
        for( std::vector<TableField *>::const_iterator it = queryFields.begin(); it < queryFields.end(); ++it )
        {
            auto tableName = (*it)->GetFullName().substr( 0, (*it)->GetFullName().rfind( '.' ) );
            for( std::vector<DatabaseTable *>::iterator it1 = tables.begin(); it1 < tables.end() && !found; ++it1 )
            {
                std::wstring dbTableName = L"";
                if( type != L"SQLite" )
                {
                    dbTableName = (*it1)->GetCatalog() + L".";
                    dbTableName += (*it1)->GetSchemaName();
                    dbTableName += L".";
                    dbTableName += (*it1)->GetTableName();
                }
                else
                {
                    dbTableName = (*it1)->GetSchemaName() + L".";
                    dbTableName += (*it1)->GetTableName();
                }
                if( dbTableName == tableName )
                {
                    std::wstring headerStr;
                    found = true;
                    ((DrawingDocument *) m_view->GetDocument() )->GetDatabase()->GetFieldHeader( tableName, (*it)->GetFieldName(), headerStr );
                    wxString headerString( headerStr );
                    wxString dataString( (*it)->GetFieldName() );
                    headerString.Replace( "_", " " );
                    dataString.Replace( "_", " " );
                    size_t headerStringSize = headerString.Length();
                    size_t dataStringSize = dataString.Length();
                    if( headerStringSize > dataStringSize )
                    {
                        size_t diff = headerStringSize - dataStringSize;
                        for( size_t i = 0; i < diff; ++i )
                        {
                            if( i % 2 == 0 )
                                dataString = " " + dataString;
                            else
                                dataString = dataString + " ";
                        }
                    }
                    else if( dataStringSize > headerStringSize )
                    {
                        size_t diff = dataStringSize - headerStringSize;
                        for( size_t i = 0; i < diff; ++i )
                        {
                            if( i % 2 == 0 )
                                headerString = " " + headerString;
                            else
                                headerString = headerString + " ";
                        }
                    }
                    wxFontStyle labelStyle = (*it1)->GetTableProperties().m_labelFontItalic == 0 ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC;
                    wxFontWeight labelWeight = ( ( (*it1)->GetTableProperties().m_labelFontWeight == 0 ) ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD );
                    wxFontStyle dataStyle = (*it1)->GetTableProperties().m_dataFontItalic  == 0 ? wxFONTSTYLE_NORMAL : wxFONTSTYLE_ITALIC;
                    wxFontWeight dataWeight = (*it1)->GetTableProperties().m_dataFontWeight == 0 ? wxFONTWEIGHT_NORMAL : wxFONTWEIGHT_BOLD;
                    wxFont labelFont( (*it1)->GetTableProperties().m_labelFontSize, wxFONTFAMILY_DEFAULT, labelStyle, labelWeight, (*it1)->GetTableProperties().m_labelFontUnderline, (*it1)->GetTableProperties().m_labelFontName );
                    wxFont dataFont( (*it1)->GetTableProperties().m_dataFontSize, wxFONTFAMILY_DEFAULT, dataStyle, dataWeight, (*it1)->GetTableProperties().m_dataFontUnderline, (*it1)->GetTableProperties().m_dataFontName );
                    dc.SetFont( labelFont );
                    wxSize size1 = dc.GetTextExtent( headerStr );
                    dc.SetFont( dataFont );
                    wxSize size2 = dc.GetTextExtent( dataString );
                    if( size1.GetWidth() > size2.GetWidth() && size1.GetHeight() > size2.GetHeight() )
                        fontUsed = labelFont;
                    else if( size1.GetWidth() < size2.GetWidth() && size2.GetHeight() < size2.GetHeight() )
                        fontUsed = dataFont;
                    else
                        fontUsed = labelFont;
                }
            }
            AddFieldLabelToCanvas( fontUsed, (*it) );
        }
        AddHeaderDivider();
        for( std::vector<TableField *>::const_iterator it = queryFields.begin(); it < queryFields.end(); ++it )
            AddFieldToCanvas( fontUsed, (*it) );
        AddDataDivider();
        Update();
        Refresh();
    }
    wxEndBusyCursor();
}

void DesignCanvas::ClearDesignCanvas()
{
    ShapeList list;
    m_pManager.GetShapes( CLASSINFO( wxSFShapeBase ), list );
    for( ShapeList::iterator it = list.begin(); it != list.end(); ++it )
    {
        m_pManager.RemoveShape( (*it), true );
    }
}

void DesignCanvas::ChangeLabel(const wxString &label)
{
    wxSFTextShape *shape;
    DesignLabel *labelShape;
    ShapeList shapes;
    GetSelectedShapes( shapes );
    auto found = false;
    for( ShapeList::iterator it = shapes.begin(); it != shapes.end() && !found; ++it )
    {
        labelShape = wxDynamicCast( (*it), DesignLabel );
        if( labelShape )
        {
            labelShape->GetProperties().m_text = label;
        }
        shape = wxDynamicCast( (*it), wxSFTextShape );
        if( shape )
        {
            found = true;
            shape->SetText( label );
            Refresh();
            Update();
        }
    }
}
