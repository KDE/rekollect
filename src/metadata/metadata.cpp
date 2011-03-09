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

#include "metadata.h"

#include <QtCore/QFile>
#include <QtCore/QXmlStreamReader>

#include <KFileItem>
#include <KLocalizedString>

MetaData::MetaData(const MetaData &metaData): QObject()
{
    fileName = metaData.fileName;
    documentName = metaData.documentName;
    modificationTime = metaData.modificationTime;
}

MetaData &MetaData::MetaData::operator=(const MetaData &metaData)
{
    if (this == &metaData) {
        return *this;
    }
    fileName = metaData.fileName;
    documentName = metaData.documentName;
    modificationTime = metaData.modificationTime;
    return *this;
}

MetaData noteMetaData(const QString &fileName)
{
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);

    MetaData metaData;

    QXmlStreamReader xml;
    xml.setDevice(&file);
    if (xml.readNextStartElement()) {
        if (xml.name() == "note" && xml.attributes().value("version") == "1") {
            do {
                xml.readNextStartElement();
            } while (xml.name() != "text");
            metaData.documentName = xml.readElementText();
            KFileItem item(KFileItem::Unknown, KFileItem::Unknown, KUrl(file.fileName()));
            metaData.fileName = file.fileName();
            metaData.modificationTime = item.time(KFileItem::ModificationTime);
        } else {
            xml.raiseError(i18nc("@info Incorrect file type error", "The file is not a Rekollect Note version 1 file"));
        }
    }

    return metaData;
}
