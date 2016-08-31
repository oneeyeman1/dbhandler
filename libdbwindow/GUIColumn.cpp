#include "wx/wx.h"
#include "column.h"
#include "GUIColumn.h"

GUIColumn::GUIColumn(const wxString &name, long propertyflags/*, UNIVERSAL_TYPE type*/)
{
    m_typeName = name;
//    m_parentName = parentName;
    if( propertyflags & dbtPRIMARY_KEY )
        m_isPK = true;
	else
        m_isPK = false;
    if( propertyflags & dbtAUTO_INCREMENT )
        m_isAutoInc = true;
	else
        m_isAutoInc = false;
    XS_SERIALIZE( m_typeName, wxT( "m_typeName" ) );
//	XS_SERIALIZE_LONG(m_dbtPropertyFlags,wxT("m_dbtPropertyFlags"));
    XS_SERIALIZE_LONG( m_size, wxT( "m_size" ) );
    XS_SERIALIZE_LONG( m_decimal, wxT( "m_decimal" ) );
//    XS_SERIALIZE(m_unique, wxT("m_unique"));
    XS_SERIALIZE( m_isPK, wxT( "m_isPK" ) );
    XS_SERIALIZE( m_isNotNull, wxT( "m_notNull" ) );
    XS_SERIALIZE( m_isAutoInc, wxT( "m_isAutoInc" ) );
//	XS_SERIALIZE_LONG(m_universalType, wxT("m_universalType"));
}
