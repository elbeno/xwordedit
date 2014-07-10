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

#include "gridview.h"
#include "grid.h"
#include "cell.h"
#include "wordslot.h"
#include "signalserver.h"

#include <gdk/gdkkeysyms.h>
#include <gtkmm/statusbar.h>
#include <gdkmm/pixbuf.h>
#include <gtkmm/messagedialog.h>

#include <cassert>

// ----------------------------------------------------------------------------

Gdk::Color white("white");
Gdk::Color red("red");
Gdk::Color yellow("yellow");
Gdk::Color black("black");
Gdk::Color blue("blue");

Pango::FontDescription	letter_desc("normal 12");
Pango::FontDescription	number_desc("normal 6");

// ----------------------------------------------------------------------------

GridView::GridView(Grid &grid, Gtk::Statusbar &statusbar)
	: mGrid(grid)
	, mStatusBar(statusbar)
	, mpSelectedCell(0)
	, mpSelectedWordSlot(0)
{
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	colormap->alloc_color(white);
	colormap->alloc_color(red);
	colormap->alloc_color(yellow);
	colormap->alloc_color(black);
	colormap->alloc_color(blue);

	set_flags(Gtk::CAN_FOCUS);
	set_events(get_events() | Gdk::BUTTON_PRESS_MASK
							| Gdk::KEY_PRESS_MASK
							| Gdk::POINTER_MOTION_HINT_MASK
							| Gdk::POINTER_MOTION_MASK);

	set_size_request(mGrid.GetWidth(), mGrid.GetHeight());
}

// ----------------------------------------------------------------------------

void GridView::Draw()
{
	if (!is_realized())
		return;
	
	int w = mGrid.GetWidth();
	int h = mGrid.GetHeight();
	
	//Glib::RefPtr<Gdk::Window> window = get_window();
	//int x, y, w, h, depth;
	//window->get_geometry(x, y, w, h, depth);

	mGC->set_foreground(white);
	mPixmap->draw_rectangle(mGC, true, 0, 0, w, h);    
	mGC->set_foreground(black);
	mPixmap->draw_rectangle(mGC, false, 0, 0, w-1, h-1);
	
	DrawSlotBackgrounds();
	DrawCellBackgrounds();
	DrawSlotForegrounds();
	DrawCellForegrounds();
	
	queue_draw();    
}

// ----------------------------------------------------------------------------

void GridView::DrawBlank()
{
	int w = mGrid.GetWidth();
	int h = mGrid.GetHeight();

	mGC->set_foreground(white);
	mPixmap->draw_rectangle(mGC, true, 0, 0, w, h);    
	mGC->set_foreground(black);
	mPixmap->draw_rectangle(mGC, false, 0, 0, w-1, h-1);
	
	DrawCellBackgrounds(false);
	DrawSlotForegrounds();
}

// ----------------------------------------------------------------------------

void GridView::DrawCellBackgrounds(bool show_selection)
{
	const Cell *pCell = mGrid.GetFirstCell();
	int cellSize = pCell->GetSize();
	int x = 1; 
	int y = 1;
	for (int i = 0; i < mGrid.GetRows(); ++i)
	{
		for (int j = 0; j < mGrid.GetCols(); ++j)
		{
			if (pCell->IsOpen())
			{
				if (pCell == mpSelectedCell && show_selection)
				{
					mGC->set_foreground(blue);
					mPixmap->draw_rectangle(mGC, false, x, y, cellSize-1, cellSize-1);
					mPixmap->draw_rectangle(mGC, false, x+1, y+1, cellSize-3, cellSize-3);
				}
				else
				{
					mGC->set_foreground(black);
					mPixmap->draw_rectangle(mGC, false, x, y, cellSize-1, cellSize-1);
				}
			}
			else
			{
				mGC->set_foreground(black);
				mPixmap->draw_rectangle(mGC, true, x, y, cellSize, cellSize);
			}
			++pCell;
			x += cellSize;
		}
		y += cellSize;
		x = 1;
	}
}

// ----------------------------------------------------------------------------

