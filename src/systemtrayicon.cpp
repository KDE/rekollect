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
#include "systemtrayicon.h"

#include <QtCore/QSignalMapper>
#include <QtGui/QMenu>

#include <KIcon>
#include <KAction>
#include <KActionCollection>
#include <KLocalizedString>

SystemTrayIcon::SystemTrayIcon(QWidget *parent) : KSystemTrayIcon(parent)
{
    setIcon(KIcon("rekollect"));
    m_noteSignalMapper = new QSignalMapper(this);
    connect(m_noteSignalMapper, SIGNAL(mapped(QString)), SIGNAL(openNoteRequested(QString)));
    setupActions();
}

void SystemTrayIcon::addDocumentAction(const QString &fileName, const QString &documentName)
{
    KAction *documentAction = new KAction(documentName, this);
    connect(documentAction, SIGNAL(triggered()), m_noteSignalMapper, SLOT(map()));
    m_noteSignalMapper->setMapping(documentAction, fileName);
    actionCollection()->addAction(fileName, documentAction);
    contextMenu()->insertAction(m_windowCommandSeparator, documentAction);
}

void SystemTrayIcon::removeDocumentAction(const QString &fileName)
{
    QAction *documentAction = actionCollection()->action(fileName);
    contextMenu()->removeAction(documentAction);
    actionCollection()->removeAction(documentAction);
}

void SystemTrayIcon::renameDocumentAction(const QString &fileName, const QString &documentName)
{
    QAction *documentAction = actionCollection()->action(fileName);
    documentAction->setText(documentName);
}

void SystemTrayIcon::setupActions()
{
    m_windowCommandSeparator = contextMenu()->addSeparator();
    m_newNoteAction = new KAction(i18nc("@action:inmenu", "Create New Note"), this);
    connect(m_newNoteAction, SIGNAL(triggered()), SIGNAL(createNoteRequested()));
    contextMenu()->addAction(m_newNoteAction);
}
