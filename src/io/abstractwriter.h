/*
    Rekollect: A note taking application
    Copyright (C) 2010  Jason Jackson <jacksonje@gmail.com>

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

#ifndef ABSTRACTWRITER_H
#define ABSTRACTWRITER_H

#include <QtCore/QSet>
#include <QtCore/QString>

class QIODevice;
class QTextFrame;

class AbstractWriter
{
public:
    AbstractWriter(QTextFrame *textFrame) { m_textFrame = textFrame; }
    virtual bool writeFile(QIODevice *device) = 0;
    virtual ~AbstractWriter() {}

protected:
    QTextFrame *m_textFrame;
};

#endif // ABSTRACTWRITER_H
