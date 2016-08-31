#include "wx/wx.h"
#include "column.h"
#include "GUIColumn.h"

GUIColumn::GUIColumn(const wxString &name, const wxString &type, long propertyflags, long size, long decimal/*, UNIVERSAL_TYPE type*/)
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
//	XS_SERIALIZE_LONG(m_dbtPropertyFlags,wxT("m_dbtPropertyFlags"));
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
//    XS_SERIALIZE(m_unique, wxT("m_unique"));
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
//	XS_SERIALIZE_LONG(m_universalType, wxT("m_universalType"));
}

wxString &GUIColumn::GetName()
{
    return m_name;
}
