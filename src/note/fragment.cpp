/*
    Rekollect: A note taking application
    Copyright (C) 2011  Jason Jackson <jacksonje@gmail.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*/

#include "fragment.h"

Fragment::Fragment()
{
    bold = false;
    italic = false;
    strikeThrough = false;
    highlight = false;
    fontSize = NORMAL;
}

Fragment::Fragment(const Fragment &fragment): QObject()
{
    bold = fragment.bold;
    italic = fragment.italic;
    strikeThrough = fragment.strikeThrough;
    highlight = fragment.highlight;
    fontSize = fragment.fontSize;
    anchorReference = fragment.anchorReference;
    text = fragment.text;
}