// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"
#include "wxsf/ShapeCanvas.h"
#include "wxsf/RoundRectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/BitmapShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "constraintsign.h"
#include "constraint.h"
#include "commenttableshape.h"
#include "MyErdTable.h"
#include "fieldwindow.h"
#include "ErdForeignKey.h"

XS_IMPLEMENT_CLONABLE_CLASS(ErdForeignKey,wxSFRoundOrthoLineShape);

ErdForeignKey::ErdForeignKey():ErdLineShape()
{
    m_pConstraint = NULL;
	
    EnableSerialization( false );
}

ErdForeignKey::ErdForeignKey(Constraint* pConstraint, ViewType type):ErdLineShape( pConstraint, type )
{
    m_pConstraint = pConstraint;
    wxSFTextShape* pLabel = new wxSFTextShape();
    if( pLabel )
    {
        pLabel->GetFont().SetPointSize( 8 );
        pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
        pLabel->SetText( pConstraint->GetName() );
        pLabel->SetVAlign( valignMIDDLE );
        pLabel->SetHAlign( halignCENTER );
        pLabel->SetFill( *wxTRANSPARENT_BRUSH );
        pLabel->SetStyle( sfsLOCK_CHILDREN | sfsPARENT_CHANGE );
        pLabel->RemoveStyle( sfsPARENT_CHANGE );
        pLabel->RemoveStyle( sfsSHOW_HANDLES );
        AddChild(pLabel);
    }
    SetTrgArrow( CLASSINFO( OneArrow ) );
    SetSrcArrow( CLASSINFO( NArrow ) );
    SetDockPoint( sfdvLINESHAPE_DOCKPOINT_CENTER );
    EnableSerialization( false );
}

ErdForeignKey::ErdForeignKey(const ErdForeignKey& obj):ErdLineShape(obj)
{
    m_pConstraint = obj.m_pConstraint;
}

ErdForeignKey::~ErdForeignKey()
{
    delete m_pConstraint;
    m_pConstraint = NULL;
}

void ErdForeignKey::CreateHandles()
{
    wxSFRoundOrthoLineShape::CreateHandles();
    RemoveHandle( wxSFShapeHandle::hndLINESTART );
    RemoveHandle( wxSFShapeHandle::hndLINEEND );
}
