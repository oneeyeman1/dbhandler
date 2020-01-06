#pragma once
class FieldTypeShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldTypeShape);
    FieldTypeShape();
    FieldTypeShape(Field *field);
    FieldTypeShape(FieldTypeShape &shape);
    virtual ~FieldTypeShape();
    const Field *GetFieldForComment();
    void SetField(Field *field);
private:
    Field *m_field;
};

