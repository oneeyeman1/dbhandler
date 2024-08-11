#pragma once
class LibraryDocument :  public wxDocument
{
public:
    LibraryDocument() : wxDocument() { }
    ~LibraryDocument();
private:
    wxDECLARE_DYNAMIC_CLASS(LibraryDocument);
};
