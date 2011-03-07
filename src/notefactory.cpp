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

#include "notefactory.h"
#include "note.h"
#include "noteparser.h"
#include "note/document.h"
#include "io/native/notereader.h"

#include <QtGui/QTextCursor>
#include <QtCore/QUuid>
#include <QtCore/QFile>

#include <KStandardDirs>
#include <KConfigGroup>
#include <KSharedConfig>


Note *NoteFactory::createNewNote(const QString &newNoteName)
{
    QString templateFilePath = KStandardDirs::locate("data", "rekollect/template.xml");
    Note *note = loadNote(templateFilePath);

    if (note) {
        if (newNoteName.isEmpty()) {
            QTextCursor cursor = note->find("%number");
            cursor.insertText(QString("%1").arg(nextNewNoteNumber()));
        }  else {
            QTextCursor cursor(note);
            cursor.select(QTextCursor::BlockUnderCursor);
            cursor.insertText(newNoteName);
        }

        note->updateDocumentName();
        note->setFileName(createFileName());
    }
    return note;
}

Note *NoteFactory::openExistingNote(const QString& fileName)
{
    Note *note = loadNote(fileName);
    if (note) {
        note->updateDocumentName();
    }
    return note;
}

/**
 * Returns a pointer to a note on success and 0 on failure.
 */
Note* NoteFactory::loadNote(const QString& fileName)
{
    Note *note = new Note(fileName);
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    NoteReader reader;
    Document document = reader.read(&file);
    if (!document.isEmpty()) {
        QTextCursor noteTextCursor(note);
        documentToNote(document, &noteTextCursor);
    } else {
        return 0;
    }

    return note;
}

int NoteFactory::nextNewNoteNumber()
{
    if (!m_nextNewNoteNumber) {
        m_nextNewNoteNumber = loadNextNewNoteNumber();
    }

    int nextNumber = m_nextNewNoteNumber;
    ++m_nextNewNoteNumber;
    storeNextNewNoteNumber(m_nextNewNoteNumber);
    return nextNumber;
}

int NoteFactory::loadNextNewNoteNumber()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup internalDataGroup = config->group("Internal");
    return internalDataGroup.readEntry("NextUnamedWindow", 1);
}

void NoteFactory::storeNextNewNoteNumber(int nextNumber)
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup internalDataGroup = config->group("Internal");
    internalDataGroup.writeEntry("NextUnamedWindow", nextNumber);
    config->sync();
}

QString NoteFactory::createFileName()
{
    QString newFileName = QUuid::createUuid().toString();
    QString fullFilePath = KStandardDirs::locateLocal("app_notes", newFileName, true);
    if (KStandardDirs::exists(fullFilePath)) {
        return createFileName();
    } else {
        return fullFilePath;
    }
}

int NoteFactory::m_nextNewNoteNumber = 0;
