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

#ifndef CELL_H_
#define CELL_H_

// ----------------------------------------------------------------------------

namespace Gdk { class Drawable; }
namespace Gdk { class GC; }
namespace Pango { class Layout; }

#include <glibmm/refptr.h>

class WordSlot;

// ----------------------------------------------------------------------------

class Cell
{
	public:
		Cell(int size = 30);
		
		void	SetAcrossWordSlot(WordSlot *w)	{ mpAcross = w; }
		void	SetDownWordSlot(WordSlot *w)	{ mpDown = w; }
		WordSlot *GetAcrossWordSlot() const		{ return mpAcross; }
		WordSlot *GetDownWordSlot() const		{ return mpDown; }

		bool	IsOpen() const				{ return mOpen; }
		bool	Toggle()					{ mOpen = !mOpen; return mOpen; }		
		char	GetAnswerLetter() const			{ return mAnswerLetter; }
		void	SetAnswerLetter(char l)			{ mAnswerLetter = l; }
		char	GetGuessLetter() const			{ return mGuessLetter; }
		void	SetGuessLetter(char l)			{ mGuessLetter = l; }
		
		int		GetSize() const				{ return mSize; }
		void	Clear()						{ mGuessLetter = mAnswerLetter = 0; mOpen = true; }
		
	private:
		bool	mOpen;
		int		mSize;
		char	mGuessLetter;
		char	mAnswerLetter;
		
		WordSlot	*mpAcross;
		WordSlot 	*mpDown;	
};

// ----------------------------------------------------------------------------

#endif /*CELL_H_*/

