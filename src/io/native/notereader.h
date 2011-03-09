/*
    Rekollect: A note taking application
    Copyright (C) 2010, 2011  Jason Jackson <jacksonje@gmail.com>

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

#ifndef NOTEREADER_H
#define NOTEREADER_H

#include "../abstractreader.h"
#include "../../note/document.h"
#include "../../note/paragraph.h"
#include "../../metadata/metadata.h"

#include <QtCore/QXmlStreamReader>

#include <kdemacros.h>

class QTextCursor;


class KDE_EXPORT NoteReader : public AbstractReader
{
public:
    NoteReader();
    ~NoteReader() {}

    Document read(QIODevice *device);
    MetaData readMetaData(QIODevice *device);
    QString errorString() const;

private:
    void readNote();
    void readBody();
    void readParagraph();
    void readList();
    void readItem(Paragraph *list);
    void readText(Paragraph *paragraph);

    bool validateNote(QIODevice *device);

    QXmlStreamReader m_xml;
    Document m_document;
};

#endif // NOTEREADER_H
