#include "wxsf/RectShape.h"
#include "wxsf/DiagramManager.h"
#include "wxsf/TextShape.h"
#include "field.h"

FieldWin::FieldWin(const wxRealPoint &pt, const wxString &name, wxSFDiagramManager manager) : wxSFRectShape( pt, wxRealPoint( 0, 0 ), &manager )
{
    AcceptChild( wxT("wxSFTextShape") );
    m_text = new wxSFTextShape();
    AddChild( m_text );
    m_text->GetFont().SetPointSize( 10 );
    m_text->SetText( name );
    m_text->SetHBorder( 5.0 );
    m_text->SetVBorder( 5.0 );
    m_fieldName = name;
}

FieldWin::~FieldWin(void)
{
}

wxString &FieldWin::GetFieldName()
{
    return m_fieldName;
}