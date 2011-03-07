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
#ifndef WINDOWCOLLECTION_H
#define WINDOWCOLLECTION_H

#include "notewindow.h"

#include <QtCore/QObject>
#include <QtCore/QHash>

class Note;

class WindowCollection : public QObject
{
    Q_OBJECT
public:
    explicit WindowCollection(QObject *parent = 0);
    ~WindowCollection();

    void addWindow(const QString &fileName, NoteWindow *window);
    bool windowExists(const QString &fileName);
    void showWindow(const QString &fileName);

public slots:
    void updateOpenWindows();

private slots:
    void removeNoteWindow(const QString &fileName);

private:
    QHash<QString, NoteWindow *> m_windows; // QString is the fileName
};



#endif // WINDOWCOLLECTION_H
