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

#include "notereader.h"
#include "../../note.h"

#include <QXmlSchema>
#include <QXmlSchemaValidator>
#include <QTextCursor>
#include <QTextList>
#include <QFile>

#include <KLocalizedString>
#include <KColorScheme>
#include <KStandardDirs>


NoteReader::NoteReader(QTextCursor *textCursor)
    : AbstractReader(textCursor)
{}

bool NoteReader::read(QIODevice *device)
{
    if (!validateNote(device)) {
        return false;
    }

    m_xml.setDevice(device);
    if (m_xml.readNextStartElement()) {
        if (m_xml.name() == "note" && m_xml.attributes().value("version") == "1") {
            readNote();
        } else {
            m_xml.raiseError(i18nc("@info Incorrect file type error", "The file is not a Rekollect Note version 1 file"));
        }
    }

    return !m_xml.error();
}

QList< QString > NoteReader::tags() const
{
    return m_tags;
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
        } else if (m_xml.name() == "metadata") {
            readMetaData();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readMetaData()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "metadata");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "tags") {
            readTags();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readTags()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "tags");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "tag") {
            readTag();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readTag()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "tag");
    m_tags << m_xml.readElementText();
}

void NoteReader::readBody()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "body");

    bool isFirstBlock = true;
    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "paragraph") {
            if (!isFirstBlock) {
                QTextBlockFormat blockFormat;
                blockFormat.setIndent(0);
                m_textCursor->insertBlock(blockFormat);
            }
            readParagraph();
            isFirstBlock = false;
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
            readText();
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
            QTextListFormat::Style listStyle;
            switch (indentLevel % 3) {
                case 1:
                    listStyle = QTextListFormat::ListDisc;
                    break;
                case 2:
                    listStyle = QTextListFormat::ListSquare;
                    break;
                default:  // case 0:
                    listStyle = QTextListFormat::ListCircle;
                    break;
            }

            QTextListFormat listFormat;
            listFormat.setIndent(indentLevel);
            listFormat.setStyle(listStyle);
            m_textCursor->insertList(listFormat);
            readItem();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readItem()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "item");

    while (m_xml.readNextStartElement()) {
        if (m_xml.name() == "text") {
            readText();
        } else {
            m_xml.skipCurrentElement();
        }
    }
}

void NoteReader::readText()
{
    Q_ASSERT(m_xml.isStartElement() && m_xml.name() == "text");

    QXmlStreamAttributes attributes = m_xml.attributes();
    QTextCharFormat charFormat;

    if (attributes.hasAttribute("bold")) {
        charFormat.setFontWeight(QFont::Bold);
    }

    if (attributes.hasAttribute("italic")) {
        charFormat.setFontItalic(true);
    }

    if (attributes.hasAttribute("strikethrough")) {
        charFormat.setFontStrikeOut(true);
    }

    if (attributes.hasAttribute("size")) {
        QStringRef size = attributes.value("size");
        if (size == "small") {
            charFormat.setFontPointSize(Note::SmallFont);
        } else if (size == "large") {
            charFormat.setFontPointSize(Note::LargeFont);
        } else if (size == "huge") {
            charFormat.setFontPointSize(Note::HugeFont);
        } else {
            charFormat.setFontPointSize(Note::NormalFont);
        }
    } else {
        charFormat.setFontPointSize(Note::NormalFont);
    }

    if (attributes.hasAttribute("highlight")) {
        charFormat.setBackground(Qt::yellow);
    } else {
        charFormat.setBackground(KColorScheme(QPalette::Active, KColorScheme::View).background().color());
    }

    if (attributes.hasAttribute("uri")) {
        charFormat.setAnchor(true);
        charFormat.setAnchorHref(attributes.value("uri").toString());
        charFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
        charFormat.setUnderlineColor(Qt::blue);
        charFormat.setForeground(Qt::blue);
    }

    QString text = m_xml.readElementText();
    if (!text.isEmpty()) {
        m_textCursor->insertText(text, charFormat);
    } else {
        m_textCursor->setBlockCharFormat(charFormat);
    }
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
