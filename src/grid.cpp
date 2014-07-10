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

#include "grid.h"
#include "wordslot.h"
#include "colors.h"
#include "signalserver.h"

#include <gdkmm/drawable.h>
#include <gtkmm/messagedialog.h>
#include <fstream>
#include <list>
#include <string>
#include <cassert>

using namespace std;

// ----------------------------------------------------------------------------

Grid::Grid(int rows, int cols, int symmetry)
	: mRows(rows)
	, mCols(cols)
	, mSymmetry(symmetry)
	, mDirty(false)
{
	assert(mSymmetry != ROTATIONAL_90 || rows == cols);
	mCells = new Cell[rows * cols];
	ResetWordSlots();
	CalculateWordSlots();	
}

Grid::~Grid()
{
	delete[] mCells;
}

// ----------------------------------------------------------------------------

Cell *Grid::GetCellAt(int x_pixel, int y_pixel) const
{
	--x_pixel;
	--y_pixel;
	if (x_pixel < 0) x_pixel = 0;
	if (y_pixel < 0) y_pixel = 0;
	
	int col = x_pixel / mCells->GetSize();
	int row = y_pixel / mCells->GetSize();

	if (col >= mCols) col = mCols-1;
	if (row >= mRows) row = mRows-1;
	
	return &mCells[row * mCols + col];
}

// ----------------------------------------------------------------------------

void Grid::GetXYPos(const Cell *pCell, int &x_pixel, int &y_pixel) const
{
	assert(pCell >= mCells && pCell < &mCells[mRows * mCols]);
	
    int cellSize = mCells->GetSize();
	x_pixel = ((pCell - mCells) % mCols) * cellSize;
	y_pixel = ((pCell - mCells) / mCols) * cellSize;
}

// ----------------------------------------------------------------------------

void Grid::ToggleCell(int x_pixel, int y_pixel)
{
	--x_pixel;
	--y_pixel;
	if (x_pixel < 0) x_pixel = 0;
	if (y_pixel < 0) y_pixel = 0;
	
	int col = x_pixel / mCells->GetSize();
	int row = y_pixel / mCells->GetSize();

	if (col >= mCols) col = mCols-1;
	if (row >= mRows) row = mRows-1;
	
	mCells[row * mCols + col].Toggle();
	switch (mSymmetry)
	{
		case LATERAL_X:
			if (col != mCols-1-col)
			{
				mCells[row * mCols + (mCols-1-col)].Toggle();
				col = std::min(col, mCols-1-col);
			}
			break;
		case LATERAL_Y:
			if (row != mRows-1-row)
			{
				mCells[(mRows-1-row) * mCols + col].Toggle();
				row = std::min(row, mRows-1-row);
			}
			break;
		case LATERAL_XY:
			if (col != mCols-1-col)
			{
				mCells[row * mCols + (mCols-1-col)].Toggle();
				if (row != mRows-1-row)
				{
					mCells[(mRows-1-row) * mCols + col].Toggle();
					mCells[(mRows-1-row) * mCols + (mCols-1-col)].Toggle();
				}
			}
			else
			{
				if (row != mRows-1-row)
				{
					mCells[(mRows-1-row) * mCols + col].Toggle();
				}
			}
			row = std::min(row, mRows-1-row);
			col = std::min(col, mCols-1-col);
			break;
		case ROTATIONAL_180:
			if (col != mCols-1-col || row != mRows-1-row)
			{
				mCells[(mRows-1-row) * mCols + (mCols-1-col)].Toggle();
				row = std::min(row, mRows-1-row);
				col = std::min(col, mCols-1-col);
			}		
			break;
		case ROTATIONAL_90:
			if (col != mCols-1-col || row != mRows-1-row)
			{
				int r = row;
				int c = col;
				mCells[col * mCols + (mCols-1-row)].Toggle();
				if (col < row)
				{
					r = col;
					c = (mCols-1-row);
				}				
				mCells[(mRows-1-row) * mCols + (mCols-1-col)].Toggle();
				if ((mRows-1-row) < r)
				{
					r = (mRows-1-row);
					c = (mCols-1-col);
				}				
				mCells[(mCols-1-col) * mCols + row].Toggle();
				if ((mCols-1-col) < r)
				{
					r = (mCols-1-col);
					c = row;
				}				
				row = r;
				col = c;
			}		
			break;
		default:
			break;
	}

	ResetWordSlots();
	CalculateWordSlots();

	mDirty = true;
	gSignalServer.signal_grid_dirty().emit(true);
	gSignalServer.signal_grid_redraw().emit();
}

