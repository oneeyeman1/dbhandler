#include <string>
#include "database.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/CommonFcn.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/DiagramManager.h"
#include "constraintsign.h"
#include "constraint.h"
#include "HeaderGrid.h"
#include "GridTableShape.h"
#include "FieldShape.h"
#include "commentfieldshape.h"
#include "commenttableshape.h"
#include "MyErdTable.h"
#include "res/gui/key-p.xpm"
#include "res/gui/key-f.xpm"

using namespace wxSFCommonFcn;

XS_IMPLEMENT_CLONABLE_CLASS(MyErdTable, wxSFRoundRectShape);

MyErdTable::MyErdTable() : wxSFRoundRectShape()
{
    SetFill( wxBrush( wxColour( 254, 253, 211 ) ) );
    AcceptConnection( wxT( "All" ) );
    AcceptTrgNeighbour( wxT( "All" ) );
    AcceptSrcNeighbour( wxT( "All" ) );
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSHOW_SHADOW );
    SetBorder( wxPen( wxColour( 70, 125, 170 ), 1, wxPENSTYLE_SOLID ) );
    SetFill( wxBrush( wxColour( 210, 225, 245 ) ) );
    SetRadius( 15 );
    m_header = new HeaderGrid();
    m_pLabel = new wxSFTextShape();
    m_comment = new CommentTableShape();
    m_pGrid = new GridTableShape( DatabaseView );
    m_pLabel->SetId( 1000 );
    m_comment->SetId( 1001 );
    if( m_header && m_pLabel && m_comment && m_pGrid )
    {
        // set table header
        m_header->SetRelativePosition( 0, 1 );
        m_header->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_header->SetDimensions( 1, 2 );
        m_header->SetFill( *wxTRANSPARENT_BRUSH );
        m_header->SetBorder( *wxTRANSPARENT_PEN );
        m_header->AcceptChild( wxT( "wxSFTextShape" ) );
        m_header->Activate( false );
        SF_ADD_COMPONENT( m_header, wxT( "header" ) );
        //table name
        if( m_header->InsertToGrid( 0, 0, m_pLabel ) )
        {
            m_pLabel->SetVAlign( wxSFShapeBase::valignTOP );
            m_pLabel->GetFont().SetPointSize( 8 );
            m_pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
            m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
//            SF_ADD_COMPONENT( m_pLabel, wxT( "title" ) );
        }
        // table comment
        if( m_header->InsertToGrid( 0, 1, m_comment ) )
        {
            m_comment->SetVAlign( wxSFShapeBase::valignTOP );
            m_comment->SetHAlign( wxSFShapeBase::halignRIGHT );
            m_comment->GetFont().SetPointSize( 8 );
            m_comment->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
            m_comment->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
//            SF_ADD_COMPONENT( m_comment, wxT( "comment" ) );
        }
        // set grid
        m_pGrid->SetRelativePosition( 0, 17 );
        m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_pGrid->SetDimensions( 1, 3 );
        m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
        m_pGrid->SetBorder( *wxTRANSPARENT_PEN );
        m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_pGrid->SetVBorder( 13 );
        m_pGrid->SetHBorder( 5 );
        m_pGrid->AcceptChild( wxT( "FieldShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFShapeBase" ) );
        m_pGrid->Activate( true );
        SF_ADD_COMPONENT( m_pGrid, wxT( "main_grid" ) );
    }
}

