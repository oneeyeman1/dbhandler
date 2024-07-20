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
    NewViewView,
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

    /*! \brief Set constraint name  */
    void SetName(const wxString & name)
    {
        m_name = name;
    }
	
    /*! \brief Get constraint name */
    const wxString & GetName() const
    {
        return m_name;
    }
	
	/*! \brief Set local column name */
    void SetLocalColumn(const std::wstring &column)
    {
        m_localCol = column;
    }
	
    const std::wstring &GetLocalColumn() const
    {
        return m_localCol;
    }

    virtual void GetLocalColumns(std::vector<std::wstring> &) const
    {
        return;
    }
	
    virtual void SetLocalColumns(const std::vector<std::wstring> &)
    {
    }

    /*! \brief Set reference column name */
    void SetRefColumn(const std::wstring &column)
    {
        m_refCol = column;
    }

    /*! \brief Get reference column name */
    const std::wstring &GetRefColumn()
    {
        return m_refCol;
    }

    virtual void GetRefColumns(std::vector<std::wstring> &) const
    {
        return;
    }

    virtual void SetRefColumns(const std::vector<std::wstring> &)
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

    void SetFKDatabaseTable(const DatabaseTable *dbTable)
    {
       m_fkTable = const_cast<DatabaseTable *>( dbTable );
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
    std::wstring m_localCol, m_refCol;
protected:
    ViewType m_viewType;
};

class DatabaseConstraint : public Constraint
{
public:
    DatabaseConstraint(std::wstring fkName);
	virtual ~DatabaseConstraint();

    /*! \brief Set local column name */
    virtual void SetLocalColumns(const std::vector<std::wstring> &columns)
    {
        m_fkColumns = columns;
    }

    /*! \brief Get local column name */
    virtual void GetLocalColumns(std::vector<std::wstring> &temp) const
    {
        temp = m_fkColumns;
    }
	
    /*! \brief Get reference column name */
    virtual void GetRefColumns(std::vector<std::wstring> &temp) const
    {
        temp = m_refColumns;
    }

    virtual void SetRefColumns(const std::vector<std::wstring> &refCol)
    {
        m_refColumns = refCol;
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
	
    void SetSign(int sign)
    {
        m_sign = sign;
    }
	
    int GetSign() const
    {
        return m_sign;
    }

protected:
    wxString m_refCol;
    int m_sign;
//    ConstraintSign *m_sign;
    void InitSerializable();
};

#endif // CONSTRAINT_H
