// ----------------------------------------------------------------------------

// Copyright (C) 2005  Ben Deane (ben@elbeno.com)

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

// ----------------------------------------------------------------------------

#ifndef PREFS_DIALOG_H_
#define PREFS_DIALOG_H_

// ----------------------------------------------------------------------------

#include <gtkmm.h>
#include <libglademm.h>

class PrefsDialog : public Gtk::Dialog
{
public:
  PrefsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade);
  virtual ~PrefsDialog();

protected:
  //Signal handlers:
  virtual void on_button_apply();
  virtual void on_button_cancel();
  virtual void on_button_ok();

  Glib::RefPtr<Gnome::Glade::Xml> mRefGlade;
  
  Gtk::Button* mpButtonApply;
  Gtk::Button* mpButtonCancel;
  Gtk::Button* mpButtonOK;
};

// ----------------------------------------------------------------------------

#endif /*PREFS_DIALOG_H_*/
