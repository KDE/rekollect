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

#include "notewriter.h"
#include "../../note.h"

#include <QtGui/QTextFrame>
#include <QtGui/QTextList>

#include <KLocalizedString>

NoteWriter::NoteWriter(QTextFrame *textFrame, QSet<QString> tags)
    : AbstractWriter(textFrame, tags)
{
    m_xml.setAutoFormatting(true);
}

bool NoteWriter::writeFile(QIODevice *device)
{
    m_xml.setDevice(device);
    m_xml.writeStartDocument();
    writeNote();
    return true;
}

void NoteWriter::writeNote()
{
    m_xml.writeStartElement("note");
    m_xml.writeAttribute("version", "1");
    writeMetaData();
    writeBody();
    m_xml.writeEndElement();
    m_xml.writeEndDocument();
}

void NoteWriter::writeMetaData()
{
    m_xml.writeStartElement("metadata");
    writeTags();
    m_xml.writeEndElement();
}

void NoteWriter::writeTags()
{
    m_xml.writeStartElement("tags");
    foreach (const QString &tag, m_tags) {
        writeTag(tag);
    }
    m_xml.writeEndElement();
}

void NoteWriter::writeTag(const QString &tag)
{
    m_xml.writeStartElement("tag");
    m_xml.writeCharacters(tag);
    m_xml.writeEndElement();
}

void NoteWriter::writeBody()
{
    m_xml.writeStartElement("body");
    QTextFrame::iterator it;
    bool inList = false;
    for (it = m_textFrame->begin(); !(it.atEnd()); ++it) {

        QTextBlock paragraph = it.currentBlock();

        QTextList *list = paragraph.textList();
        if (list == 0) {
            if (inList) {
                m_xml.writeEndElement();
                inList = false;
            }
            writeParagraph(paragraph);
        } else {
            if (!inList) {
                m_xml.writeStartElement("list");
            }
            inList = true;
            writeItem(paragraph, list);
        }
    }
    m_xml.writeEndElement();
}

void NoteWriter::writeParagraph(const QTextBlock &paragraph)
{
    m_xml.writeStartElement("paragraph");
    processTexts(paragraph);
    m_xml.writeEndElement();
}

void NoteWriter::writeItem(const QTextBlock &item, QTextList *list)
{
    m_xml.writeStartElement("item");
    m_xml.writeAttribute("indent", QString("%1").arg(list->format().indent()));
    processTexts(item);
    m_xml.writeEndElement();
}

void NoteWriter::writeText(const QTextFragment &fragment)
{
    m_xml.writeStartElement("text");
    QTextCharFormat format = fragment.charFormat();
    if (format.fontWeight() == QFont::Bold) {
        m_xml.writeAttribute("bold", "true");
    }

    if (format.fontItalic() == true) {
        m_xml.writeAttribute("italic", "true");
    }

    if (format.fontStrikeOut() == true) {
        m_xml.writeAttribute("strikethrough", "true");
    }

    switch (int(format.fontPointSize())) {
    case Note::SmallFont:
        m_xml.writeAttribute("size", "small");
        break;
    case Note::NormalFont:
        m_xml.writeAttribute("size", "normal");
        break;
    case Note::LargeFont:
        m_xml.writeAttribute("size", "large");
        break;
    case Note::HugeFont:
        m_xml.writeAttribute("size", "huge");
        break;
    }

    if (format.background() == Qt::yellow) {
        m_xml.writeAttribute("highlight", "true");
    }

    if (format.isAnchor()) {
        m_xml.writeAttribute("uri", format.anchorHref());
    }

    m_xml.writeCharacters(fragment.text());
    m_xml.writeEndElement();
}

void NoteWriter::processTexts(const QTextBlock &texts)
{
    if (texts.length() == 1) {
        m_xml.writeEmptyElement("text");
    } else {
        QTextBlock::iterator it;
        for (it = texts.begin(); !(it.atEnd()); ++it) {
            QTextFragment currentText = it.fragment();
            if (currentText.isValid()) {
                writeText(currentText);
            }
        }
    }
}
