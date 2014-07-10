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

#include "cell.h"
#include "wordslot.h"
#include "colors.h"

#include <gdkmm/drawable.h>
#include <pangomm/layout.h>
#include <pangomm/rectangle.h>

#include <cassert>
#include <stdio.h>

// ----------------------------------------------------------------------------

Cell::Cell(int size)
	: mOpen(true)
	, mSize(size)
	, mGuessLetter(0)
	, mAnswerLetter(0)
	, mpAcross(0)
	, mpDown(0)
{
}

// ----------------------------------------------------------------------------
