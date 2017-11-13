/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/mainframe.cpp
// Purpose:     DB Handler
// Author:      Igor Korot
// Created:     17 DEC 2015
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#include "wx/mdi.h"
#endif

#include "wxsf/RectShape.h"
#include "wxsf/TextShape.h"
#include "wxsf/BitmapShape.h"
#include "constraint.h"
#include "constraintsign.h"

ConstraintSign::ConstraintSign(ViewType type)
{
    m_type = type;
    m_sign = NULL;
    m_fKey = NULL;
}

ConstraintSign::~ConstraintSign()
{
}

void ConstraintSign::SetSign(const wxString &sign)
{
    m_sign->SetText( sign );
}

const wxString &ConstraintSign::GetSign()
{
    return m_sign->GetText();
}
