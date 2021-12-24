#pragma once

class QueryArguments
{
public:
    int m_pos;
    wxString m_name, m_type;
    QueryArguments(int pos, const wxString &name, const wxString type) : m_pos(pos), m_name(name), m_type(type) {}
};


