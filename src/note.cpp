/*
    Rekollect: A note taking application
    Copyright (C) 2009, 2010  Jason Jackson <jacksonje@gmail.com>

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
#include "note.h"

#include <QtGui/QTextBlock>

#include <KFileItem>


Note::Note(const QString &noteFileName, QObject *parent)
    : QTextDocument(parent)
{
    m_fileName = noteFileName;
}

void Note::setFileName(const QString& fileName)
{
    m_fileName = fileName;
}


QString Note::fileName() const
{
    return m_fileName;
}

void Note::setDocumentName(const QString &documentName)
{
    m_documentName = documentName;
}

QString Note::documentName() const
{
        return m_documentName;
}

KDateTime Note::modificationTime() const
{
    KFileItem item(KFileItem::Unknown, KFileItem::Unknown, KUrl(m_fileName));
    return item.time(KFileItem::ModificationTime);
}

void Note::updateDocumentName()
{
    QString currentDocumentName = parseDocumentName();
    if (currentDocumentName != m_documentName) {
        m_documentName = currentDocumentName;
        emit documentNameChanged(m_fileName, m_documentName);
    }
}

QString Note::parseDocumentName() const
{
    QTextBlock firstTextBlock = firstBlock();
    while (firstTextBlock.length() == 0) {
        firstTextBlock = firstTextBlock.next();
    }
    return firstTextBlock.text();
}
