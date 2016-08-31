#include "database.h"
#include "ErdTable.h"
#include "GUIColumn.h"
#include "GUIDatabaseTable.h"
#include "MyErdTable.h"
#include "res/gui/key-p.xpm"
#include "res/gui/key-f.xpm"

MyErdTable::MyErdTable(GUIDatabaseTable *table) : ErdTable( table )
{
//    SetUserData( table );
    SetFill( wxBrush( wxColour( *wxLIGHT_GREY ) ) );
    AcceptConnection(wxT("All"));
    AcceptTrgNeighbour(wxT("All"));
    AcceptSrcNeighbour(wxT("All"));
    AddStyle( sfsLOCK_CHILDREN );
    AddStyle( sfsSHOW_SHADOW );
    SetBorder( wxPen( wxColour( 70, 125, 170 ), 1, wxPENSTYLE_SOLID ) );
    SetFill( wxBrush( wxColour( *wxLIGHT_GREY ) ) );
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
        m_pLabel->SetText( wxT( "DBETable name" ) );
        m_pLabel->SetStyle( sfsHOVERING | sfsALWAYS_INSIDE | sfsPROCESS_DEL | sfsEMIT_EVENTS |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION );
        SF_ADD_COMPONENT( m_pLabel, wxT( "title" ) );
        m_pGrid->SetRelativePosition( 0, 17 );
        m_pGrid->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION  );
        m_pGrid->SetDimensions( 1, 2 );
        m_pGrid->SetFill( *wxTRANSPARENT_BRUSH );
        m_pGrid->SetBorder( *wxTRANSPARENT_PEN );
        m_pGrid->SetHAlign( wxSFShapeBase::halignLEFT );
        m_pGrid->SetVBorder( 13 );
        m_pGrid->SetHBorder( 5 );
        m_pGrid->AcceptChild( wxT( "wxSFTextShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFBitmapShape" ) );
        m_pGrid->AcceptChild( wxT( "wxSFShapeBase" ) );
        m_pGrid->Activate( false );
        SF_ADD_COMPONENT( m_pGrid, wxT( "main_grid" ) );
    }
}

void MyErdTable::UpdateColumn()
{
    ClearGrid();
    ClearConnections();
	SetRectSize( GetRectSize().x, 0 );
    int i = 0;
	Constraint::constraintType type;
    GUIDatabaseTable *table = (GUIDatabaseTable *) wxDynamicCast( GetUserData(), GUIDatabaseTable );
    if( table )
    {
        m_pLabel->SetText( table->GetName() );
        SerializableList::compatibility_iterator node = table->GetFirstChildNode();
        while( node )
        {
            GUIColumn *pCol = wxDynamicCast( node->GetData(), GUIColumn );
            if( pCol )
            {
                type = Constraint::noKey;
                SerializableList::compatibility_iterator cstrNode = table->GetFirstChildNode();
                while( cstrNode )
                {
                    Constraint *constraint = wxDynamicCast( cstrNode->GetData(), Constraint );
                    if( constraint )
                    {
                        if( constraint->GetLocalColumn() == pCol->GetName() ) 
                            type = constraint->GetType();
                    }
                    cstrNode = cstrNode->GetNext();
                }
                AddColumnShape( pCol->GetName(), i, type );
                i += 2;
            }
            node = node->GetNext();
        }
        node = table->GetFirstChildNode();
        while( node )
        {
            if( node->GetData()->IsKindOf( CLASSINFO( Constraint ) ) )
            {
            }
        }
    }
    m_pGrid->Update();
    Update();
}

void MyErdTable::AddColumnShape(const wxString& colName, int id, Constraint::constraintType type)
{
    if( type != Constraint::noKey )
    {
		// key bitmap
        wxSFBitmapShape* pBitmap = new wxSFBitmapShape();
        if( pBitmap )
        {
            pBitmap->SetId( id + 10000 );
            if( m_pGrid->AppendToGrid(pBitmap) )
            {
                if( type == Constraint::primaryKey )
                    pBitmap->CreateFromXPM( key_p_xpm );
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
        if( pSpacer )
        {
            pSpacer->SetId( id+10000 );
            if( m_pGrid->AppendToGrid( pSpacer ) )
                SetCommonProps( pSpacer );
            else
                delete pSpacer;
        }
    }
    wxSFTextShape* pCol = new wxSFTextShape();
    if( pCol )
    {
        pCol->SetId( id + 10000 + 1 );
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
