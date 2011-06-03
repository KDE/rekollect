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
#ifndef NOTECOLLECTION_H
#define NOTECOLLECTION_H

#include <QtGui/QStandardItemModel>

class Note;
class MetaData;

class QDateTime;

class NoteCollection : public QStandardItemModel
{
    Q_OBJECT
public:
    NoteCollection(QObject *parent = 0);

    enum ColumnName {DocumentNameColumn, ModificationTimeColumn, FileNameColumn};

    QString fileNameAt(const QModelIndex &index) const;

    void addNote(Note *note);
    void addNote(const MetaData &metaData);
    bool noteExists(const QString &fileName);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    void clear();

public slots:
    void removeNote(const QString &fileName);
    void documentNameChanged(const QString &fileName, const QString &documentName);
    void documentModified(const QString &fileName, const QDateTime &modificationTime);

private:
    void initializeHeader();

    QStandardItem *itemFromFileName(const QString &fileName);

private:
    const int m_numberOfDisplayableColumns;

};

#endif // NOTECOLLECTION_H
