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

#ifndef SIGNALSERVER_H_
#define SIGNALSERVER_H_

// ----------------------------------------------------------------------------

#include <sigc++/signal.h>

// ----------------------------------------------------------------------------

class SignalServer
{
	public:
		typedef sigc::signal<void, bool> signal_grid_dirty_t;
		typedef sigc::signal<void> signal_grid_redraw_t;
		
		signal_grid_dirty_t signal_grid_dirty()
		{
			return mSignalGridDirty;
		}
		signal_grid_redraw_t signal_grid_redraw()
		{
			return mSignalGridRedraw;
		}

	protected:
		signal_grid_dirty_t		mSignalGridDirty;
		signal_grid_redraw_t	mSignalGridRedraw;
};

extern SignalServer	gSignalServer;

// ----------------------------------------------------------------------------

#endif /*SIGNALSERVER_H_*/
