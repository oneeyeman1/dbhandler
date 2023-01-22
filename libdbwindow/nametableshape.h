#pragma once
class NameTableShape :  public wxSFTextShape
{
public:
    XS_DECLARE_CLONABLE_CLASS(NameTableShape);
    NameTableShape();
    NameTableShape(DatabaseTable *table);
    virtual ~NameTableShape() {};
private:
    DatabaseTable *m_table;
};

