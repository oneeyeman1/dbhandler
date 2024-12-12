#include "wxsf/TextShape.h"
#include "wxsf/FlexGridShape.h"
#include "database.h"
#include "configuration.h"
#include "guiobjectsproperties.h"
#include "propertieshandlerbase.h"
#include "GridTableShape.h"
#include "commentfieldshape.h"
#include "fieldtypeshape.h"
#include "FieldShape.h"

XS_IMPLEMENT_CLONABLE_CLASS(FieldShape,wxSFTextShape);

FieldShape::FieldShape() : wxSFTextShape()
{
    m_comment = nullptr;
    m_object = DatabaseFieldPropertiesType;
    m_typeShape = nullptr;
    SetHAlign(wxSFShapeBase::halignCENTER );
    SetVAlign(wxSFShapeBase::valignMIDDLE );
    m_name = wxEmptyString;
    m_type = wxEmptyString;
    m_size = 0;
    m_decimal = 0;
    m_isNotNull = false;
    m_isPK = false;
    m_isAutoInc = false;
    m_backColour = wxColour( 210, 225, 245 );
    XS_SERIALIZE( m_name, wxT( "m_name" ) );
    XS_SERIALIZE( m_type, wxT( "m_type" ) );
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
    XS_SERIALIZE( m_backColour, wxT( "m_backColour" ) );
}

FieldShape::FieldShape(const TableField *field) : wxSFTextShape()
{
    m_comment = nullptr;
    m_object = DatabaseFieldPropertiesType;
    SetHAlign(wxSFShapeBase::halignCENTER );
    SetVAlign(wxSFShapeBase::valignMIDDLE );
    m_name = field->GetFieldName();
    m_type = field->GetFieldType();
    m_size = field->GetFieldSize();
    m_decimal = field->GetPrecision();
    m_isPK = field->IsPrimaryKey();
    m_isNotNull = field->IsNullAllowed();
//    m_isAutoInc = field->Is
    XS_SERIALIZE( m_name, wxT( "m_name" ) );
    XS_SERIALIZE( m_type, wxT( "m_type" ) );
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
    XS_SERIALIZE( m_backColour, wxT( "m_backColour" ) );
}

FieldShape::FieldShape(const FieldShape &shape)
{
    m_field = shape.m_field;
    m_parentRect = shape.m_parentRect;
    m_backColour = shape.m_backColour;
    m_isNotNull = shape.m_isNotNull;
    m_isPK = shape.m_isPK;
    m_isAutoInc = shape.m_isAutoInc;
    m_size = shape.m_size;
    m_decimal = shape.m_decimal;
    m_name = shape.m_name;
    m_type = shape.m_type;
    RemoveStyle( sfsSHOW_HANDLES );
}

FieldShape::~FieldShape(void)
{
}

void FieldShape::DrawNormal(wxDC &dc)
{
    wxRect rect = this->GetBoundingBox();
    rect.x = GetParentShape()->GetParentShape()->GetBoundingBox().GetLeft();
    rect.width = GetParentShape()->GetParentShape()->GetBoundingBox().GetWidth();
/*    if( m_typeShape && !m_typeShape->GetText().IsEmpty() )
    {
        rect.width = m_typeShape->GetBoundingBox().GetLeft();
    }
    else if( m_comment && !m_comment->GetText().IsEmpty() )
    {
        rect.width = m_comment->GetBoundingBox().GetLeft();
    }
    else
        rect.width = GetParentShape()->GetParentShape()->GetBoundingBox().GetWidth();
    wxSFShapeBase *parentShape = GetParentShape()->GetParentShape();
    wxRect rectParent = parentShape->GetBoundingBox();
    m_parentRect.x = rectParent.x;
    m_parentRect.width = rectParent.width;*/
    wxString line;
    int i = 0;
    if( this->m_fSelected )
    {
        m_backColour = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT );
        dc.SetBrush( m_backColour );
        if( m_typeShape )
            m_typeShape->Select( true );
        if( m_comment )
            m_comment->Select( true );
    }
    else
    {
        m_backColour = wxColour( 210, 225, 245 );
        dc.SetBrush( m_Fill );
        dc.SetPen( m_Border );
        dc.SetBackgroundMode( wxTRANSPARENT );
        if( m_typeShape )
            m_typeShape->Select( false );
        if( m_comment )
            m_comment->Select( false );
    }
    dc.DrawRectangle( rect.x, rect.y, rect.width + sfdvGRIDSHAPE_CELLSPACE * 3, rect.height );
    dc.SetFont( m_Font );
    wxRealPoint pos = GetAbsolutePosition();
    // draw all text lines
    wxStringTokenizer tokens( m_sText, wxT("\n\r"), wxTOKEN_RET_EMPTY );
    while( tokens.HasMoreTokens() )
    {
        line = tokens.GetNextToken();
        dc.DrawText( line, (int )pos.x, (int) pos.y + i * 12 );
        i++;
    }
    dc.SetPen( wxNullPen );
    dc.SetFont( wxNullFont );
    dc.SetBrush( wxNullBrush );
}

