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

#ifndef CLUESVIEW_H_
#define CLUESVIEW_H_

// ----------------------------------------------------------------------------

#include <gtkmm/frame.h>
#include <gtkmm/box.h>
#include <gtkmm/label.h>

class WordSlot;

#include <list>

// ----------------------------------------------------------------------------

class CluesView : public Gtk::Frame
{
	public:
		CluesView();

		void Populate(const std::list<WordSlot *> &slotlist);

	private:
		virtual void on_realize();
		
		void	InitClueBoxes();
		
		Gtk::HBox	mHBox;
		Gtk::VBox	mAcrossBox;
		Gtk::VBox	mDownBox;
		
};

// ----------------------------------------------------------------------------

#endif /*CLUESVIEW_H_*/
