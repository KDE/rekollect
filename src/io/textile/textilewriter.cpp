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
#include "../../note.h"

#include <QtGui/QTextFrame>
#include <QtGui/QTextList>

#include <KLocalizedString>

TextileWriter::TextileWriter(QTextFrame *textFrame, QSet<QString> tags)
    : AbstractWriter(textFrame, tags)
{
}

bool TextileWriter::writeFile(QIODevice *device)
{
    m_output.setDevice(device);
    writeNote();
    return true;
}

bool TextileWriter::write(QString *string)
{
    m_output.setString(string);
    writeNote();
    return true;
}

void TextileWriter::writeNote()
{
    writeBody();
}

void TextileWriter::writeBody()
{
    QTextFrame::iterator it;
    bool inList = false;
    for (it = m_textFrame->begin(); !(it.atEnd()); ++it) {

        QTextBlock paragraph = it.currentBlock();

        QTextList *list = paragraph.textList();
        if (list == 0) {
            writeParagraph(paragraph);
        } else {
            writeItem(paragraph, list);
        }
    }
}

void TextileWriter::writeParagraph(const QTextBlock &paragraph)
{
    processTexts(paragraph);
}

void TextileWriter::writeItem(const QTextBlock &item, QTextList *list)
{
    QString list_bullets(list->format().indent(), '*');
    m_output << list_bullets << ' ';
    processTexts(item);
}

void TextileWriter::writeText(const QTextFragment &fragment)
{
    QTextCharFormat format = fragment.charFormat();

    switch (int(format.fontPointSize())) {
    case Note::SmallFont:
        break;
    case Note::NormalFont:
        break;
    case Note::LargeFont:
        m_output << "h2. ";
        break;
    case Note::HugeFont:
        m_output << "h1. ";
        break;
    }

    if (format.fontWeight() == QFont::Bold) {
        m_output << "*";
    }

    if (format.fontItalic() == true) {
        m_output << "_";
    }

    if (format.fontStrikeOut() == true) {
        m_output << "-";
    }

    if (format.isAnchor()) {
        m_output << '"';
    }

    m_output << fragment.text();

    if (format.isAnchor()) {
        m_output << "\":" << format.anchorHref() << ' ';
    }

    if (format.fontStrikeOut() == true) {
        m_output << "-";
    }

    if (format.fontItalic() == true) {
        m_output << "_";
    }

    if (format.fontWeight() == QFont::Bold) {
        m_output << "*";
    }
}

void TextileWriter::processTexts(const QTextBlock &texts)
{
    if (texts.length() == 1) {
        m_output << "\n";
    } else {
        QTextBlock::iterator it;
        for (it = texts.begin(); !(it.atEnd()); ++it) {
            QTextFragment currentText = it.fragment();
            if (currentText.isValid()) {
                writeText(currentText);
            }
        }
        m_output << "\n";
    }
}
