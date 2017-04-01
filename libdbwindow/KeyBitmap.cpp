#include "wxsf/BitmapShape.h"
#include "wxsf/GridShape.h"
#include "GridTableShape.h"
#include "KeyBitmap.h"

KeyBitmap::KeyBitmap(void)
{
}

KeyBitmap::~KeyBitmap(void)
{
}

wxRect KeyBitmap::GetBoundingBox()
{
    wxRect rectParent = dynamic_cast<GridTableShape *>( GetParent() )->GetBoundingBox();
    wxRect rect = wxSFBitmapShape::GetBoundingBox();
    rect.SetX( rectParent.GetX() );
    rect.SetWidth( rectParent.GetWidth() );
    return rect;
}
