#pragma once

struct LibraryProperty
{
    wxDateTime m_created;
    wxString m_comment, m_name;
};

struct LibraryObjectProperty
{
    wxDateTime m_compiled, m_modifiied;
    int m_size;
    wxString m_name, m_comment, m_libName, m_checkedOutBy;

};

struct LibraryObjects
{
public:
    LibraryObjects(const wxString &libName, const wxString &name, const wxDateTime &created, const wxDateTime &modified, int size, const wxString &comment)
    {
        m_properties.m_libName   = libName;
        m_properties.m_name      = name;
        m_properties.m_compiled  = created;
        m_properties.m_modifiied = modified;
        m_properties.m_size      = size;
        m_properties.m_comment   = comment; 
    }

    LibraryObjects()
    {
        m_properties.m_compiled     = wxDateTime::Now();
        m_properties.m_modifiied    = wxDateTime::Now();
        m_properties.m_size         = 0;
        m_properties.m_name         = "";
        m_properties.m_comment      = "";
        m_properties.m_libName      = "";
        m_properties.m_checkedOutBy = "";
    }

    LibraryObjectProperty m_properties;
};

struct LibraryObject
{
public:
    LibraryObject(const wxString &name, const wxDateTime &timestamp, const wxString &comment)
    {
        m_prop.m_name = name;
        auto stamp = timestamp.GetTm();
        m_prop.m_created.SetDay( stamp.mday );
        m_prop.m_created.SetMonth( stamp.mon );
        m_prop.m_created.SetYear( stamp.year );
        m_prop.m_created.SetHour( stamp.hour );
        m_prop.m_created.SetMinute( stamp.min );
        m_prop.m_created.SetSecond( stamp.sec );
        m_prop.m_created.SetMillisecond( stamp.msec );
        m_prop.m_comment = comment;
    }

    LibraryObject()
    {
        m_prop.m_created = wxDateTime::Now();
        m_prop.m_name = wxEmptyString;
        m_prop.m_comment = wxEmptyString;
    }

    const wxString &GetLibraryName() const { return m_prop.m_name; }
    void SetLibraryName(const wxString &libName) { m_prop.m_name = libName; }
    const wxDateTime &GetCreationTime() const { return m_prop.m_created; }
    void SetCreationTime(const wxDateTime &timeStamp)
    {
        auto stamp = timeStamp.GetTm();
        m_prop.m_created.SetDay( stamp.mday );
        m_prop.m_created.SetMonth( stamp.mon );
        m_prop.m_created.SetYear( stamp.year );
        m_prop.m_created.SetHour( stamp.hour );
        m_prop.m_created.SetMinute( stamp.min );
        m_prop.m_created.SetSecond( stamp.sec );
        m_prop.m_created.SetMillisecond( stamp.msec );
    }
    void SetCreationTime(const wxString &dt)
    {
        wxDateTime timeStamp;
        auto res = timeStamp.ParseISOCombined( dt );
        if( res )
            SetCreationTime( timeStamp );
    }
    const wxString &GetComment() const { return m_prop.m_comment; }
    void SetComment(const wxString &comment) { m_prop.m_comment = comment; }
    const LibraryProperty &GetProperties() { return m_prop; }
    std::vector<LibraryObjects> &GetObjects() { return  m_objects; }
private:
    LibraryProperty m_prop;
    std::vector<LibraryObjects> m_objects;
};
