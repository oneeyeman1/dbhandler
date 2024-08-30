#pragma once

struct Object
{
    wxDateTime m_compiled, m_modifiied;
    int m_size;
    wxString m_name, m_comment, m_libName, m_checkedOutBy;
    Object(const wxString &libName, const wxString &name, const wxDateTime &created, const wxDateTime &modified, const wxString &comment) : m_libName( libName ), m_name( name ), m_compiled( created ), m_modifiied( modified ), m_comment( comment ) { }
};

struct LibraryObject
{
    wxDateTime m_created;
    wxString m_comment, m_name;
    std::vector<Object> m_objects;
};
