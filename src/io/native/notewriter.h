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

#ifndef NOTEWRITER_H
#define NOTEWRITER_H

#include "../abstractwriter.h"

#include <QtCore/QXmlStreamWriter>

#include <kdemacros.h>

class Document;
class Paragraph;
class Fragment;
class QTextFrame;
class QTextBlock;
class QTextList;
class QTextFragment;

class KDE_EXPORT NoteWriter : public AbstractWriter
{
public:
    NoteWriter();
    virtual ~NoteWriter() {}

    bool writeFile(const Document &document, QIODevice *device);

private:
    void writeNote(const Document &document);
    void writeBody(const Document &document);
    void writeParagraph(const Paragraph &paragraph);
    void writeItem(const Paragraph &item);
    void writeText(const Fragment &fragment);

    void processTexts(const Paragraph &paragraph);

    QXmlStreamWriter m_xml;
};

#endif // NOTEWRITER_H
