#pragma once
class GUIColumn : public xsSerializable
{
protected:
    wxString m_name, m_type;
    long m_size, m_decimal;
    bool m_isNotNull, m_isPK, m_isAutoInc;
public:
    XS_DECLARE_CLONABLE_CLASS(GUIColumn);
    enum PROPERTY
    {
        /*! \brief Enable parameter Not NULL */
        dbtNOT_NULL = 1,
        /*! \brief Enable parameter SIZE*/
        dbtSIZE = 2,
        /*! \brief Enable parameter second SIZE */
        dbtSIZE_TWO = 4,
        /*! \brief Enable parameter PRIMARY KEY */
        dbtPRIMARY_KEY = 8,
        /*! \brief Enable parameter UNIQUE */
        dbtUNIQUE = 16,
        /*! \brief Enable parameter AUTO_INCREMENT */
        dbtAUTO_INCREMENT = 32		
    };
	
    enum UNIVERSAL_TYPE
    {
        dbtTYPE_INT = 1,
        dbtTYPE_FLOAT = 2,
        dbtTYPE_DECIMAL = 3,
        dbtTYPE_TEXT = 4,
        dbtTYPE_DATE_TIME = 5,
        dbtTYPE_BOOLEAN = 6,
        dbtTYPE_OTHER = 7
    };
    GUIColumn();
    GUIColumn(const wxString &name, const wxString &type, long propertyflags, long size, long decimal);
    virtual ~GUIColumn();
    const wxString &GetName();
    bool IsPrimaryKey();
};