void GridView::DrawSlotBackgrounds()
{
	int cellSize = mGrid.GetFirstCell()->GetSize();
	int x, y, w;
	const std::list<WordSlot*> &slotlist = mGrid.GetWordSlots();
	for (std::list<WordSlot*>::const_iterator i = slotlist.begin(); i != slotlist.end(); ++i)
	{
		if ((*i)->GetWarningPriority() == 1)
			mGC->set_foreground(yellow);
		else continue;
		
		mGrid.GetXYPos((*i)->GetStartCell(), x, y);
		++x, ++y;
		w = cellSize * (*i)->GetLength();
		
		if ((*i)->IsAcross())
			mPixmap->draw_rectangle(mGC, true, x, y, w, cellSize);
		else
			mPixmap->draw_rectangle(mGC, true, x, y, cellSize, w);
	}
	for (std::list<WordSlot*>::const_iterator i = slotlist.begin(); i != slotlist.end(); ++i)
	{
		if ((*i)->GetWarningPriority() == 2)
			mGC->set_foreground(red);
		else continue;
		
		mGrid.GetXYPos((*i)->GetStartCell(), x, y);
		++x, ++y;
		w = cellSize * (*i)->GetLength();
		
		if ((*i)->IsAcross())
			mPixmap->draw_rectangle(mGC, true, x, y, w, cellSize);
		else
			mPixmap->draw_rectangle(mGC, true, x, y, cellSize, w);
	}

	if (mpSelectedWordSlot != 0)
	{
		mGC->set_foreground(blue);
		mGrid.GetXYPos(mpSelectedWordSlot->GetStartCell(), x, y);
		++x, ++y;
		w = cellSize * mpSelectedWordSlot->GetLength();
		
		if (mpSelectedWordSlot->IsAcross())
		{
			mPixmap->draw_rectangle(mGC, false, x, y, w-1, cellSize-1);
			mPixmap->draw_rectangle(mGC, false, x+1, y+1, w-3, cellSize-3);
		}
		else
		{
			mPixmap->draw_rectangle(mGC, false, x, y, cellSize-1, w-1);
			mPixmap->draw_rectangle(mGC, false, x+1, y+1, cellSize-3, w-3);
		}
	}
}

// ----------------------------------------------------------------------------

extern bool gShowAnswers;

void GridView::DrawCellForegrounds()
{
	mGC->set_foreground(black);
	
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout("");
	layout->set_font_description(letter_desc);
	Pango::Rectangle ink_rect, logical_rect;
	int w, h;
	char buffer[2];
	buffer[1] = 0;
	
	const Cell *pCell = mGrid.GetFirstCell();
	int cellSize = pCell->GetSize();
	int x = 1; 
	int y = 1;
	for (int i = 0; i < mGrid.GetRows(); ++i)
	{
		for (int j = 0; j < mGrid.GetCols(); ++j)
		{
			if (gShowAnswers)
				buffer[0] = pCell->GetAnswerLetter();
			else
				buffer[0] = pCell->GetGuessLetter();

			if (buffer[0] != 0)
			{
				layout->set_text(buffer);
				layout->get_extents(ink_rect, logical_rect);
				w = ink_rect.get_width() / Pango::SCALE;
				h = ink_rect.get_height() / Pango::SCALE;
				mPixmap->draw_layout(mGC, x + (cellSize - w)/2, y + (cellSize - h)/2, layout);
			}
			++pCell;
			x += cellSize;
		}
		y += cellSize;
		x = 1;
	}
}

// ----------------------------------------------------------------------------

void GridView::DrawSlotForegrounds()
{
	mGC->set_foreground(black);
	
	Glib::RefPtr<Pango::Layout> layout = create_pango_layout("");
	layout->set_font_description(number_desc);
	int x, y;
	char buffer[8];
	
	const std::list<WordSlot*> &slotlist = mGrid.GetWordSlots();
	for (std::list<WordSlot*>::const_iterator i = slotlist.begin(); i != slotlist.end(); ++i)
	{
		mGrid.GetXYPos((*i)->GetStartCell(), x, y);
		++x, ++y;
		
		sprintf(buffer, "%d", (*i)->GetNumber());
		layout->set_text(buffer);
		mPixmap->draw_layout(mGC, x+2, y+2, layout);
	}
}

// ----------------------------------------------------------------------------

void GridView::AdvanceSelectedCell()
{
	assert(mpSelectedCell != 0);
	
	int size = mpSelectedCell->GetSize();
	int x, y;
	mGrid.GetXYPos(mpSelectedCell, x, y);
	x++, y++;
	Cell *pCell = 0;
	if (mpSelectedWordSlot)
	{
		if (mpSelectedWordSlot->IsAcross())
			pCell = mGrid.GetCellAt(x + size, y);
		else
			pCell = mGrid.GetCellAt(x, y + size);

		if (pCell->IsOpen())
			mpSelectedCell = pCell;
	}
}

// ----------------------------------------------------------------------------

void GridView::BackspaceSelectedCell()
{
	assert(mpSelectedCell != 0);
	
	int size = mpSelectedCell->GetSize();
	int x, y;
	mGrid.GetXYPos(mpSelectedCell, x, y);
	x++, y++;
	Cell *pCell = 0;
	if (mpSelectedWordSlot)
	{
		if (mpSelectedWordSlot->IsAcross())
			pCell = mGrid.GetCellAt(x - size, y);
		else
			pCell = mGrid.GetCellAt(x, y - size);

		if (pCell->IsOpen())
			mpSelectedCell = pCell;	
	}
}

