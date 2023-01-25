#include "wxsf/TextShape.h"
#include "database.h"
#include "commenttableshape.h"

XS_IMPLEMENT_CLONABLE_CLASS(CommentTableShape, wxSFTextShape);

CommentTableShape::CommentTableShape(void) : wxSFTextShape()
{
    m_comment = wxEmptyString;
    XS_SERIALIZE( m_comment, "table_comment" );
}

CommentTableShape::CommentTableShape(DatabaseTable *table) : wxSFTextShape()
{
    m_comment = table->GetTableProperties().m_comment;
    XS_SERIALIZE( m_comment, "table_comment" );
}

CommentTableShape::~CommentTableShape(void)
{
}

const wxString &CommentTableShape::GetDatabaseComment()
{
    return m_comment;
}

void CommentTableShape::SetDatabaseComment(const wxString &table)
{
    m_comment = table;
    SetText( m_comment );
}
