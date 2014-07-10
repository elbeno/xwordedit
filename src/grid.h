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

#ifndef GRID_H_
#define GRID_H_

// ----------------------------------------------------------------------------

namespace Gdk { class Drawable; }
namespace Gdk { class GC; }
namespace Pango { class Layout; }

#include <glibmm/refptr.h>

#include "cell.h"
#include <list>

class WordSlot;
class GridView;

// ----------------------------------------------------------------------------

class Grid
{
	public:
		enum Symmetry
		{
			NONE,
			LATERAL_X,
			LATERAL_Y,
			LATERAL_XY,
			ROTATIONAL_90,
			ROTATIONAL_180
		};
		
		Grid(int rows, int cols, int symmetry = ROTATIONAL_180);
		~Grid();
		
		void	ToggleCell(int x_pixel, int y_pixel);

		Cell 	*GetCellAt(int x_pixel, int y_pixel) const;
		void	GetXYPos(const Cell *pCell, int &x_pixel, int &y_pixel) const;
		
		int		GetRows() const			{ return mRows; }
		int		GetCols() const			{ return mCols; }
		int		GetWidth() const		{ return mRows * mCells->GetSize() + 2; }
		int		GetHeight() const		{ return mCols * mCells->GetSize() + 2; }
		void	Clear();

		void	Clean()					{ mDirty = false; }
		bool	IsDirty() const			{ return mDirty; }

		const Cell *GetFirstCell() const					{ return mCells; }
		const std::list<WordSlot *>	&GetWordSlots() const	{ return mWordSlots; }

		bool Load(const char *filename);
		bool LoadPUZ(const char *filename);
		bool LoadXML(const char *filename);
		bool Save(const char *filename);
		bool SavePUZ(const char *filename);
		bool SaveXML(const char *filename);

	private:
		int					mRows;
		int					mCols;
		int					mSymmetry;
		bool				mDirty;
		
		Cell				*mCells;
		
		std::list<WordSlot*>	mWordSlots;
		int						mSlotNumber;
				
		void	ResetWordSlots();
		void	CalculateWordSlots(int startrow = 0, int startcol = 0);
};

// ----------------------------------------------------------------------------

#endif /*GRID_H_*/