// ----------------------------------------------------------------------------

void GridView::SelectWordSlot(Cell *pCell)
{
	if (pCell->GetAcrossWordSlot() == 0)		
		mpSelectedWordSlot = pCell->GetDownWordSlot();
	else if (pCell->GetDownWordSlot() == 0)		
		mpSelectedWordSlot = pCell->GetAcrossWordSlot();					
	else if (mpSelectedWordSlot == pCell->GetAcrossWordSlot())
		mpSelectedWordSlot = pCell->GetDownWordSlot();
	else
		mpSelectedWordSlot = pCell->GetAcrossWordSlot();
}

// ----------------------------------------------------------------------------

void GridView::ResizeCanvas()
{
	Glib::RefPtr<Gdk::Window> window = get_window();
	int x, y, w, h, depth;
	window->get_geometry(x, y, w, h, depth);
	w = mGrid.GetWidth();
	h = mGrid.GetHeight();
	
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	Glib::RefPtr<Gdk::Pixmap> newPixmap = Gdk::Pixmap::create(window, w, h, depth);
	newPixmap->set_colormap(colormap);

	// if we already had one, then draw	again
	if (mPixmap)
	{
		mPixmap = newPixmap;
		Draw();
	}
	else
	{
		mPixmap = newPixmap;
	}
}

// ----------------------------------------------------------------------------

void GridView::on_realize()
{
	Gtk::DrawingArea::on_realize();

	Glib::RefPtr<Gdk::Window> window = get_window();
	window->set_background(white);
	mGC = Gdk::GC::create(window);
	
	ResizeCanvas();
	Draw();

	grab_focus();
}

// ----------------------------------------------------------------------------

bool GridView::on_configure_event(GdkEventConfigure *)
{
	ResizeCanvas();
	return true;
}

// ----------------------------------------------------------------------------

bool GridView::on_expose_event(GdkEventExpose *e)
{
	// bit the pixmap to the window
	Glib::RefPtr<Gdk::Window> window = get_window();
	window->draw_drawable(mGC, mPixmap,
						e->area.x, e->area.y, 
						e->area.x, e->area.y,
						e->area.width, e->area.height);
	return true;
}

// ----------------------------------------------------------------------------

bool GridView::on_button_press_event(GdkEventButton *e)
{
	if (e->type == GDK_BUTTON_PRESS)
	{
		// single click
		// left click - select the cell
		if (e->button == 1)
		{
			Cell *pCell = mGrid.GetCellAt((int)e->x, (int)e->y);
			if (mpSelectedCell != pCell && pCell->IsOpen())
			{
				mpSelectedCell = pCell;
				gSignalServer.signal_grid_redraw().emit();
			}
		}
		// right click - toggle it open/closed
		else if (e->button == 3)
		{
			Cell *pCell = mGrid.GetCellAt((int)e->x, (int)e->y);
			if (mpSelectedCell == pCell)
				mpSelectedCell = 0;
			
			mGrid.ToggleCell((int)e->x, (int)e->y);
			mpSelectedWordSlot = 0;
			Draw();
			UpdateStatusBar(pCell);
			gSignalServer.signal_grid_redraw().emit();
		}
	}
	else if (e->type == GDK_2BUTTON_PRESS)
	{
		// double (left) click - select the slot
		if (e->button == 1)
		{
			Cell *pCell = mGrid.GetCellAt((int)e->x, (int)e->y);
			if (pCell->IsOpen())
			{
				SelectWordSlot(pCell);
				Draw();
			}
		}		
	}
	
	return true;
}

// ----------------------------------------------------------------------------

