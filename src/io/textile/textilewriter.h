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

#ifndef TEXTILEWRITER_H
#define TEXTILEWRITER_H

#include "../abstractwriter.h"

#include <QtCore/QTextStream>

#include <kdemacros.h>

class QTextFrame;
class QTextBlock;
class QTextList;
class QTextFragment;

class KDE_EXPORT TextileWriter : public AbstractWriter
{
public:
    explicit TextileWriter(QTextFrame *textFrame, QSet<QString> tags = QSet<QString>());
    ~TextileWriter() {}

    bool writeFile(QIODevice *device);
    bool write(QString *string);

private:
    void writeNote();
    void writeParagraph(const QTextBlock &paragraph);
    void writeItem(const QTextBlock &item, QTextList *list);
    void writeText(const QTextFragment &fragment, bool atBlockStart);

    void processTexts(const QTextBlock &texts);

    QTextStream m_output;
};

#endif // NOTEWRITER_H
