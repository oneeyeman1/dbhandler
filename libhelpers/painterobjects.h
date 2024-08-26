#pragma once

struct Object
{
    wxDateTime m_created, m_modifed;
    int m_size;
    wxString m_name, m_comment, m_libName, m_checkedOutBy;
};

struct LibraryObject
{
    wxDateTime m_created, m_modified;
    wxString m_comment, m_name;
    std::vector<Object> m_objects;
};