bool GridView::on_key_press_event(GdkEventKey *e)
{	
	if (mpSelectedCell)
	{
		if (e->keyval >= GDK_A && e->keyval <= GDK_Z)
		{
			if (gShowAnswers)
				mpSelectedCell->SetAnswerLetter(e->keyval - GDK_A + 'A');
			else
				mpSelectedCell->SetGuessLetter(e->keyval - GDK_A + 'A');
			AdvanceSelectedCell();
			gSignalServer.signal_grid_redraw().emit();
		}
		else if (e->keyval >= GDK_a && e->keyval <= GDK_z)
		{
			if (gShowAnswers)
				mpSelectedCell->SetAnswerLetter(e->keyval - GDK_a + 'A');
			else
				mpSelectedCell->SetGuessLetter(e->keyval - GDK_a + 'A');
			AdvanceSelectedCell();
			gSignalServer.signal_grid_redraw().emit();
		}
		else
		{
			int size = mpSelectedCell->GetSize();
			int x, y;
			mGrid.GetXYPos(mpSelectedCell, x, y);
			x++, y++;
			Cell *pCell = 0;
			switch (e->keyval)
			{
				case GDK_Return:
				case GDK_KP_Enter:
					SelectWordSlot(mpSelectedCell);
					break;
				case GDK_BackSpace:
					if (gShowAnswers)
						mpSelectedCell->SetAnswerLetter(0);
					else
						mpSelectedCell->SetGuessLetter(0);
					BackspaceSelectedCell();
					gSignalServer.signal_grid_redraw().emit();
					break;
				case GDK_Delete:
					if (gShowAnswers)
						mpSelectedCell->SetAnswerLetter(0);
					else
						mpSelectedCell->SetGuessLetter(0);
					gSignalServer.signal_grid_redraw().emit();
					break;
				case GDK_Left:
					pCell = mGrid.GetCellAt(x - size, y);
					if (pCell && pCell->IsOpen())
						mpSelectedCell = pCell;
					break;
				case GDK_Right:
					pCell = mGrid.GetCellAt(x + size, y);
					if (pCell && pCell->IsOpen())
						mpSelectedCell = pCell;
					break;
				case GDK_Up:
					pCell = mGrid.GetCellAt(x, y - size);
					if (pCell && pCell->IsOpen())
						mpSelectedCell = pCell;
					break;
				case GDK_Down:
					pCell = mGrid.GetCellAt(x, y + size);
					if (pCell && pCell->IsOpen())
						mpSelectedCell = pCell;
					break;
				default:
					break;
			}
		}
		Draw();
	}
	return true;
}

// ----------------------------------------------------------------------------

bool GridView::on_motion_notify_event(GdkEventMotion *e)
{
	int x, y;
	Gdk::ModifierType state;
	
	if (e && e->window)
	{
		const Glib::RefPtr<Gdk::Window> refWindow = Glib::wrap((GdkWindowObject*) e->window, true);
		refWindow->get_pointer(x, y, state);
		Cell *pCell = mGrid.GetCellAt((int)e->x, (int)e->y);		
		UpdateStatusBar(pCell);
	}
	return true;
}

// ----------------------------------------------------------------------------

void GridView::UpdateStatusBar(Cell *pCell)
{
	mStatusBar.pop();
	Glib::ustring statusbar_text;
	if (pCell->IsOpen())
	{
		WordSlot *pASlot = pCell->GetAcrossWordSlot();
		WordSlot *pDSlot = pCell->GetDownWordSlot();
		assert(pASlot != 0 || pDSlot != 0);
		int aslot_priority = 0;
		int dslot_priority = 0;
		if (pASlot) aslot_priority = pASlot->GetWarningPriority();
		if (pDSlot)	dslot_priority = pDSlot->GetWarningPriority();
		if (aslot_priority != 0 || dslot_priority != 0)
		{
			if (aslot_priority >= dslot_priority)
				statusbar_text = pASlot->GetWarningText();
			else
				statusbar_text = pDSlot->GetWarningText();
		}
	}
	if (statusbar_text != "")
	{
		mStatusBar.push(statusbar_text);
	}
}

// ----------------------------------------------------------------------------

bool GridView::SavePNG(const char *filename)
{
	DrawBlank();
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w = mGrid.GetWidth();
	int h = mGrid.GetHeight();
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "png");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------

bool GridView::SaveJPEG(const char *filename)
{
	DrawBlank();
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w = mGrid.GetWidth();
	int h = mGrid.GetHeight();
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "jpeg");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------

bool GridView::SaveBMP(const char *filename)
{
	DrawBlank();
	Glib::RefPtr<Gdk::Colormap>	colormap = get_default_colormap();
	int w = mGrid.GetWidth();
	int h = mGrid.GetHeight();
	Glib::RefPtr<Gdk::Image> image = mPixmap->get_image(0, 0, w, h);
	Glib::RefPtr<Gdk::Pixbuf> pixbuf = Gdk::Pixbuf::create(image, colormap, 0, 0, 0, 0, w, h);
	bool ret = true;
	try
	{
		pixbuf->save(filename, "bmp");
	}
	catch (Glib::FileError e)
	{
		ret = false;
	}
	Draw();
	return ret;
}

// ----------------------------------------------------------------------------

bool GridView::SaveHTML(const char *filename)
{
	Gtk::MessageDialog dialog("Saving in HTML format is not yet implemented", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
	dialog.run();	
	return true;
}

// ----------------------------------------------------------------------------