// ----------------------------------------------------------------------------

void Grid::ResetWordSlots()
{
	mSlotNumber = 1;
	for (std::list<WordSlot *>::iterator i = mWordSlots.begin(); i != mWordSlots.end(); ++i)
		delete *i;
	mWordSlots.clear();
	
	for (Cell *pCell = mCells; pCell < &mCells[mRows * mCols]; ++pCell)
	{
		pCell->SetAcrossWordSlot(0);
		pCell->SetDownWordSlot(0);
	}
}

void Grid::CalculateWordSlots(int startrow, int startcol)
{
	assert(startrow >= 0 && startrow < mRows);
	assert(startcol >= 0 && startcol < mCols);

	// start searching for slots at the given row and column
	Cell *pCell = &mCells[startrow * mCols + startcol];
	for (int i = startrow; i < mRows; ++i)
	{
		for (int j = startcol; j < mCols; ++j)
		{
			if (pCell->IsOpen())
			{
				bool slot_start = false;
				bool above_open = (i != 0 && (pCell-mCols)->IsOpen());
				bool below_open = (i != mRows-1 && (pCell+mCols)->IsOpen());
				bool left_open = (j != 0 && (pCell-1)->IsOpen());
				bool right_open = (j != mCols-1 && (pCell+1)->IsOpen());
				
				if (right_open && !left_open)
				{						
					WordSlot *pws = new WordSlot(pCell, mSlotNumber, true, mCols, &mCells[(i+1) * mCols]);
					mWordSlots.push_back(pws);
					slot_start = true;
				}
				if (below_open && !above_open)
				{
					WordSlot *pws = new WordSlot(pCell, mSlotNumber, false, mCols, &mCells[mRows * mCols]);
					mWordSlots.push_back(pws);
					slot_start = true;
				}
				else if (!(left_open || right_open || above_open || below_open))
				{
					WordSlot *pws = new WordSlot(pCell, mSlotNumber, false, mCols, pCell+1);
					mWordSlots.push_back(pws);
					slot_start = true;
				}		
				
				if (slot_start)
				{
					++mSlotNumber;
				}
			}
			++pCell;
		}
		// next row we start from col 0
		startcol = 0;
	}	
}

// ----------------------------------------------------------------------------

void Grid::Clear()
{
	for (Cell *pCell = mCells; pCell < &mCells[mRows * mCols]; ++pCell)
		pCell->Clear();
	ResetWordSlots();
	CalculateWordSlots();

	mDirty = false;
	gSignalServer.signal_grid_dirty().emit(false);
}

// ----------------------------------------------------------------------------

bool Grid::Load(const char *filename)
{
	ifstream is(filename);
	
	if (!is.is_open())
		return false;
	
	is >> mRows;
	is >> mCols;
	is >> mSymmetry;
	
	delete[] mCells;
	mCells = new Cell[mRows * mCols];

	bool open;
	char letter;
	for (Cell *pCell = mCells; pCell != &mCells[mRows * mCols]; ++pCell)
	{
		is >> open;
		is >> letter;
		if (!open)
			pCell->Toggle();
		pCell->SetAnswerLetter(letter);
	}
	is.close();

	ResetWordSlots();
	CalculateWordSlots();	

	mDirty = false;
	gSignalServer.signal_grid_dirty().emit(false);
	gSignalServer.signal_grid_redraw().emit();
		
	return true;
}

// ----------------------------------------------------------------------------

bool Grid::Save(const char *filename)
{
	ofstream os(filename);
	
	if (!os.is_open())
		return false;
		
	os << mRows << endl;
	os << mCols << endl;
	os << mSymmetry << endl;

	for (Cell *pCell = mCells; pCell != &mCells[mRows * mCols]; ++pCell)
	{
		os << pCell->IsOpen() << endl;
		os << pCell->GetAnswerLetter() << endl;
	}
	os.close();
	
	mDirty = false;
	gSignalServer.signal_grid_dirty().emit(false);
	
	return true;
}

