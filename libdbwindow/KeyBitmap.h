#pragma once

class KeyBitmap : public wxSFBitmapShape
{
public:
    KeyBitmap(void);
    virtual ~KeyBitmap(void);
    virtual wxRect GetBoundingBox();
};

