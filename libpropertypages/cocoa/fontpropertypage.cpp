// -*- C++ -*- generated by wxGlade 0.5 on Wed Feb 06 22:39:49 2008
/***************************************************************************
 *   Copyright (C) 2005 by Igor Korot                                      *
 *   igor@IgorsGentoo                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#import "AppKit/AppKit.h"

#include "wx/font.h"
#include "wx/nativewin.h"
#include "../fontpropertypagebase.h"

@interface AppDelegate : NSObject <NSApplicationDelegate>
{
    NSFont *font;
}
- (IBAction)selectFont:(id)sender;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    font = [NSFont boldSystemFontOfSize:12];
}

- (IBAction)selectFont:(id)sender
{
    NSFontManager *fontManager = [NSFontManager sharedFontManager];
    [fontManager setDelegate:self];
    [fontManager setTarget:self];
    [fontManager orderFrontFontPanel:self];
}

- (void)changeFont:(id)sender
{
    font = [sender convertFont:font];
    NSLog(@"%@", font);
}
@end

CFontPropertyPage::CFontPropertyPage(wxWindow* parent, wxFont font, int id, const wxPoint& pos, const wxSize& size, long style)
 : CFontPropertyPageBase(parent, font, id, pos, size, wxTAB_TRAVERSAL)
{
    m_font = font;
    NSFontManager *manager = [NSFontManager sharedFontManager];
//    [manager setDelegate:self];
    NSFontPanel *panel = [manager fontPanel:true];
//    [panel setPanelFont: m_font->GetNativeFontInfo isMultiple: false];
//    m_holder = new wxNativeWindow( this, wxID_ANY, panel );
}

CFontPropertyPage::~CFontPropertyPage()
{
}