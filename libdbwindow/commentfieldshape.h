#pragma once
class CommentFieldShape : public wxSFTextShape
{
public:
    CommentFieldShape(Field *field);
    virtual ~CommentFieldShape(void);
private:
    Field *m_field;
};

