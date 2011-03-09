/*
    Rekollect: A note taking application
    Copyright (C) 2009, 2010, 2011  Jason Jackson <jacksonje@gmail.com>

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
#include "notecollection.h"
#include "metadata/metadata.h"
#include "note.h"
#include "datetimeitem.h"

#include <QtCore/QDateTime>

#include <KLocalizedString>

NoteCollection::NoteCollection(QObject *parent) :
    QStandardItemModel(parent), m_numberOfDisplayableColumns(2)
{
    setColumnCount(3);
    initializeHeader();
}

QString NoteCollection::fileNameAt(const QModelIndex &index) const
{
    QStandardItem *noteData = item(index.row(), FileNameColumn);
    return noteData->text();
}

void NoteCollection::addNote(Note *note)
{
    QStandardItem *documentNameItem = new QStandardItem(note->documentName());
    QStandardItem *fileNameItem = new QStandardItem(note->fileName());
    DateTimeItem *modificationTimeItem = new DateTimeItem(note->modificationTime().dateTime());
    QList<QStandardItem *> newRow;
    newRow << documentNameItem << modificationTimeItem << fileNameItem;
    appendRow(newRow);
    emit noteAdded(note->fileName(), note->documentName());
}

void NoteCollection::addNote(const MetaData &metaData)
{
    QStandardItem *documentNameItem = new QStandardItem(metaData.documentName);
    QStandardItem *fileNameItem = new QStandardItem(metaData.fileName);
    DateTimeItem *modificationTimeItem = new DateTimeItem(metaData.modificationTime.dateTime());
    QList<QStandardItem *> newRow;
    newRow << documentNameItem << modificationTimeItem << fileNameItem;
    appendRow(newRow);
    emit noteAdded(metaData.fileName, metaData.documentName);
}

bool NoteCollection::noteExists(const QString& fileName)
{
    return itemFromFileName(fileName) != 0;
}

int NoteCollection::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_numberOfDisplayableColumns;
}

QVariant NoteCollection::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical) {
        return QString();
    } else {
        return QStandardItemModel::headerData(section, orientation, role);
    }
}

void NoteCollection::clear()
{
    removeRows(0, rowCount());
}

void NoteCollection::removeNote(const QString &fileName)
{
    QStandardItem *item = itemFromFileName(fileName);
    if (item != 0) {
        removeRow(item->row());
    }
    emit noteRemoved(fileName);
}

void NoteCollection::documentNameChanged(const QString &fileName, const QString &documentName)
{
    QStandardItem *item = itemFromFileName(fileName);
    if (item != 0) {
        QModelIndex indexToChange = index(item->row(), DocumentNameColumn);
        setData(indexToChange, documentName);
    }
}

void NoteCollection::documentModified(const QString &fileName, const QDateTime &modificationTime)
{
    QStandardItem *item = itemFromFileName(fileName);
    if (item != 0) {
        QModelIndex indexToChange = index(item->row(), ModificationTimeColumn);
        setData(indexToChange, modificationTime);
    }
}

void NoteCollection::initializeHeader()
{
    setHeaderData(DocumentNameColumn, Qt::Horizontal, i18nc("@title:column Title of note", "Document Name"));
    setHeaderData(ModificationTimeColumn, Qt::Horizontal, i18nc("@title:column Last modified time of note", "Modification Time"));
    setHeaderData(FileNameColumn, Qt::Horizontal, i18nc("@title:column File name of note, not normally visible", "File Name"));
}

QStandardItem* NoteCollection::itemFromFileName(const QString &fileName)
{
    QList<QStandardItem *> items = findItems(fileName, Qt::MatchExactly, FileNameColumn);
    if (items.count()) {
        return items.takeFirst();
    } else {
        return 0;
    }

}
