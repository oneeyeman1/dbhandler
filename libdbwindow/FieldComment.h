#pragma once
class FieldComment : public wxSFTextShape
{
public:
    FieldComment(void);
    virtual ~FieldComment(void);
    virtual wxRect GetBoundingBox();
};

