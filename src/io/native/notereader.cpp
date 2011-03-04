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

#include "notereader.h"
#include "../../note/paragraph.h"
#include "../../note/fragment.h"

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QTextCursor>
#include <QTextList>
#include <QFile>

#include <KLocalizedString>
#include <KColorScheme>
#include <KStandardDirs>



NoteReader::NoteReader()
    : AbstractReader()
{}

Document NoteReader::read(QIODevice *device)
{
    if (!validateNote(device)) {
        return Document();
    }

    m_xml.setDevice(device);
    if (m_xml.readNextStartElement()) {
        if (m_xml.name() == "note" && m_xml.attributes().value("version") == "1") {
            readNote();
        } else {
            m_xml.raiseError(i18nc("@info Incorrect file type error", "The file is not a Rekollect Note version 1 file"));
        }
    }

    if (!m_xml.error()) {
        return m_document;
    } else {
        return Document();
    }
}

QString NoteReader::errorString() const
{
    return i18nc("@info Error string", "%1\nLine %2, column %3",
                m_xml.errorString(),
                m_xml.lineNumber(),
                m_xml.columnNumber());
}

void NoteReader::readNote()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "note");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "body") {
            readBody();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readBody()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "body");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "paragraph") {
            readParagraph();
        } else if (m_xml.name() == "list") {
            readList();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readParagraph()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "paragraph");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "text") {
            Paragraph paragraph;
            readText(&paragraph);
            m_document.body.append(paragraph);
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readList()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "list");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "item") {
            int indentLevel = 1;
            if (m_xml.attributes().hasAttribute("indent")) {
                indentLevel = m_xml.attributes().value("indent").toString().toInt();
            }
            Paragraph paragraph;
            paragraph.indentLevel = indentLevel;

            readItem(&paragraph);
            m_document.body.append(paragraph);
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readItem(Paragraph *list)
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "item");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "text") {
            readText(list);
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readText(Paragraph *paragraph)
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "text");

    Fragment fragment;

    QXmlStreamAttributes attributes = m_xml.attributes();

    if (attributes.hasAttribute("bold")) {
        fragment.bold = true;
    }

    if (attributes.hasAttribute("italic")) {
        fragment.italic = true;
    }

    if (attributes.hasAttribute("strikethrough")) {
        fragment.strikeThrough = true;
    }

    if (attributes.hasAttribute("size")) {
        QStringRef size = attributes.value("size");
        if (size == "small") {
            fragment.fontSize = Fragment::SMALL;
        } else if (size == "large") {
            fragment.fontSize = Fragment::LARGE;
        } else if (size == "huge") {
            fragment.fontSize = Fragment::HUGE;
        } else {
            fragment.fontSize = Fragment::NORMAL;
        }
    } else {
            fragment.fontSize = Fragment::NORMAL;
    }

    if (attributes.hasAttribute("highlight")) {
        fragment.highlight = true;
    }

    if (attributes.hasAttribute("uri")) {
        fragment.anchorReference = attributes.value("uri").toString();
    }

    QString text = m_xml.readElementText();
    if (!text.isEmpty()) {
        fragment.text = text;
    }
    paragraph->fragments.append(fragment);
}

bool NoteReader::validateNote(QIODevice *file)
{
    QString schemaFilePath = KStandardDirs::locate("data", "rekollect/note.xsd");
    QFile schemaFile(schemaFilePath);
    if (schemaFile.open(QIODevice::ReadOnly)) {
        QXmlSchema schema;
        schema.load(&schemaFile);

        if (schema.isValid()) {
            QXmlSchemaValidator validator(schema);
            if (validator.validate(file)) {
                file->seek(0);
                return true;
            } else {
                file->seek(0);
                return false;
            }
        } else {
            file->seek(0);
            return false;
        }
    } else {
        return false;
    }
}
