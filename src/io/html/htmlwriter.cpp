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

#include "htmlwriter.h"
#include "../../note/document.h"
#include "../../note/paragraph.h"

#include <KLocalizedString>

HTMLWriter::HTMLWriter()
    : AbstractWriter()
{
}

bool HTMLWriter::writeFile(const Document &document, QIODevice *device)
{
    m_output.setDevice(device);
    writeNote(document);
    return true;
}

bool HTMLWriter::write(const Document &document, QString *string)
{
    m_output.setString(string);
    writeNote(document);
    return true;
}

void HTMLWriter::writeNote(const Document &document)
{
    writeHeader(document.name);

    int previousIndentLevel = 0;
    foreach (Paragraph paragraph, document.body) {
        if (paragraph.indentLevel <= 0) {
            if (previousIndentLevel > 0) {
                for (int i = 0; i < previousIndentLevel; i++) {
                    m_output << "</li></ul>";
                }
            }
            writeParagraph(paragraph);
            previousIndentLevel = 0;
        } else {
            if (previousIndentLevel == 0) {
                m_output << "<ul>\n";
            } else if (paragraph.indentLevel > previousIndentLevel) {
                m_output << "<ul>\n";
            }
            if (paragraph.indentLevel <= previousIndentLevel) {
                m_output << "\n</li>";
            }
            if (paragraph.indentLevel < previousIndentLevel) {
                m_output << "\n</ul>";
                if (paragraph.indentLevel > 0) {
                    m_output << "</li>\n";
                }
            }
            m_output << "<li>\n";
            writeItem(paragraph);


            if (paragraph.indentLevel > previousIndentLevel) {
                previousIndentLevel++;
            } else if (paragraph.indentLevel < previousIndentLevel) {
                previousIndentLevel--;
            }

        }
    }
    writeFooter();
}

void HTMLWriter::writeParagraph(const Paragraph &paragraph)
{
    m_output << "    <p>\n";
    processTexts(paragraph);
    m_output << "    </p>\n";
}

void HTMLWriter::writeItem(const Paragraph &paragraph)
{
    processTexts(paragraph);
}

void HTMLWriter::writeText(const Fragment &fragment)
{
    m_output << "      <span style='";
    // Textile does not support text size changes in the middle of a block of text
    if (fragment.fontSize != Fragment::NORMAL) {
        m_output << "font-size: ";
        switch (fragment.fontSize) {
        case Fragment::SMALL:
            m_output << "small";
            break;
        case Fragment::LARGE:
            m_output << "large";
            break;
        case Fragment::HUGE:
            m_output << "x-large";
            break;
        default:
            m_output << "medium";
            break;
        }
        m_output << "; ";
    }

    if (fragment.strikeThrough) {
        m_output << "text-decoration: line-through; ";
    }

    if (fragment.highlight) {
        m_output << "background-color: yellow; ";
    }
    m_output << "'>";

    if (fragment.bold) {
        m_output << "<b>";
    }

    if (fragment.italic) {
        m_output << "<i>";
    }

    if (!fragment.anchorReference.isEmpty()) {
        if (!fragment.anchorReference.startsWith("rekollect:")) {
             m_output << "<a href='" << fragment.anchorReference << "'>";
        }
    }

    m_output << fragment.text;

    if (!fragment.anchorReference.isEmpty()) {
        if (!fragment.anchorReference.startsWith("rekollect:")) {
             m_output << "</a>";
        }
    }

    if (fragment.italic) {
        m_output << "</i>";
    }

    if (fragment.bold) {
        m_output << "</b>";
    }
    m_output << "</span>\n";
}

void HTMLWriter::writeHeader(const QString &title)
{
    m_output << "<html>\n"
            << "  <head><title>" << title << "</title></head>\n"
            << "  <body>\n";
}

void HTMLWriter::writeFooter()
{
    m_output << "  </body>\n"
            << "</html>\n";
}

void HTMLWriter::processTexts(const Paragraph &paragraph)
{
    if (paragraph.fragments.length() > 0) {
        foreach (Fragment fragment, paragraph.fragments) {
            writeText(fragment);
        }
    }
    m_output << "\n";
}
