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

#include "grid_window.h"
#include "grid.h"

#include <gtkmm/stock.h>
#include <gtkmm/main.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>
#include <gtkmm/toggleaction.h>
#include <gtkmm/radioaction.h>
#include <libglademm.h>

#include <string.h>

#include "signalserver.h"

// ----------------------------------------------------------------------------

bool gShowAnswers;

GridWindow::GridWindow(Grid &grid)
	: mGrid(grid)
	, mGridView(mGrid, mStatusBar)
	, mCrosswordNumber(0)
	, mpPrefsDialog(0)
{
	Glib::RefPtr<Gtk::ActionGroup> mActionGroup = Gtk::ActionGroup::create();
	
	mActionGroup->add(Gtk::Action::create("MenuFile", "_File"));
	mActionGroup->add(Gtk::Action::create("New", Gtk::Stock::NEW),
						sigc::mem_fun(*this, &GridWindow::on_action_file_new));
	mActionGroup->add(Gtk::Action::create("Open", Gtk::Stock::OPEN),
						sigc::mem_fun(*this, &GridWindow::on_action_file_open));
	mActionGroup->add(Gtk::Action::create("Save", Gtk::Stock::SAVE),
						sigc::mem_fun(*this, &GridWindow::on_action_file_save));
	mActionGroup->add(Gtk::Action::create("Save As", Gtk::Stock::SAVE_AS, "Save _As..."),
						sigc::mem_fun(*this, &GridWindow::on_action_file_saveas));
  	mActionGroup->add(Gtk::Action::create("Export", "_Export..."),
						sigc::mem_fun(*this, &GridWindow::on_action_file_export));
  	mActionGroup->add(Gtk::Action::create("Properties", Gtk::Stock::PROPERTIES),
						sigc::mem_fun(*this, &GridWindow::on_action_file_properties));
	mActionGroup->add(Gtk::Action::create("Quit", Gtk::Stock::QUIT),
						sigc::mem_fun(*this, &GridWindow::on_action_file_quit));

	mActionGroup->add(Gtk::Action::create("MenuEdit", "_Edit"));
	mActionGroup->add(Gtk::Action::create("Preferences", Gtk::Stock::PREFERENCES),
						sigc::mem_fun(*this, &GridWindow::on_action_edit_preferences));
						
	mActionGroup->add(Gtk::Action::create("MenuView", "_View"));
	mActionGroup->add(Gtk::ToggleAction::create("Toolbar", "_Toolbar", "", true),
						sigc::mem_fun(*this, &GridWindow::on_action_view_toolbar));
	mActionGroup->add(Gtk::ToggleAction::create("Status Bar", "Status _Bar", "", true),
						sigc::mem_fun(*this, &GridWindow::on_action_view_statusbar));
	mActionGroup->add(Gtk::ToggleAction::create("Answers", "_Answers", "", false),
						sigc::mem_fun(*this, &GridWindow::on_action_view_answers));

	mActionGroup->add(Gtk::Action::create("MenuGrid", "_Grid"));
	mActionGroup->add(Gtk::Action::create("Fill", "_Fill"),
						sigc::mem_fun(*this, &GridWindow::on_action_grid_fill));
	mActionGroup->add(Gtk::Action::create("Rules", "_Rules"),
						sigc::mem_fun(*this, &GridWindow::on_action_grid_rules));

	mUIManager = Gtk::UIManager::create();
	mUIManager->insert_action_group(mActionGroup);
	add_accel_group(mUIManager->get_accel_group());
	
	Glib::ustring ui_info =
        "<ui>"
        "  <menubar name='MenuBar'>"
        "    <menu action='MenuFile'>"
        "      <menuitem action='New'/>"
        "      <menuitem action='Open'/>"
        "      <separator/>"
        "      <menuitem action='Save'/>"
        "      <menuitem action='Save As'/>"
        "      <separator/>"
        "      <menuitem action='Export'/>"
        "      <separator/>"
        "      <menuitem action='Properties'/>"
        "      <separator/>"
        "      <menuitem action='Quit'/>"
        "    </menu>"
        "    <menu action='MenuEdit'>"
        "      <menuitem action='Preferences'/>"
        "    </menu>"
        "    <menu action='MenuView'>"
        "      <menuitem action='Toolbar'/>"
        "      <menuitem action='Status Bar'/>"
        "      <menuitem action='Answers'/>"
        "    </menu>"
        "    <menu action='MenuGrid'>"
        "      <menuitem action='Fill'/>"
        "      <menuitem action='Rules'/>"
        "    </menu>"
        "  </menubar>"
        "  <toolbar  name='ToolBar'>"
        "    <toolitem action='New'/>"
        "    <toolitem action='Open'/>"
        "    <toolitem action='Save'/>"
        "    <separator/>"
        "    <toolitem action='Answers'/>"
        "    <toolitem action='Fill'/>"
        "    <toolitem action='Rules'/>"
        "    <separator/>"
        "    <toolitem action='Quit'/>"
        "  </toolbar>"
        "</ui>";

    mUIManager->add_ui_from_string(ui_info);	
    Gtk::Widget	*pMenuBar = mUIManager->get_widget("/MenuBar");
    mVBox.pack_start(*pMenuBar, Gtk::PACK_SHRINK);

    Gtk::Widget	*pToolBar = mUIManager->get_widget("/ToolBar");
    mVBox.pack_start(*pToolBar, Gtk::PACK_SHRINK);

	mHBox.pack_start(mGridView);
	mHBox.pack_start(mCluesView);
	mVBox.pack_start(mHBox);
	mStatusBar.set_has_resize_grip(false);
	mVBox.pack_start(mStatusBar, Gtk::PACK_SHRINK);
	
	set_resizable(false);
	add(mVBox);
	
	gSignalServer.signal_grid_dirty().connect(sigc::mem_fun(*this, &GridWindow::on_grid_dirty));	
	gSignalServer.signal_grid_redraw().connect(sigc::mem_fun(*this, &GridWindow::on_grid_redraw));	
	
	char buffer[256];
	sprintf(buffer, "crossword%03d", ++mCrosswordNumber);
	mTitle = buffer;
}