MyErdTable::MyErdTable(DatabaseTable *table, ViewType type) : wxSFRoundRectShape()
{
    m_type = type;
    m_table = table;
    std::vector<Field *> fields = m_table->GetFields();
    SetBorder( wxPen( wxColour( 70, 125, 170 ), 1, wxPENSTYLE_SOLID ) );
    SetFill( wxBrush( wxColour( 210, 225, 245 ) ) );
    AcceptConnection( wxT( "All" ) );
    AcceptTrgNeighbour( wxT( "All" ) );
    AcceptSrcNeighbour( wxT( "All" ) );
    AddStyle( sfsLOCK_CHILDREN );
    SetRadius(15);
    m_header = new HeaderGrid();
    m_pLabel = new wxSFTextShape();
    m_comment = new CommentTableShape( table );
    m_pGrid = new GridTableShape( type );
    m_pLabel->SetId( 1000 );
    m_comment->SetId( 1001 );
    if( m_header && m_pLabel && m_comment && m_pGrid )
    {
        // set table header
        m_header->SetRelativePosition( 0, 1 );
        m_header->SetVAlign( wxSFShapeBase::valignTOP );
        m_header->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_header->SetDimensions( 1, 2 );
        m_header->SetFill( *wxTRANSPARENT_BRUSH );
        m_header->SetBorder( *wxTRANSPARENT_PEN);
        m_header->AcceptChild( wxT( "wxSFTextShape" ) );
        m_header->AcceptChild( wxT( "CommentTableShape" ) );
        m_header->Activate( false );
        SF_ADD_COMPONENT( m_header, wxT( "header" ) );
        //table name
        if( m_header->InsertToGrid( 0, 0, m_pLabel ) )
        {
            m_pLabel->SetVAlign( wxSFShapeBase::valignTOP );
            m_pLabel->GetFont().SetPointSize( 8 );
            m_pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
            m_pLabel->SetText( m_table->GetTableName() );
            m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            m_pLabel->Activate( false );
//            SF_ADD_COMPONENT( m_pLabel, wxT( "title" ) );
        }
        // table comment
        if( m_header->InsertToGrid( 0, 1, m_comment ) )
        {
            m_comment->SetVAlign( wxSFShapeBase::valignTOP );
            m_comment->SetHAlign( wxSFShapeBase::halignLEFT );
            m_comment->GetFont().SetPointSize( 8 );
            m_comment->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
            m_comment->SetText( m_table->GetComment() );
            m_comment->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            m_comment->Activate( false );
//            SF_ADD_COMPONENT( m_comment, wxT( "comment" ) );
        }
        // set grid
        m_pGrid->SetRelativePosition( 0, 17 );
        m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_pGrid->SetCellSpace( 8 );
        m_pGrid->SetDimensions( 1, 3 );
        m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
        m_pGrid->SetBorder( *wxTRANSPARENT_PEN);
        m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_pGrid->SetVBorder( 13 );
        m_pGrid->SetHBorder( 5 );
        m_pGrid->AcceptChild( wxT( "FieldShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFShapeBase" ) );
        m_pGrid->AcceptChild( wxT( "CommentFieldShape" ) );
        m_pGrid->Activate( false );
        SF_ADD_COMPONENT( m_pGrid, wxT( "main_grid" ) );
    }
}

MyErdTable::~MyErdTable()
{
}

void MyErdTable::UpdateTable()
{
    std::vector<Field *> fields = m_table->GetFields();
    int i = 0;
    ClearGrid();
    ClearConnections();
    SetRectSize( GetRectSize().x, 0 );
    ShapeList list;
    wxSFDiagramManager *manager = GetShapeManager();
    if( manager )
        manager->GetShapes( CLASSINFO( MyErdTable ), list );
    for( std::vector<Field *>::iterator it = fields.begin(); it < fields.end(); it++ )
    {
        AddColumn( (*it), i, (*it)->IsPrimaryKey() ? Constraint::primaryKey : (*it)->IsForeignKey() ? Constraint::foreignKey : Constraint::noKey );
        i += 3;
    }
    m_pGrid->Update();
    Update();
}

void MyErdTable::SetTableComment(const wxString &comment)
{
    m_table->SetComment( comment.ToStdWstring() );
    m_comment->SetText( m_table->GetComment() );
}

void MyErdTable::ClearGrid()
{
    m_pGrid->RemoveChildren();
    // re-initialize grid control
    m_pGrid->ClearGrid();
    m_pGrid->SetDimensions( 1, 3 );
    m_pGrid->SetCellSpace( 2 );
    Refresh();
}

void MyErdTable::ClearConnections()
{
}

void MyErdTable::DrawDetail(wxDC &dc)
{
    dc.SetPen( *wxWHITE_PEN );
    dc.SetBrush( *wxLIGHT_GREY_BRUSH );
    dc.DrawRectangle( Conv2Point( GetAbsolutePosition() + wxRealPoint( 1, m_nRadius ) ), Conv2Size( m_nRectSize - wxRealPoint( 2, 2 * m_nRadius - 2 ) ) );
}
/*
void MyErdTable::DrawHighlighted(wxDC &dc)
{
    wxSFRoundRectShape::DrawHighlighted( dc );
    DrawDetail( dc );
}
*/
void MyErdTable::DrawHover(wxDC &dc)
{
    wxSFRoundRectShape::DrawHover( dc );
    DrawDetail( dc );
}

void MyErdTable::DrawNormal(wxDC &dc)
{
/*    if( this->m_fSelected )
    {
        m_header->SetFill( wxBrush( wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) ) );
    }
    else
    {
        m_header->SetFill( wxBrush() );
    }*/
    wxSFRoundRectShape::DrawNormal( dc );
    DrawDetail( dc );
}

