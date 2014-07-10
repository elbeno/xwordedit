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

#include "wordslot.h"
#include "cell.h"
#include "colors.h"

#include <gdkmm/drawable.h>
#include <gdkmm/gc.h>
#include <pangomm/layout.h>

#include <cassert>

// ----------------------------------------------------------------------------

WordSlot::WordSlot(Cell *pStartCell, int number, bool across, int gridWidth, const Cell *pLimitCell)
	: mpStartCell(pStartCell)
	, mNumber(number)
	, mAcross(across)
	, mLength(0)
	, mGridWidth(gridWidth)
	, mUncheckedPercent(-1)
	, mClue("WWWWWWWWW")
{
	Cell *pCell = mpStartCell; 
	assert(pCell->IsOpen());
	
	while (pCell < pLimitCell)
	{
		if (!pCell->IsOpen())
			break;
		++mLength;
		if (mAcross)
		{
			pCell->SetAcrossWordSlot(this);
			++pCell;
		}
		else
		{
			pCell->SetDownWordSlot(this);
			pCell += mGridWidth;
		}	
	}
}

// ----------------------------------------------------------------------------

int WordSlot::GetUncheckedPercent() const
{
	if (mUncheckedPercent == -1)
	{
		int unchecked = 0;
		Cell *pCell = mpStartCell; 	
		for (int i = 0; i < mLength; ++i)
		{
			if (mAcross)
			{
				if (!pCell->GetDownWordSlot())
					unchecked++;
				++pCell;
			}
			else
			{
				if (!pCell->GetAcrossWordSlot())
					unchecked++;				
				pCell += mGridWidth;
			}
		}
		mUncheckedPercent = (unchecked * 100) / mLength;
	}
	return mUncheckedPercent;
}

// ----------------------------------------------------------------------------

int WordSlot::GetWarningPriority() const
{
	if (mLength < 3)
		return 2;
	else if (GetUncheckedPercent() > 50)
		return 1;
	return 0;
}

// ----------------------------------------------------------------------------

const Glib::ustring WordSlot::GetWarningText() const
{
	char buffer[64];

	assert(GetWarningPriority() != 0);
	
	if (mLength < 3)
		sprintf(buffer, "%d%c: Word has fewer than 3 letters", mNumber, mAcross ? 'A' : 'D');
	else if (GetUncheckedPercent() > 50)
		sprintf(buffer, "%d%c: Word has more than 50%% unchecked letters", mNumber, mAcross ? 'A' : 'D');
		
	return Glib::ustring(buffer);

}

// ----------------------------------------------------------------------------
