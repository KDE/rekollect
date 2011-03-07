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
#include "../../note/document.h"
#include "../../note/paragraph.h"

#include <KLocalizedString>

NoteWriter::NoteWriter() : AbstractWriter()
{
    m_xml.setAutoFormatting(true);
}

bool NoteWriter::writeFile(const Document &document, QIODevice *device)
{
    m_xml.setDevice(device);
    m_xml.writeStartDocument();
    writeNote(document);
    return true;
}

void NoteWriter::writeNote(const Document &document)
{
    m_xml.writeStartElement("note");
    m_xml.writeAttribute("version", "1");
    writeBody(document);
    m_xml.writeEndElement();
    m_xml.writeEndDocument();
}

void NoteWriter::writeBody(const Document &document)
{
    m_xml.writeStartElement("body");
    bool inList = false;
    foreach (Paragraph paragraph, document.body) {
        if (paragraph.indentLevel == 0) {
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
            writeItem(paragraph);
        }
    }
    m_xml.writeEndElement();
}

void NoteWriter::writeParagraph(const Paragraph &paragraph)
{
    m_xml.writeStartElement("paragraph");
    processTexts(paragraph);
    m_xml.writeEndElement();
}

void NoteWriter::writeItem(const Paragraph &paragraph)
{
    m_xml.writeStartElement("item");
    m_xml.writeAttribute("indent", QString("%1").arg(paragraph.indentLevel));
    processTexts(paragraph);
    m_xml.writeEndElement();
}

void NoteWriter::writeText(const Fragment &fragment)
{
    m_xml.writeStartElement("text");
    if (fragment.bold) {
        m_xml.writeAttribute("bold", "true");
    }

    if (fragment.italic) {
        m_xml.writeAttribute("italic", "true");
    }

    if (fragment.strikeThrough) {
        m_xml.writeAttribute("strikethrough", "true");
    }

    switch (fragment.fontSize) {
    case Fragment::SMALL:
        m_xml.writeAttribute("size", "small");
        break;
    case Fragment::NORMAL:
        m_xml.writeAttribute("size", "normal");
        break;
    case Fragment::LARGE:
        m_xml.writeAttribute("size", "large");
        break;
    case Fragment::HUGE:
        m_xml.writeAttribute("size", "huge");
        break;
    default:
        m_xml.writeAttribute("size", "normal");
        break;
    }

    if (fragment.highlight) {
        m_xml.writeAttribute("highlight", "true");
    }

    if (!fragment.anchorReference.isEmpty()) {
        m_xml.writeAttribute("uri", fragment.anchorReference);
    }

    m_xml.writeCharacters(fragment.text);
    m_xml.writeEndElement();
}

void NoteWriter::processTexts(const Paragraph &paragraph)
{
    if (paragraph.fragments.length() == 0) {
        m_xml.writeEmptyElement("text");
    } else {
        foreach (Fragment fragment, paragraph.fragments) {
            writeText(fragment);
        }
    }
}
