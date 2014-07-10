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

#ifndef GRID_WINDOW_H_
#define GRID_WINDOW_H_

// ----------------------------------------------------------------------------

#include <gtkmm/window.h>
#include <gtkmm/statusbar.h>
#include <gtkmm/box.h>
#include <glibmm/refptr.h>
#include <gtkmm/uimanager.h>
#include <gtkmm/actiongroup.h>
#include <glibmm/ustring.h>
#include <string>

#include "gridview.h"
#include "cluesview.h"
#include "prefs_dialog.h"

class Grid;

// ----------------------------------------------------------------------------

class GridWindow : public Gtk::Window
{
	public:
		GridWindow(Grid &grid);
		
	private:
		enum FileType
		{
			NONE,
			PNG,
			JPEG,
			BMP,
			HTML,
			PUZ,
			XML
		};

		Glib::RefPtr<Gtk::UIManager> mUIManager;
		Glib::RefPtr<Gtk::ActionGroup> mActionGroup;
		
		Gtk::VBox		mVBox;
		Gtk::HBox		mHBox;
		
		Gtk::Statusbar	mStatusBar;
		Grid 			&mGrid;
		GridView 		mGridView;	
		CluesView		mCluesView;
		
		Glib::ustring	mTitle;
		std::string		mFilename;
		
		int				mCrosswordNumber;
		
		PrefsDialog		*mpPrefsDialog;
		
		void on_action_file_new();
		void on_action_file_open();
		void on_action_file_save();
		void on_action_file_saveas();
		void on_action_file_export();
		void on_action_file_properties();
		void on_action_file_quit();
		
		void on_action_edit_preferences();
		
		void on_action_view_toolbar();
		void on_action_view_statusbar();
		void on_action_view_answers();
		
		void on_action_grid_fill();
		void on_action_grid_rules();
		
		virtual void	on_realize();
		virtual bool	on_delete_event(GdkEventAny *e);	
		void			on_grid_dirty(bool dirty);
		void			on_grid_redraw();
		
		bool	ConfirmQuit();
		bool	ConfirmOverwrite(const char *filename);
		void	GetTitleFromFilename();
		FileType GetTypeFromFilename(const char *filename);		
};

// ----------------------------------------------------------------------------

#endif /*GRID_WINDOW_H_*/
