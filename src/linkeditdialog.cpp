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

#include "linkeditdialog.h"

#include "notecollection.h"
#include "negativefilterproxymodel.h"

#include <KLocalizedString>

LinkEditDialog::LinkEditDialog(NoteCollection *noteCollection, const QString &noteName,
                               const QString &currentFileName, QWidget *parent)
    : KDialog(parent), m_noteCollection(noteCollection), m_noteName(noteName),
      m_currentFileName(currentFileName)
{
    setWindowTitle(i18nc("@title:window", "Edit Link"));
    setupWindow();
}

LinkEditDialog::LinkType LinkEditDialog::linkType()
{
    return m_linkType;
}

QString LinkEditDialog::linkFileName()
{
    return m_linkFileName;
}

void LinkEditDialog::setupWindow()
{
    ui.setupUi(mainWidget());
    m_filteredModel = new NegativeFilterProxyModel(this);
    m_filteredModel->setSourceModel(m_noteCollection);
    m_filteredModel->setFilterKeyColumn(NoteCollection::FileNameColumn);
    m_filteredModel->setFilterFixedString(m_currentFileName);
    ui.noteListView->setModel(m_filteredModel);
    ui.noteListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui.noteListView, SIGNAL(doubleClicked(QModelIndex)), SLOT(linkToExistingNote(QModelIndex)));
    setButtons(KDialog::User1 | KDialog::User2 | KDialog::Cancel);
    setButtonText(KDialog::User1, i18nc("@action:button", "Link to Selected Note"));
    setButtonText(KDialog::User2, i18nc("@action:button", "Create New Note: \"%1\"", m_noteName));
    connect(this, SIGNAL(user1Clicked()), SLOT(linkToExistingNote()));
    connect(this, SIGNAL(user2Clicked()), SLOT(linkToNewNote()));
}

void LinkEditDialog::linkToExistingNote(const QModelIndex &selectedIndex)
{
    QModelIndex fileModelIndex;
    if (selectedIndex == QModelIndex()) {
        fileModelIndex = ui.noteListView->currentIndex();
    } else {
        fileModelIndex = selectedIndex;
    }
    fileModelIndex = m_filteredModel->mapToSource(fileModelIndex);
    m_linkFileName = m_noteCollection->fileNameAt(fileModelIndex);
    m_linkType = ExistingNote;
    accept();
}

void LinkEditDialog::linkToNewNote()
{
    m_linkType = NewNote;
    accept();
}