void FieldShape::SetParentRect(const wxRect &rect)
{
    m_parentRect = rect;
}

void FieldShape::SetField(TableField *field)
{
    m_field = field;
    FieldTableProperties prop;
    prop.m_comment = field->GetFieldProperties().m_comment;
    prop.m_heading.m_label = field->GetFieldProperties().m_heading.m_label;
    prop.m_heading.m_heading = field->GetFieldProperties().m_heading.m_heading;
    prop.m_heading.m_labelAlignment = field->GetFieldProperties().m_heading.m_labelAlignment;
    prop.m_heading.m_headingAlignment = field->GetFieldProperties().m_heading.m_headingAlignment;
    for( std::map<int, std::vector<std::pair<std::wstring,std::wstring> > >::iterator it = field->GetFieldProperties().m_display.m_format.begin(); it != field->GetFieldProperties().m_display.m_format.end(); ++it )
        for( std::vector<std::pair<std::wstring,std::wstring> >::iterator it1 = (*it).second.begin(); it1 != (*it).second.end(); ++it1 )
            prop.m_display.m_format[(*it).first].push_back( (*it1) );
    m_any = prop;
}

TableField *FieldShape::GetField()
{
    return m_field;
}
/*
bool FieldShape::Contains(const wxPoint& pos)
{
    bool result = false;
    wxSFShapeBase *parent = GetParentShape();
    wxRect rectParent = parent->GetBoundingBox();
    wxRect rect = GetBoundingBox();
    if( pos.y >= rect.GetTop() && pos.y < rect.GetBottom() && pos.x >= rectParent.GetLeft() && pos.x <= rectParent.GetRight() )
        result = true;
    return result;
}
*/
void FieldShape::DrawSelected(wxDC& dc)
{
    DrawNormal( dc );
}

void FieldShape::Select(bool state)
{
    m_fSelected = state;
}

/*wxRect FieldShape::GetBoundingBox()
{
    wxRect rect = wxSFRectShape::GetBoundingBox();
    wxRect parentRect = this->GetParentShape()->GetParentShape()->GetBoundingBox();
    rect.SetLeft( parentRect.GetLeft() );
    if( m_typeShape )
    {
        rect.width = m_typeShape->GetBoundingBox().GetLeft();
    }
    else if( m_comment && !m_comment->GetText().IsEmpty() )
    {
        rect.width = m_comment->GetBoundingBox().GetLeft();
    }
    else
    {
        rect.width = parentRect.GetRight();
    }
    return rect;
}
*/
int FieldShape::ApplyProperties(const wxAny &any, bool logOnly, std::wstring &command)
{
    return 0;
}

wxAny &FieldShape::GetProperties()
{
    return m_any;
}

void FieldShape::SetProperties(FieldProperties prop)
{
    m_any = prop;
}
