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
#include "configuration.h"
#include "guiobjectsproperties.h"
#include "propertieshandlerbase.h"
#include "GridTableShape.h"
#include "HeaderGrid.h"
#include "constraint.h"
#include "constraintsign.h"
#include "nametableshape.h"
#include "commenttableshape.h"
#include "MyErdTable.h"
#include "field.h"
#include "fieldwindow.h"
#include "ErdForeignKey.h"

XS_IMPLEMENT_CLONABLE_CLASS(ErdForeignKey,wxSFRoundOrthoLineShape);

ErdForeignKey::ErdForeignKey():ErdLineShape()
{
    m_pConstraint = NULL;
}

ErdForeignKey::ErdForeignKey(Constraint* pConstraint, ViewType type, const wxSFDiagramManager &pManager):ErdLineShape( pConstraint, type, pManager )
{
    m_pConstraint = pConstraint;
    wxSFTextShape* pLabel = new wxSFTextShape();
    if( pLabel )
    {
        pLabel->GetFont().SetPointSize( 8 );
        pLabel->GetFont().SetWeight( wxFONTWEIGHT_BOLD );
        pLabel->SetText( "" );
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
}

ErdForeignKey::ErdForeignKey(const ErdForeignKey& obj):ErdLineShape(obj)
{
    m_pConstraint = obj.m_pConstraint;
}

ErdForeignKey::~ErdForeignKey()
{
}

void ErdForeignKey::CreateHandles()
{
    wxSFRoundOrthoLineShape::CreateHandles();
    RemoveHandle( wxSFShapeHandle::hndLINESTART );
    RemoveHandle( wxSFShapeHandle::hndLINEEND );
}
