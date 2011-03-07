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

#include "textilewriter.h"
#include "../../note/document.h"
#include "../../note/paragraph.h"

#include <KLocalizedString>

TextileWriter::TextileWriter()
    : AbstractWriter()
{
}

bool TextileWriter::writeFile(const Document &document, QIODevice *device)
{
    m_output.setDevice(device);
    writeNote(document);
    return true;
}

bool TextileWriter::write(const Document &document, QString *string)
{
    m_output.setString(string);
    writeNote(document);
    return true;
}

void TextileWriter::writeNote(const Document &document)
{
    foreach (Paragraph paragraph, document.body) {
        if (paragraph.indentLevel == 0) {
            writeParagraph(paragraph);
        } else {
            writeItem(paragraph);
        }
    }
}

void TextileWriter::writeParagraph(const Paragraph &paragraph)
{
    processTexts(paragraph);
}

void TextileWriter::writeItem(const Paragraph &paragraph)
{
    QString list_bullets(paragraph.indentLevel, '*');
    m_output << list_bullets << ' ';
    processTexts(paragraph);
}

void TextileWriter::writeText(const Fragment &fragment, bool atBlockStart)
{
    // Textile does not support text size changes in the middle of a block of text
    if (atBlockStart) {
        if (fragment.fontSize == Fragment::LARGE) {
            m_output << "h2. ";
        } else if (fragment.fontSize == Fragment::HUGE) {
            m_output << "h1. ";
        }
    }

    if (fragment.bold) {
        m_output << "*";
    }

    if (fragment.italic) {
        m_output << "_";
    }

    if (fragment.strikeThrough) {
        m_output << "-";
    }

    if (!fragment.anchorReference.isEmpty()) {
        if (fragment.anchorReference.startsWith("rekollect:")) {
            m_output << "[[";
        } else {
            m_output << '"';
        }
    }

    m_output << fragment.text;

    if (!fragment.anchorReference.isEmpty()) {
        if (fragment.anchorReference.startsWith("rekollect:")) {
            m_output << "]]";
        } else {
            m_output << "\":" << fragment.anchorReference << ' ';
        }
    }

    if (fragment.strikeThrough) {
        m_output << "-";
    }

    if (fragment.italic) {
        m_output << "_";
    }

    if (fragment.bold) {
        m_output << "*";
    }
}

void TextileWriter::processTexts(const Paragraph &paragraph)
{
    if (paragraph.fragments.length() > 0) {
        bool atBlockStart = true;
        foreach (Fragment fragment, paragraph.fragments) {
            writeText(fragment, atBlockStart);
            atBlockStart = false;
        }
    }
    m_output << "\n";
}
