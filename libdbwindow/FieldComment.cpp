#include "wxsf/TextShape.h"
#include "wxsf/GridShape.h"
#include "GridTableShape.h"
#include "FieldComment.h"

FieldComment::FieldComment(void)
{
}

FieldComment::~FieldComment(void)
{
}

wxRect FieldComment::GetBoundingBox()
{
    wxRect rectParent = dynamic_cast<GridTableShape *>( GetParent() )->GetBoundingBox();
    wxRect rect = wxSFTextShape::GetBoundingBox();
    rect.SetX( rectParent.GetX() );
    rect.SetWidth( rectParent.GetWidth() );
    return rect;
}
