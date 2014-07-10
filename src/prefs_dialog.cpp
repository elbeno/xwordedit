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

#include "prefs_dialog.h"
#include <assert.h>

// ----------------------------------------------------------------------------

PrefsDialog::PrefsDialog(BaseObjectType* cobject, const Glib::RefPtr<Gnome::Glade::Xml>& refGlade)
	: Gtk::Dialog(cobject)
	, mRefGlade(refGlade)
	, mpButtonApply(0)
	, mpButtonCancel(0)
	, mpButtonOK(0)
{
	mRefGlade->get_widget("applybutton1", mpButtonApply);
	mRefGlade->get_widget("cancelbutton1", mpButtonCancel);
	mRefGlade->get_widget("okbutton1", mpButtonOK);
  
	assert(mpButtonApply);
	assert(mpButtonCancel);
	assert(mpButtonOK);
  
	mpButtonApply->signal_clicked().connect(sigc::mem_fun(*this, &PrefsDialog::on_button_apply));
	mpButtonCancel->signal_clicked().connect(sigc::mem_fun(*this, &PrefsDialog::on_button_cancel));
	mpButtonOK->signal_clicked().connect(sigc::mem_fun(*this, &PrefsDialog::on_button_ok));
}

PrefsDialog::~PrefsDialog()
{
}

// ----------------------------------------------------------------------------

void PrefsDialog::on_button_apply()
{
}

void PrefsDialog::on_button_cancel()
{
	hide();
}

void PrefsDialog::on_button_ok()
{
	hide();
}

// ----------------------------------------------------------------------------
