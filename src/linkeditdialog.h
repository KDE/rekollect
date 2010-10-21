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

#ifndef LINKEDITDIALOG_H
#define LINKEDITDIALOG_H

#include <KDialog>

#include "ui_edit_link.h"

class NoteCollection;
class NegativeFilterProxyModel;

class LinkEditDialog : public KDialog
{
    Q_OBJECT
public:
    LinkEditDialog(NoteCollection *noteCollection, const QString &noteName,
                   const QString &currentFileName, QWidget *parent = 0);

    enum LinkType {NewNote, ExistingNote};

    LinkType linkType();
    QString linkFileName();

private:
    void setupWindow();

private slots:
    void linkToExistingNote(const QModelIndex &selectedIndex = QModelIndex());
    void linkToNewNote();

private:
    Ui::EditLinkForm ui;
    NoteCollection *m_noteCollection;
    QString m_noteName;
    QString m_currentFileName;
    NegativeFilterProxyModel *m_filteredModel;
    LinkType m_linkType;
    QString m_linkFileName;
};

#endif // LINKEDITDIALOG_H
