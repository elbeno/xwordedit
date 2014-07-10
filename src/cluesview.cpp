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

#include "cluesview.h"
#include "wordslot.h"

// ----------------------------------------------------------------------------

CluesView::CluesView()
{
}

// ----------------------------------------------------------------------------

const int SPACING = 5;

void CluesView::on_realize()
{
	Gtk::Frame::on_realize();
	
	set_label("Clues");
	set_border_width(SPACING);
	
	mHBox.pack_start(mAcrossBox, Gtk::PACK_SHRINK, 0);
	mHBox.pack_start(mDownBox, Gtk::PACK_SHRINK, 0);
	
//	mAcrossBox.set_size_request(250);
	//mDownBox.set_size_request(250);
	
	add(mHBox);
	
	show_all_children();
}

// ----------------------------------------------------------------------------

void CluesView::InitClueBoxes()
{
	Gtk::HBox *h = Gtk::manage(new Gtk::HBox);
	mAcrossBox.pack_start(*h, Gtk::PACK_SHRINK);
	Gtk::Label *l = Gtk::manage(new Gtk::Label("ACROSS"));	
	h->pack_start(*l, Gtk::PACK_SHRINK, SPACING);

	h = Gtk::manage(new Gtk::HBox);
	mDownBox.pack_start(*h, Gtk::PACK_SHRINK);
	l = Gtk::manage(new Gtk::Label("DOWN"));	
	h->pack_start(*l, Gtk::PACK_SHRINK, SPACING);
}

// ----------------------------------------------------------------------------

void CluesView::Populate(const std::list<WordSlot *> &slotlist)
{
	mAcrossBox.children().erase(mAcrossBox.children().begin(), mAcrossBox.children().end());
	mDownBox.children().erase(mDownBox.children().begin(), mDownBox.children().end());
	
	InitClueBoxes();
		
	char buffer[512];
	Gtk::HBox *h;
	Gtk::Label *l;
	Gtk::Frame *f;
	for (std::list<WordSlot *>::const_iterator i = slotlist.begin(); i != slotlist.end(); ++i)
	{
		h = Gtk::manage(new Gtk::HBox);
		if ((*i)->IsAcross())
			mAcrossBox.pack_start(*h, Gtk::PACK_SHRINK);
		else
			mDownBox.pack_start(*h, Gtk::PACK_SHRINK);
		
		// clue number
		sprintf(buffer, "%d", (*i)->GetNumber());
		f = Gtk::manage(new Gtk::Frame);
		f->set_shadow_type(Gtk::SHADOW_NONE);
		f->set_size_request(20);
		l = Gtk::manage(new Gtk::Label(buffer, Gtk::ALIGN_RIGHT));
		f->add(*l);
		h->pack_start(*f, Gtk::PACK_SHRINK, SPACING);

		// clue text - if it already ends in ) then don't add the length
		const Glib::ustring &clue = (*i)->GetClue();				
		f = Gtk::manage(new Gtk::Frame);
		f->set_shadow_type(Gtk::SHADOW_NONE);
		//f->set_size_request(200);
		if (clue[clue.length()-1] == ')')
		{
			l = Gtk::manage(new Gtk::Label(clue.c_str(), Gtk::ALIGN_LEFT));
		}
		else
		{
			sprintf(buffer, "%s (%d)", clue.c_str(), (*i)->GetLength());
			l = Gtk::manage(new Gtk::Label(buffer, Gtk::ALIGN_LEFT));
		}
		l->set_justify(Gtk::JUSTIFY_LEFT);
		l->set_line_wrap();
		f->add(*l);
		h->pack_start(*f, Gtk::PACK_SHRINK, SPACING);
	}

	show_all_children();
}

// ----------------------------------------------------------------------------
