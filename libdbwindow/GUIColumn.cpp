#include "wx/wx.h"
#include "column.h"
#include "GUIColumn.h"

XS_IMPLEMENT_CLONABLE_CLASS(GUIColumn,xsSerializable);

GUIColumn::GUIColumn()
{
    m_name = wxEmptyString;
    m_type = wxEmptyString;
    m_size = 0;
    m_decimal = 0;
    m_isNotNull = false;
    m_isPK = false;
    m_isAutoInc = false;
    XS_SERIALIZE( m_name, wxT( "m_name" ) );
    XS_SERIALIZE( m_type, wxT( "m_type" ) );
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
}

GUIColumn::GUIColumn(const wxString &name, const wxString &type, long propertyflags, long size, long decimal)
{
    m_name = name;
    m_type = type;
    m_size = size;
    m_decimal = decimal;
    if( propertyflags & dbtPRIMARY_KEY )
        m_isPK = true;
	else
        m_isPK = false;
    if( propertyflags & dbtAUTO_INCREMENT )
        m_isAutoInc = true;
	else
        m_isAutoInc = false;
    XS_SERIALIZE( m_name, wxT( "m_name" ) );
    XS_SERIALIZE( m_type, wxT( "m_type" ) );
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
}

const wxString &GUIColumn::GetName()
{
    return m_name;
}

bool GUIColumn::IsPrimaryKey()
{
    return m_isPK;
}
