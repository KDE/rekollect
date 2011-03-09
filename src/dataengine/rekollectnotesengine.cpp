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

#include "rekollectnotesengine.h"

#include <QtCore/QFile>

#include <KDirWatch>
#include <KStandardDirs>
#include <KFileItem>

RekollectNotesEngine::RekollectNotesEngine(QObject *parent, const QVariantList &args)
: Plasma::DataEngine(parent, args)
{
    Q_UNUSED(args)
    setMinimumPollingInterval(333);
    m_dirWatch = new KDirWatch(this);
}

QStringList RekollectNotesEngine::sources() const
{
    QStringList sourcesList;
    foreach (const MetaData metaData, m_noteMetaData) {
        sourcesList << metaData.fileName;
    }
    sourcesList << "DEBUG";
    return sourcesList;
}

void RekollectNotesEngine::init()
{
    #ifndef DEBUG
    KGlobal::dirs()->addResourceType("app_notes", "data", "rekollect/notes");
    #else
    KGlobal::dirs()->addResourceType("app_notes", "data", "rekollect/notesdev");
    #endif
    loadNoteMetaData();
    startDirWatch();
}

bool RekollectNotesEngine::sourceRequestEvent(const QString &name)
{
    return updateSourceEvent(name);
}

bool RekollectNotesEngine::updateSourceEvent(const QString &name)
{
    if (m_noteMetaData.contains(name)) {
        setData(name, I18N_NOOP("File Name"), m_noteMetaData.value(name).fileName);
        setData(name, I18N_NOOP("Document Name"), m_noteMetaData.value(name).documentName);
        setData(name, I18N_NOOP("Modification Time"), m_noteMetaData.value(name).modificationTime.dateTime());
        return true;
    } else {
        return false;
    }
}

void RekollectNotesEngine::loadNoteMetaData()
{
    QStringList noteList = KGlobal::dirs()->findAllResources("app_notes", QString());
    foreach (const QString &notePath, noteList) {
        MetaData metaData = noteMetaData(notePath);
        if (!metaData.documentName.isEmpty()) {
            m_noteMetaData.insert(metaData.fileName, metaData);
        }
    }
}

void RekollectNotesEngine::startDirWatch()
{
    QStringList noteDirs = KGlobal::dirs()->findDirs("app_notes", QString());
    foreach (const QString &noteDir, noteDirs) {
        m_dirWatch->addDir(noteDir, KDirWatch::WatchDirOnly);
    }
    connect(m_dirWatch, SIGNAL(dirty(QString)), this, SLOT(reloadNoteMetaData()));
}

void RekollectNotesEngine::reloadNoteMetaData()
{
    m_noteMetaData.clear();
    loadNoteMetaData();
    updateSourceEvent("notes");
    emit scheduleSourcesUpdated();
}

K_EXPORT_PLASMA_DATAENGINE(rekollectnotes, RekollectNotesEngine)

#include "rekollectnotesengine.moc"