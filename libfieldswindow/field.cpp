#include "wxsf/RectShape.h"
#include "wxsf/DiagramManager.h"
#include "wxsf/TextShape.h"
#include "field.h"

FieldWin::FieldWin(const wxRealPoint &pt, const wxString &name, wxSFDiagramManager manager) : wxSFRectShape( pt, wxRealPoint( 0, 0 ), &manager )
{
    AcceptChild( wxT("wxSFTextShape") );
    AddStyle( sfsLOCK_CHILDREN );
    m_text = new wxSFTextShape();
    m_text->SetStyle( sfsALWAYS_INSIDE | sfsPROCESS_DEL |sfsPROPAGATE_DRAGGING | sfsPROPAGATE_SELECTION | sfsLOCK_CHILDREN );
    AddChild( m_text );
    m_text->GetFont().SetPointSize( 10 );
    m_text->SetText( name );
    m_text->SetHBorder( 5.0 );
    m_text->SetVBorder( 5.0 );
    m_fieldName = name;
    Activate( true );
    m_text->Activate( false );
}

FieldWin::~FieldWin(void)
{
}

const wxString &FieldWin::GetFieldName() const
{
    return m_fieldName;
}

void FieldWin::SetFieldName(const wxString &name)
{
    m_text->SetText( name );
    m_fieldName = name;
}
