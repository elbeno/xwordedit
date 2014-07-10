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

#ifndef WORDSLOT_H_
#define WORDSLOT_H_

// ----------------------------------------------------------------------------

#include <glibmm/ustring.h>

class Cell;

// ----------------------------------------------------------------------------

class WordSlot
{
	public:
		WordSlot(Cell *pStartCell, int number, bool across, int gridWidth, const Cell *pLimitCell);	
		
		Cell				*GetStartCell() const	{ return mpStartCell; }
		int					GetLength() const		{ return mLength; }
		int					GetNumber() const		{ return mNumber; }
		bool				IsAcross() const		{ return mAcross; }
		int					GetUncheckedPercent() const;

		int					GetWarningPriority() const;
		const Glib::ustring	GetWarningText() const;
		const Glib::ustring	&GetClue() const				{ return mClue; }
		void				SetClue(const Glib::ustring s)	{ mClue = s; }
		
	private:
		Cell			*mpStartCell;
		int				mNumber;
		bool			mAcross;
		int				mLength;
		int				mGridWidth;
		mutable int 	mUncheckedPercent;
		Glib::ustring	mClue;
};

// ----------------------------------------------------------------------------

#endif /*WORDSLOT_H_*/
