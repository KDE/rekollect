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
#ifndef NOTE_H
#define NOTE_H

#include <QtGui/QTextDocument>

#include <KDateTime>

class NoteCollection;

class Note : public QTextDocument
{
    Q_OBJECT
public:
    explicit Note(const QString &noteFileName, QObject *parent=0);

    void setFileName(const QString &fileName);
    QString fileName() const;

    void setDocumentName(const QString &documentName);
    QString documentName() const;

    KDateTime modificationTime() const;

public slots:
    void updateDocumentName();

signals:
    void documentNameChanged(const QString &fileName, const QString &documentName);
    void documentModified(const QString &fileName, const QDateTime &modificationTime);

private:
    QString parseDocumentName() const;

    QString m_fileName;
    QString m_documentName;
};

#endif // NOTE_H