// ----------------------------------------------------------------------------

void GridWindow::on_realize()
{
	Gtk::Window::on_realize();
	gSignalServer.signal_grid_redraw().emit();	
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_new()
{
	int result = Gtk::RESPONSE_YES;
	if (mGrid.IsDirty())
	{
		Gtk::MessageDialog dialog(*this, "There are unsaved changes. Discard them?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		result = dialog.run();
	}
	
	if (result == Gtk::RESPONSE_YES)
	{
		char buffer[256];
		sprintf(buffer, "crossword%03d", ++mCrosswordNumber);
		mTitle = buffer;
		mGrid.Clear();
		mGridView.Draw();
		gSignalServer.signal_grid_redraw().emit();	
	}
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_open()
{
	int result = Gtk::RESPONSE_YES;
	if (mGrid.IsDirty())
	{
		Gtk::MessageDialog dialog(*this, "There are unsaved changes. Discard them?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		result = dialog.run();
	}
	
	if (result != Gtk::RESPONSE_YES)
		return;
	
	Gtk::FileChooserDialog dialog("Open", Gtk::FILE_CHOOSER_ACTION_OPEN);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::OPEN, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:
	Gtk::FileFilter filter_any;
	filter_any.set_name("Any files");
	filter_any.add_pattern("*");
	dialog.add_filter(filter_any);

	Gtk::FileFilter filter_xml;
	filter_xml.set_name("XML files");
	filter_xml.add_mime_type("text/xml");
	dialog.add_filter(filter_xml);

	Gtk::FileFilter filter_puz;
	filter_puz.set_name(".PUZ files");
	dialog.add_filter(filter_puz);
  
  	bool error = true;
  	while (error)
  	{
  		error = false;
		//Show the dialog and wait for a user response:
		int result = dialog.run();
	
		//Handle the response:
		switch(result)
		{
			case Gtk::RESPONSE_OK:
			{
				mFilename = dialog.get_filename();
				const Gtk::FileFilter *filter = dialog.get_filter();
				bool success = true;
				FileType t = GetTypeFromFilename(mFilename.c_str());
				GetTitleFromFilename();
				switch (t)
				{
					case NONE:
						success = mGrid.Load(mFilename.c_str()); break;
					case PUZ:
						success = mGrid.LoadPUZ(mFilename.c_str()); break;
					case XML:
						success = mGrid.LoadXML(mFilename.c_str()); break;
					default:
						if (filter == &filter_puz)
							success = mGrid.LoadPUZ(mFilename.c_str());
						else if (filter == &filter_xml)
							success = mGrid.LoadXML(mFilename.c_str());
						else
							success = mGrid.Load(mFilename.c_str());
						break;
				}				
				if (!success)
				{
					Gtk::MessageDialog dialog(*this, "Error loading the file!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
					dialog.run();
					error = true;
				}
				break;
			}
			case Gtk::RESPONSE_CANCEL:
			default:
				break;
		}	
  	}
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_save()
{
	if (mFilename.empty())
	{
		on_action_file_saveas();
	}
	else
	{
		if (!mGrid.Save(mFilename.c_str()))
		{
			Gtk::MessageDialog dialog(*this, "Error saving the file!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
			dialog.run();
		}
	}
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_saveas()
{
	Gtk::FileChooserDialog dialog("Save As", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:
	Gtk::FileFilter filter_any;
	filter_any.set_name("Any files");
	filter_any.add_pattern("*");
	dialog.add_filter(filter_any);

	Gtk::FileFilter filter_xml;
	filter_xml.set_name("XML files");
	filter_xml.add_mime_type("text/xml");
	dialog.add_filter(filter_xml);

	Gtk::FileFilter filter_puz;
	filter_puz.set_name(".PUZ files");
	dialog.add_filter(filter_puz);
  
  	bool error = true;
  	while (error)
  	{
  		error = false;
		//Show the dialog and wait for a user response:
		int result = dialog.run();
	
		//Handle the response:
		switch(result)
		{
			case Gtk::RESPONSE_OK:
			{
				mFilename = dialog.get_filename();
				if (ConfirmOverwrite(mFilename.c_str()))
				{
					const Gtk::FileFilter *filter = dialog.get_filter();
					bool success = true;
					FileType t = GetTypeFromFilename(mFilename.c_str());
					GetTitleFromFilename();
					switch (t)
					{
						case NONE:
							success = mGrid.Save(mFilename.c_str()); break;
						case PUZ:
							success = mGrid.SavePUZ(mFilename.c_str()); break;
						case XML:
							success = mGrid.SaveXML(mFilename.c_str()); break;
						default:
							if (filter == &filter_puz)
								success = mGrid.SavePUZ(mFilename.c_str());
							else if (filter == &filter_xml)
								success = mGrid.SaveXML(mFilename.c_str());
							else
								success = mGrid.Save(mFilename.c_str());
							break;
					}				
					if (!success)
					{
						Gtk::MessageDialog dialog(*this, "Error saving the file!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
						dialog.run();
						error = true;
					}
				}
				break;
			}
			case Gtk::RESPONSE_CANCEL:
			default:
				break;
		}
  	}
}

// ----------------------------------------------------------------------------

void GridWindow::GetTitleFromFilename()
{
	std::string::size_type pos = mFilename.rfind('/');
	if (pos == std::string::npos)
		pos = mFilename.rfind('\\');
	if (pos == std::string::npos)
		pos = 0;
	mTitle = &mFilename.c_str()[pos+1];
}

// ----------------------------------------------------------------------------

GridWindow::FileType GridWindow::GetTypeFromFilename(const char *filename)
{
	const char *extension = strrchr(filename, '.');
	if (extension)
	{	
		if (!strcmp(extension, ".png"))
			return PNG;
		else if (!strcmp(extension, ".jpg") || !strcmp(extension, ".jpeg"))
			return JPEG;
		else if (!strcmp(extension, ".bmp"))
			return BMP;
		else if (!strcmp(extension, ".html") || !strcmp(extension, ".htm"))
			return HTML;
		else if (!strcmp(extension, ".puz"))
			return PUZ;
	}
	return NONE;
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_export()
{
	Gtk::FileChooserDialog dialog("Export empty grid...", Gtk::FILE_CHOOSER_ACTION_SAVE);
	dialog.set_transient_for(*this);

	//Add response buttons the the dialog:
	dialog.add_button(Gtk::Stock::CANCEL, Gtk::RESPONSE_CANCEL);
	dialog.add_button(Gtk::Stock::SAVE, Gtk::RESPONSE_OK);

	//Add filters, so that only certain file types can be selected:
	Gtk::FileFilter filter_png;
	filter_png.set_name("PNG files");
	filter_png.add_mime_type("image/png");
	dialog.add_filter(filter_png);

	Gtk::FileFilter filter_jpeg;
	filter_jpeg.set_name("JPEG files");
	filter_jpeg.add_mime_type("image/jpeg");
	dialog.add_filter(filter_jpeg);
	
	Gtk::FileFilter filter_bmp;
	filter_bmp.set_name("BMP files");
	filter_bmp.add_mime_type("image/bmp");
	dialog.add_filter(filter_bmp);
	
	Gtk::FileFilter filter_html;
	filter_html.set_name("HTML files");
	filter_html.add_mime_type("text/html");
	dialog.add_filter(filter_html);

  	bool error = true;
  	while (error)
  	{
  		error = false;
		//Show the dialog and wait for a user response:
		int result = dialog.run();
	
		//Handle the response:
		switch(result)
		{
			case Gtk::RESPONSE_OK:
			{
				Glib::ustring filename = dialog.get_filename();
				if (ConfirmOverwrite(filename.c_str()))
				{
					const Gtk::FileFilter *filter = dialog.get_filter();
					bool success = true;
					FileType t = GetTypeFromFilename(filename.c_str());
					switch (t)
					{
						case PNG:
							success = mGridView.SavePNG(filename.c_str()); break;
						case JPEG:
							success = mGridView.SaveJPEG(filename.c_str()); break;
						case BMP:
							success = mGridView.SaveBMP(filename.c_str()); break;
						case HTML:
							success = mGridView.SaveHTML(filename.c_str()); break;
						default:
							if (filter == &filter_png)
								success = mGridView.SavePNG(filename.c_str());
							else if (filter == &filter_jpeg)
								success = mGridView.SaveJPEG(filename.c_str());
							else if (filter == &filter_bmp)
								success = mGridView.SaveBMP(filename.c_str());
							else if (filter == &filter_html)
								success = mGridView.SaveHTML(filename.c_str());
							break;
					}
					if (!success)
					{
						Gtk::MessageDialog dialog(*this, "Error saving the file!", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
						dialog.run();
						error = true;
					}
				}
				break;
			}
			case Gtk::RESPONSE_CANCEL:
			default:
				break;
		}
  	}
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_properties()
{
}

// ----------------------------------------------------------------------------

bool GridWindow::ConfirmQuit()
{
	int result = Gtk::RESPONSE_YES;
	if (mGrid.IsDirty())
	{
		Gtk::MessageDialog dialog(*this, "There are unsaved changes. Quit anyway?", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		result = dialog.run();
	}
	return (result == Gtk::RESPONSE_YES);
}

// ----------------------------------------------------------------------------

bool GridWindow::ConfirmOverwrite(const char *filename)
{
	int result = Gtk::RESPONSE_YES;
	FILE *fp = fopen(filename, "rb");
	bool exists = (fp != 0);
	if (exists)
	{
		fclose(fp);
		char msgbuf[256];
		sprintf(msgbuf, "%s already exists. Overwrite it?", filename);
		Gtk::MessageDialog dialog(*this, msgbuf, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
		result = dialog.run();
	}
	return (result == Gtk::RESPONSE_YES);
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_file_quit()
{	
	if (ConfirmQuit())
	{
	  	Gtk::Main::quit();
	}
}

// ----------------------------------------------------------------------------

bool GridWindow::on_delete_event(GdkEventAny *e)
{
	return !ConfirmQuit();
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_edit_preferences()
{
	if (mpPrefsDialog == 0)
	{
		Glib::RefPtr<Gnome::Glade::Xml> refXml = Gnome::Glade::Xml::create("project1.glade");
		refXml->get_widget_derived("Preferences", mpPrefsDialog);
	}	
	mpPrefsDialog->run();
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_view_toolbar()
{	
    Gtk::Widget	*pToolBar = mUIManager->get_widget("/ToolBar");
	if (pToolBar->is_visible())
		pToolBar->hide();
	else
		pToolBar->show();
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_view_statusbar()
{	
	if (mStatusBar.is_visible())
		mStatusBar.hide();
	else
		mStatusBar.show();
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_view_answers()
{	
	gShowAnswers = !gShowAnswers;
	mGridView.Draw();
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_grid_fill()
{	
}

// ----------------------------------------------------------------------------

void GridWindow::on_action_grid_rules()
{	
}

// ----------------------------------------------------------------------------

void GridWindow::on_grid_dirty(bool dirty)
{
	char buffer[256];
	if (dirty)
	{
		sprintf(buffer, "* %s - xwordedit", mTitle.c_str());
		set_title(buffer);
	}
	else
	{
		sprintf(buffer, "%s - xwordedit", mTitle.c_str());
		set_title(buffer);
	}
}

// ----------------------------------------------------------------------------

void GridWindow::on_grid_redraw()
{
	mGridView.set_size_request(mGrid.GetWidth(), mGrid.GetHeight());
	mGridView.Draw();
	mCluesView.Populate(mGrid.GetWordSlots());
}

// ----------------------------------------------------------------------------