void MyErdTable::AddColumn(Field *field, int id, Constraint::constraintType type)
{
    if( m_type == DatabaseView )
    {
        if( type != Constraint::noKey )
        {
            // key bitmap
            wxSFBitmapShape* pBitmap = new wxSFBitmapShape();
            if( pBitmap )
            {
                pBitmap->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
                pBitmap->SetId( id + 10000 );
                pBitmap->Activate( true );
                pBitmap->RemoveStyle( sfsSHOW_HANDLES );
                if( m_pGrid->InsertToTableGrid( pBitmap ) )
                {
                    if( type == Constraint::primaryKey )
                    {
                        pBitmap->CreateFromXPM( key_p_xpm );
                    }
                    else
                        pBitmap->CreateFromXPM( key_f_xpm );
                    SetCommonProps( pBitmap );
                }
                else
                    delete pBitmap;
            }
        }
        else
        {
            // spacer
            wxSFShapeBase* pSpacer = new wxSFShapeBase();
            pSpacer->Activate( true );
            pSpacer->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            if( pSpacer )
            {
                pSpacer->SetId( id + 10000 );
                pSpacer->Activate( true );
                pSpacer->RemoveStyle( sfsSHOW_HANDLES );
                if( m_pGrid->InsertToTableGrid( pSpacer ) )
                {
                    SetCommonProps( pSpacer );
                }
                else
                    delete pSpacer;
            }
        }
        // label
        FieldShape *pCol = new FieldShape();
        if( pCol )
        {
            pCol->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            pCol->SetId( id + 10000 + 1 );
            pCol->Activate( true );
            pCol->RemoveStyle( sfsSHOW_HANDLES );
            if( m_pGrid->InsertToTableGrid( pCol ) )
            {
                SetCommonProps( pCol );
                pCol->GetFont().SetPointSize( 8 );
                pCol->SetText( field->GetFieldName() );
                pCol->SetField( field );
            }
            else
                delete pCol;
        }
        CommentFieldShape *comment_shape = new CommentFieldShape();
        if( comment_shape )
        {
            comment_shape->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            comment_shape->SetId( id + 10000 + 2 );
            comment_shape->Activate( true );
            comment_shape->RemoveStyle( sfsSHOW_HANDLES );
            if( m_pGrid->InsertToTableGrid( comment_shape ) )
            {
                SetCommonProps( comment_shape );
                comment_shape->SetField( field );
                comment_shape->GetFont().SetPointSize( 8 );
                comment_shape->SetText( field->GetComment() );
            }
            else
                delete comment_shape;
        }
    }
    else
    {
        // label
        FieldShape *pCol = new FieldShape();
        if( pCol )
        {
            pCol->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            pCol->SetId( id + 10000 );
            pCol->Activate( true );
            pCol->RemoveStyle( sfsSHOW_HANDLES );
            if( m_pGrid->InsertToTableGrid( pCol ) )
            {
                SetCommonProps( pCol );
                pCol->GetFont().SetPointSize( 8 );
                pCol->SetText( field->GetFieldName() );
                pCol->SetField( field );
            }
            else
                delete pCol;
        }
        wxSFTextShape *type_shape = new wxSFTextShape();
        if( type_shape )
        {
            type_shape->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            type_shape->SetId( id + 10000 + 1 );
            type_shape->Activate( true );
            type_shape->RemoveStyle( sfsSHOW_HANDLES );
            if( m_pGrid->InsertToTableGrid( type_shape ) )
            {
                SetCommonProps( type_shape );
                type_shape->GetFont().SetPointSize( 8 );
                type_shape->SetText( field->GetFullType() );
            }
            else
                delete type_shape;
        }
        CommentFieldShape *comment_shape = new CommentFieldShape();
        if( comment_shape )
        {
            comment_shape->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
            comment_shape->SetId( id + 10000 + 2 );
            comment_shape->Activate( true );
            comment_shape->RemoveStyle( sfsSHOW_HANDLES );
            if( m_pGrid->InsertToTableGrid( comment_shape ) )
            {
                SetCommonProps( comment_shape );
                comment_shape->GetFont().SetPointSize( 8 );
                comment_shape->SetText( field->GetComment() );
            }
            else
                delete comment_shape;
        }
    }
}

void MyErdTable::SetCommonProps(wxSFShapeBase* shape)
{
    shape->EnableSerialization( false );
    shape->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
    shape->SetHAlign( wxSFShapeBase::halignLEFT );
    shape->SetVAlign( wxSFShapeBase::valignMIDDLE );
    shape->SetVBorder( 0 );
    shape->SetHBorder( 0 );
}

const DatabaseTable &MyErdTable::GetTable()
{
    return *m_table;
}

wxSFTextShape *MyErdTable::GetLabel()
{
    return m_pLabel;
}

GridTableShape *MyErdTable::GetFieldGrid()
{
    return m_pGrid;
}

std::wstring &MyErdTable::GetTableName()
{
    return const_cast<std::wstring &>( m_table->GetTableName() );
}

void MyErdTable::DrawSelected(wxDC& dc)
{
    if( m_type == DatabaseView )
        wxSFRoundRectShape::DrawSelected( dc );
}