#pragma once

struct LibraryObject
{
    wxDateTime m_created, m_modified;
    wxString m_comment, m_name, m_libName, m_checkedOutBy;
    int m_size;
};