// ----------------------------------------------------------------------------

static int GetLine(FILE* pFile, std::string& s)
{
	s = "";
	if (!pFile)
		return EOF;
		
	char c = fgetc(pFile);
	while (c != EOF && c != '\n' && c != 0)
	{
		s.push_back(c);
		c = fgetc(pFile);
	}

	if (c == EOF)
		return EOF;
	else
		return s.length();
}

bool Grid::LoadPUZ(const char *filename)
{
	FILE* fp = fopen(filename, "r");
	if (!fp) return false;

	// puzzle size
	int nResult = fseek(fp, 0x2C, SEEK_SET);
	if (nResult == -1)
	{
		fclose(fp);
		return false;
	}

	mRows = fgetc(fp);
	mCols = fgetc(fp);
	mSymmetry = NONE;

	delete[] mCells;
	mCells = new Cell[mRows * mCols];

	// answer grid
	nResult = fseek(fp, 0x34, SEEK_SET);
	if (nResult == -1)
	{
		fclose(fp);
		return false;
	}
	
	char *pString = new char[mCols + 1];
	pString[mCols] = 0;
	std::list<std::string> answers;
	for (int i = 0; i < mRows; i++)
	{
		int nRead = fread(pString, 1, mCols, fp);
		if (nRead != mCols)
		{
			fclose(fp);
			return false;
		}

		char *pLetter = pString;
		Cell *pCell = &mCells[i*mCols];
		for (int j = 0; j < mCols; ++j, ++pCell, ++pLetter)
		{
			if (!isalpha(*pLetter))
				pCell->Toggle();
			else
				pCell->SetAnswerLetter(*pLetter);
		}
	}
	
	ResetWordSlots();
	CalculateWordSlots();	
	
	// puzzle grid
	for (int i = 0; i < mRows; i++)
	{
		int nRead = fread(pString, 1, mCols, fp);
		if (nRead != mCols)
		{
			fclose(fp);
			return false;
		}
		
		char *pLetter = pString;
		Cell *pCell = &mCells[i*mCols];
		for (int j = 0; j < mCols; ++j, ++pCell, ++pLetter)
		{
			if (isalpha(*pLetter))
			{
				assert(pCell->IsOpen());
				pCell->SetGuessLetter(*pLetter);
			}
			else if (*pLetter == '-')
			{
				assert(pCell->IsOpen());
				pCell->SetGuessLetter(0);
			}
			else
			{
				assert(*pLetter == '.');
				assert(!pCell->IsOpen());
			}
		}		
	}
	delete[] pString;
	
	// puzzle headers
	std::string title;
	std::string author;
	std::string copyright;
	GetLine(fp, title);
	GetLine(fp, author);
	GetLine(fp, copyright);

	// clue strings
	std::string s;
	std::list<WordSlot *>::iterator i = mWordSlots.begin();
	while (GetLine(fp, s) != EOF)
	{
		if (i != mWordSlots.end())
			(*i)->SetClue(s.c_str());
		++i;
	}
	
	fclose(fp);	
	
	mDirty = false;	
	gSignalServer.signal_grid_dirty().emit(false);
	gSignalServer.signal_grid_redraw().emit();
	
	return true;
}

// ----------------------------------------------------------------------------

bool Grid::SavePUZ(const char *filename)
{
	Gtk::MessageDialog dialog("Saving in .PUZ format is not yet implemented", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
	dialog.run();
	return true;
}

// ----------------------------------------------------------------------------

bool Grid::LoadXML(const char *filename)
{
	Gtk::MessageDialog dialog("Loading XML format is not yet implemented", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
	dialog.run();
	return true;
}

// ----------------------------------------------------------------------------

bool Grid::SaveXML(const char *filename)
{
	Gtk::MessageDialog dialog("Saving in XML format not yet implemented", false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK);
	dialog.run();
	return true;
}

// ----------------------------------------------------------------------------
