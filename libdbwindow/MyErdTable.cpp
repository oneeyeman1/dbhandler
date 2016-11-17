#include <string>
#include "database.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/CommonFcn.h"
#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "wxsf/DiagramManager.h"
#include "constraint.h"
#include "GUIColumn.h"
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
    m_pLabel = new wxSFTextShape();
    m_pGrid = new wxSFFlexGridShape();
    if( m_pLabel && m_pGrid )
    {
        m_pLabel->SetVAlign( wxSFShapeBase::valignTOP );
        m_pLabel->SetHAlign( wxSFShapeBase::halignCENTER );
        m_pLabel->SetVBorder( 1 );
        m_pLabel->SetHBorder( 5 );
        m_pLabel->GetFont().SetPointSize( 8 );
        m_pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
        m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
        SF_ADD_COMPONENT( m_pLabel, wxT( "title" ) );
        // set grid
        m_pGrid->SetRelativePosition( 0, 17 );
        m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_pGrid->SetDimensions( 1, 2 );
        m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
        m_pGrid->SetBorder( *wxTRANSPARENT_PEN );
        m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_pGrid->SetVBorder( 13 );
        m_pGrid->SetHBorder( 5 );
        m_pGrid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFShapeBase" ) );
        m_pGrid->Activate( true );
        SF_ADD_COMPONENT( m_pGrid, wxT( "main_grid" ) );
	}
}

MyErdTable::MyErdTable(DatabaseTable *table) : wxSFRoundRectShape()
{
    m_table = table;
	std::vector<Field *> fields = m_table->GetFields();
    SetBorder( wxPen( wxColour( 70, 125, 170 ), 1, wxPENSTYLE_SOLID ) );
    SetFill( wxBrush( wxColour( 210, 225, 245 ) ) );
    AcceptConnection( wxT( "All" ) );
    AcceptTrgNeighbour( wxT( "All" ) );
    AcceptSrcNeighbour( wxT( "All" ) );
    AddStyle( sfsLOCK_CHILDREN );
    SetRadius(15);
    m_pLabel = new wxSFTextShape();
    m_pGrid = new wxSFFlexGridShape();
    if( m_pLabel && m_pGrid )
    {
        m_pLabel->SetVAlign( wxSFShapeBase::valignTOP );
        m_pLabel->SetHAlign( wxSFShapeBase::halignCENTER );
        m_pLabel->SetVBorder( 1 );
        m_pLabel->SetHBorder( 5 );
        m_pLabel->GetFont().SetPointSize( 8 );
        m_pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
		m_pLabel->SetText( m_table->GetTableName() );
        m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
        SF_ADD_COMPONENT( m_pLabel, wxT( "title" ) );
        m_pGrid->SetRelativePosition( 0, 17 );
        m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
        m_pGrid->SetDimensions( 1, 2 );
        m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
        m_pGrid->SetBorder( *wxTRANSPARENT_PEN);
        m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_pGrid->SetVBorder( 13 );
        m_pGrid->SetHBorder( 5 );
        m_pGrid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFShapeBase" ) );
        m_pGrid->Activate( true );
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
        long properties = 0;
        if( (*it)->IsPrimaryKey() )
            properties |= GUIColumn::dbtPRIMARY_KEY;
        if( (*it)->IsAutoIncrement() )
            properties |= GUIColumn::dbtAUTO_INCREMENT;
        GUIColumn *col = new GUIColumn( (*it)->GetFieldName(), (*it)->GetFieldType(), properties, (*it)->GetFieldSize(), (*it)->GetPrecision() );
        m_columns.push_back( col );
        AddColumn( col->GetName(), i, col->IsPrimaryKey() ? Constraint::primaryKey : Constraint::noKey );
        delete col;
        i += 2;
    }
    m_pGrid->Update();
    Update();
}

void MyErdTable::ClearGrid()
{
    m_pGrid->RemoveChildren();
    // re-initialize grid control
    m_pGrid->ClearGrid();
    m_pGrid->SetDimensions( 1, 2 );
    m_pGrid->SetCellSpace( 2 );
    Refresh();
}

void MyErdTable::ClearConnections()
{
}

void MyErdTable::AddColumn(GUIColumn *col)
{
	m_columns.push_back( col );
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
    wxSFRoundRectShape::DrawNormal( dc );
    DrawDetail( dc );
}

void MyErdTable::AddColumn(const wxString &colName, int id, Constraint::constraintType type)
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
			if( m_pGrid->AppendToGrid( pBitmap ) )
			{
				if( type == Constraint::primaryKey )
				{
					pBitmap->CreateFromXPM( key_p_xpm );
				}
				else
					pBitmap->CreateFromXPM( key_f_xpm );
				
				SetCommonProps(pBitmap);
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
			if( m_pGrid->AppendToGrid( pSpacer ) )
			{
				SetCommonProps( pSpacer );
			}
			else
				delete pSpacer;
		}
	}
	
    // label
    wxSFTextShape *pCol = new wxSFTextShape();
    if( pCol )
    {
        pCol->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
        pCol->SetId( id + 10000 + 1 );
        pCol->Activate( true );
        if( m_pGrid->AppendToGrid( pCol ) )
        {
            SetCommonProps( pCol );
            pCol->GetFont().SetPointSize( 8 );
            pCol->SetText( colName );
        }
        else
            delete pCol;
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

wxSFFlexGridShape *MyErdTable::GetFieldGrid()
{
    return m_pGrid;
}

std::wstring &MyErdTable::GetTableName()
{
	return const_cast<std::wstring &>( m_table->GetTableName() );
}
