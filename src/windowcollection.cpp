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
#include "windowcollection.h"

#include "notewindow.h"

WindowCollection::WindowCollection(QObject *parent) : QObject(parent) {}

WindowCollection::~WindowCollection()
{
    foreach(NoteWindow *window, m_windows) {
        delete(window);
    }
}

void WindowCollection::addWindow(const QString &fileName, NoteWindow *noteWindow)
{
    connect(noteWindow, SIGNAL(closeRequested(QString)), SLOT(removeNoteWindow(QString)));
    m_windows.insert(fileName, noteWindow);
}

bool WindowCollection::windowExists(const QString &fileName)
{
    return m_windows.contains(fileName);
}

void WindowCollection::showWindow(const QString &fileName)
{
    NoteWindow *window = m_windows.value(fileName);
    if (window->isMinimized()) {
        window->showNormal();
    }
    window->raise();
    window->activateWindow();
}

void WindowCollection::updateOpenWindows()
{
    foreach (NoteWindow *window, m_windows) {
        window->updateNote();
    }
}

void WindowCollection::removeNoteWindow(const QString &fileName)
{
    if (m_windows.contains(fileName)) {
        m_windows.remove(fileName);
    }
}
