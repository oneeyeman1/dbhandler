#pragma once
class FieldTypeShape : public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(FieldTypeShape);
    FieldTypeShape();
    FieldTypeShape(TableField *field);
    FieldTypeShape(FieldTypeShape &shape);
    virtual ~FieldTypeShape();
    const TableField *GetFieldForComment();
    void SetField(TableField *field);
protected:    
//    virtual void DrawNormal(wxDC &dc);
    virtual void DrawSelected(wxDC& dc);
private:
    TableField *m_field;
};

