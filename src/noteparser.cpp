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

#include "noteparser.h"
#include "settings.h"

#include <QtGui/QFont>

#include <KColorScheme>

NoteParser::NoteParser(const Document &document, QTextCursor *textCursor)
: QObject(), m_document(document), m_textCursor(textCursor)
{
}

bool NoteParser::parse()
{
    bool isFirstBlock = true;
    foreach (Paragraph paragraph, m_document.body) {
        if (!isFirstBlock) {
            if (paragraph.indentLevel == 0) {
                QTextBlockFormat blockFormat;
                blockFormat.setIndent(0);
                m_textCursor->insertBlock(blockFormat);
            } else {
                QTextListFormat::Style listStyle;
                switch (paragraph.indentLevel % 3) {
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
                listFormat.setIndent(paragraph.indentLevel);
                listFormat.setStyle(listStyle);
                m_textCursor->insertList(listFormat);
            }
        }
        foreach (Fragment fragment, paragraph.fragments) {
            QTextCharFormat charFormat;

            if (fragment.bold) {
                charFormat.setFontWeight(QFont::Bold);
            }

            if (fragment.italic) {
                charFormat.setFontItalic(true);
            }

            if (fragment.strikeThrough) {
                charFormat.setFontStrikeOut(true);
            }

            switch (fragment.fontSize) {
            case Fragment::SMALL:
                charFormat.setFontPointSize(Settings::smallFont().pointSize());
                break;
            case Fragment::NORMAL:
                charFormat.setFontPointSize(Settings::normalFont().pointSize());
                break;
            case Fragment::LARGE:
                charFormat.setFontPointSize(Settings::largeFont().pointSize());
                break;
            case Fragment::HUGE:
                charFormat.setFontPointSize(Settings::hugeFont().pointSize());
                break;
            default:
                charFormat.setFontPointSize(Settings::normalFont().pointSize());
                break;
            }

            if (fragment.highlight) {
                charFormat.setBackground(Qt::yellow);
            } else {
                charFormat.setBackground(KColorScheme(QPalette::Active, KColorScheme::View).background().color());
            }

            if (!fragment.anchorReference.isEmpty()) {
                charFormat.setAnchor(true);
                charFormat.setAnchorHref(fragment.anchorReference);
                charFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
                charFormat.setUnderlineColor(Qt::blue);
                charFormat.setForeground(Qt::blue);
            }

            if (!fragment.text.isEmpty()) {
                m_textCursor->insertText(fragment.text, charFormat);
            } else {
                m_textCursor->setBlockCharFormat(charFormat);
            }
        }
        isFirstBlock = false;

    }
    return false;
}
