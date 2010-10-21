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
#ifndef SYSTEMTRAYICON_H
#define SYSTEMTRAYICON_H

#include <KSystemTrayIcon>

#include <QHash>

class QAction;
class QSignalMapper;

class KAction;

class SystemTrayIcon : public KSystemTrayIcon
{
    Q_OBJECT
public:
    SystemTrayIcon(QWidget *parent = 0);

public slots:
    void addDocumentAction(const QString &fileName, const QString &documentName);
    void removeDocumentAction(const QString &fileName);
    void renameDocumentAction(const QString &fileName, const QString &documentName);

signals:
    void openNoteRequested(const QString &fileName);
    void createNoteRequested();

private:
    void setupActions();

    QHash<QString, KAction *> m_documentActions;
    QSignalMapper *m_noteSignalMapper;

    QAction *m_windowCommandSeparator;
    KAction *m_newNoteAction;
};

#endif // SYSTEMTRAYICON_H
