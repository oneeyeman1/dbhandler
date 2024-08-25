#pragma once


class LibraryDocument :  public wxDocument
{
public:
    LibraryDocument() : wxDocument() { }
    ~LibraryDocument();
private:
    LibraryObject m_libraryObject;
    wxDECLARE_DYNAMIC_CLASS(LibraryDocument);
};
