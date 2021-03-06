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

#include "rekollectapplication.h"

#include "notebrowserwindow.h"
#include "settings.h"

#include <QtCore/QFileInfo>

#include <KStandardDirs>
#include <KCmdLineArgs>


RekollectApplication::RekollectApplication()
    : m_noteBrowserWindow(0)
{}

RekollectApplication::~RekollectApplication()
{
    delete m_noteBrowserWindow;
}

int RekollectApplication::newInstance()
{
    if (!m_noteBrowserWindow) {
        // Add a directory to the resources for saving notes
        #ifndef DEBUG
        KGlobal::dirs()->addResourceType("app_notes", "appdata", "notes");
        #else
        KGlobal::dirs()->addResourceType("app_notes", "appdata", "notesdev");
        #endif

        m_noteBrowserWindow = new NoteBrowserWindow;
    }

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    if (args->isSet("new")) {
        m_noteBrowserWindow->createNewNote();
        return 0;
    } else if (args->isSet("browse")) {
        m_noteBrowserWindow->showBrowserWindow();
    }

    if (args->count() == 0) {
        m_noteBrowserWindow->showBrowserWindow();
    } else {
        QString fullFilePath;
        QString noteFileName = args->arg(0);
        if (noteFileName.startsWith("/")) {
            fullFilePath = noteFileName;
        } else {
            noteFileName = noteFileName.section("/", -1);
            fullFilePath = KStandardDirs::locateLocal("app_notes", noteFileName);
        }

        QFileInfo info(fullFilePath);
        if (info.exists() && info.isFile() && info.isReadable()) {
            m_noteBrowserWindow->openNote(fullFilePath);
        } else {
            m_noteBrowserWindow->showBrowserWindow();
        }
    }

    return 0;
}
