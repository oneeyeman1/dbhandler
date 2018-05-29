//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// copyright            : (C) 2014 Eran Ifrah
// file name            : constraint.h
//
// -------------------------------------------------------------------------
// A
//              _____           _      _     _ _
//             /  __ \         | |    | |   (_) |
//             | /  \/ ___   __| | ___| |    _| |_ ___
//             | |    / _ \ / _  |/ _ \ |   | | __/ _ )
//             | \__/\ (_) | (_| |  __/ |___| | ||  __/
//              \____/\___/ \__,_|\___\_____/_|\__\___|
//
//                                                  F i l e
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef CONSTRAINT_H
#define CONSTRAINT_H

#if !defined __DATABASEVIEW__H && !defined __GRIDTABLESHAPE_H__
enum ViewType
{
    DatabaseView,
    QueryView
};
#endif

class Constraint : public xsSerializable
{
public:
    /*! \brief Constraint type enum */
    enum constraintType
    {
        primaryKey,
        foreignKey,
        noKey
    };

    /*! \brief Constraint action enum */
    enum constraintAction
    {
        restrict,
        cascade,
        setNull,
        noAction,
        setDefault
    };

	/*! \brief Set local column name */
    virtual void SetLocalColumn(const wxString & localColumn)
    {
    }
	
    /*! \brief Set constraint name  */
    void SetName(const wxString & name)
    {
        m_name = name;
    }
	
    virtual void GetLocalColumn(wxString &) const
    {
    }

    virtual void GetLocalColumn(std::vector<std::wstring> &) const
    {
    }
	
    /*! \brief Get constraint name */
    const wxString & GetName() const
    {
        return m_name;
    }
	
    /*! \brief Set reference column name */
    virtual void SetRefCol(const wxString & refCol)
    {
    }

    virtual const void GetRefCol(wxString &refCol) const
    {
    }

    virtual const void GetRefCol(std::vector<std::wstring> &refCol) const
    {
    }

	/*! \brief Set constraint type */
    void SetType(constraintType type)
    {
        this->m_type = type;
    }
	
    /*! \brief Get constraint type */
    constraintType GetType() const
    {
        return m_type;
    }

    const DatabaseTable *GetFKTable() const
    {
        return m_fkTable;
    }

    void SetFKDatabaseTable(const DatabaseTable *table)
    {
       m_fkTable = const_cast<DatabaseTable *>( table );
    }

    /*! \brief Set reference table name */
    void SetRefTable(const wxString & refTable)
    {
        this->m_refTable = refTable;
    }

    /*! \brief Get reference table name */
    const wxString & GetRefTable() const
    {
        return m_refTable;
    }
	
	/*! \brief Set action OnDelete */
    void SetOnDelete(constraintAction onDelete)
    {
        this->m_onDelete = onDelete;
    }
	
    /*! \brief Get action OnDelete */
    constraintAction GetOnDelete() const
    {
        return m_onDelete;
    }
	
    /*! \brief Set action OnUpdate */
    void SetOnUpdate(constraintAction onUpdate)
    {
        this->m_onUpdate = onUpdate;
    }

    /*! \brief Get action OnUpdate */
    constraintAction GetOnUpdate() const
    {
        return m_onUpdate;
    }
	
    void SetPGMatch(int match)
    {
        m_match = match;
    }

    int GetPGMatch()
    {
        return m_match;
    }

private:
    wxString m_refTable, m_name;
    DatabaseTable *m_fkTable;
    constraintType m_type;
    constraintAction m_onUpdate, m_onDelete;
    int m_match;
protected:
    ViewType m_viewType;
};

class DatabaseConstraint : public Constraint
{
public:
    DatabaseConstraint(std::wstring fkName);
	virtual ~DatabaseConstraint();
    /*! \brief Set local column name */
    virtual void SetLocalColumn(const wxString &localColumn)
    {
        this->m_fkColumns.push_back( localColumn.ToStdWstring() );
    }
	
    /*! \brief Get local column name */
    virtual void GetLocalColumn(std::vector<std::wstring> &column) const
    {
        column = m_fkColumns;
    }
	
    /*! \brief Set reference column name */
    virtual void SetRefCol(const wxString &refCol)
    {
        this->m_refColumns.push_back( refCol.ToStdWstring() );
    }

    /*! \brief Get reference column name */
    virtual const void GetRefCol(std::vector<std::wstring> &refCol) const
    {
        refCol = m_refColumns;
    }

private:
    std::vector<std::wstring> m_fkColumns, m_refColumns;
};

/*! \brief Class representing one table constraint. */
class QueryConstraint : public Constraint
{
public:
    XS_DECLARE_CLONABLE_CLASS(Constraint);
    /*! \brief Default constructors */
    QueryConstraint();

    QueryConstraint(ViewType type);

    QueryConstraint(const QueryConstraint& obj);

    QueryConstraint(const wxString& name, const wxString &localColumn, constraintType type, constraintAction onDelete, constraintAction onUpdate);
    /*! \brief Default destructors */
    virtual ~QueryConstraint();
	
    /*! \brief Set local column name */
    virtual void SetLocalColumn(const wxString & localColumn)
    {
        this->m_localColumn = localColumn;
    }
	
    /*! \brief Set reference column name */
    virtual void SetRefCol(const wxString & refCol)
    {
        this->m_refCol = refCol;
    }

    void SetSign(int sign)
    {
        m_sign = sign;
    }
	
    /*! \brief Get local column name */
    virtual void GetLocalColumn(wxString &column) const
    {
        column = m_localColumn;
    }
	
    /*! \brief Get reference column name */
    virtual const void GetRefCol(wxString &refCol) const
    {
        refCol = m_refCol;
    }

    int GetSign() const
    {
        return m_sign;
    }

protected:
    wxString m_localColumn;
    wxString m_refCol;
    int m_sign;
//    ConstraintSign *m_sign;
    void InitSerializable();
};

#endif // CONSTRAINT_H
