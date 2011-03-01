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

#include "noteeditor.h"
#include "note.h"
#include "settings.h"

#include <QtGui/QTextList>
#include <QtGui/QMouseEvent>
#include <QtGui/QMenu>

#include <KColorScheme>
#include <KLocalizedString>
#include <KAction>
#include <KDebug>

NoteEditor::NoteEditor(Note *note, QWidget *parent) : KTextEdit(parent)
{
    setDocument(note);
    enableFindReplace(true);
    setAutoFormatting(AutoBulletList);
    setTabStopWidth();
    setupActions();
    setLineMargin(2);
}

void NoteEditor::setDocument(Note* note)
{
    m_document = note;
    note->setParent(this);
    QTextEdit::setDocument(note);
    connect(this, SIGNAL(textChanged()), m_document, SLOT(updateDocumentName()));
}

Note *NoteEditor::document() const
{
    return m_document;
}

QString NoteEditor::currentLinkText() const
{
    QTextCursor cursor = textCursor();
    QTextCharFormat originalCharFormat = cursor.charFormat();
    if (!cursor.hasSelection()) {
        if (originalCharFormat.isAnchor()) {
            selectLinkText(&cursor);
        } else {
            cursor.select(QTextCursor::WordUnderCursor);
        }
    }
    return cursor.selectedText();
}

void NoteEditor::setLineMargin(qreal margin)
{
    m_lineMargin = margin;
    QTextBlockFormat lineSpacingFormat;
    lineSpacingFormat.setBottomMargin(margin);
    QTextCursor cursor = textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(lineSpacingFormat);
}

qreal NoteEditor::lineMargin()
{
    return m_lineMargin;
}

void NoteEditor::updateLink(const QString& linkUrl)
{
    QTextCursor cursor = textCursor();
    QTextCharFormat originalCharFormat = cursor.charFormat();
    QTextCharFormat linkFormat;
    if (linkUrl.isEmpty()) {
        if (originalCharFormat.isAnchor()) {
            if (!cursor.hasSelection()) {
                selectLinkText(&cursor);
            }
            clearLinkFormat();
        }
    } else {
        if (originalCharFormat.isAnchor()) {
            selectLinkText(&cursor);
            linkFormat.setAnchorHref(linkUrl);
            cursor.mergeCharFormat(linkFormat);
        } else {
            if (!cursor.hasSelection()) {
                cursor.select(QTextCursor::WordUnderCursor);
            }
            linkFormat.setAnchor(true);
            linkFormat.setAnchorHref(linkUrl);
            linkFormat.setUnderlineStyle(QTextCharFormat::SingleUnderline);
            linkFormat.setUnderlineColor(KColorScheme(QPalette::Active, KColorScheme::View).foreground(KColorScheme::LinkText).color());
            linkFormat.setForeground(KColorScheme(QPalette::Active, KColorScheme::View).foreground(KColorScheme::LinkText).color());
            cursor.mergeCharFormat(linkFormat);
            if (cursor.atBlockEnd()) {
                cursor.setPosition(cursor.selectionEnd());
                cursor.setCharFormat(originalCharFormat);
                cursor.insertText(" ");
            }
        }
    }
}

void NoteEditor::removeCurrentLink()
{
    updateLink(QString());
}

void NoteEditor::setTextHighlight(bool highlight)
{
    QTextCharFormat highlightFormat;
    if (highlight) {
        highlightFormat.setBackground(Qt::yellow);
    } else {
        highlightFormat.setBackground(KColorScheme(QPalette::Active, KColorScheme::View).background().color());
    }
    mergeCharFormatToSelectionOrWord(highlightFormat);
}

void NoteEditor::setTextBold(bool bold)
{
    QTextCharFormat boldFormat;
    if (bold) {
        boldFormat.setFontWeight(QFont::Bold);
    } else {
        boldFormat.setFontWeight(QFont::Normal);
    }
    mergeCharFormatToSelectionOrWord(boldFormat);
}

void NoteEditor::setTextItalic(bool italic)
{
    QTextCharFormat italicFormat;
    italicFormat.setFontItalic(italic);
    mergeCharFormatToSelectionOrWord(italicFormat);
}

void NoteEditor::setTextStrikeOut(bool strikeOut)
{
    QTextCharFormat strikeOutFormat;
    strikeOutFormat.setFontStrikeOut(strikeOut);
    mergeCharFormatToSelectionOrWord(strikeOutFormat);
}

void NoteEditor::clearLinkFormat()
{
    mergeCharFormatToSelectionOrWord(createLinkClearFormat());
}

void NoteEditor::resetTextFormat()
{
    textCursor().beginEditBlock();
    resetFontSize();
    setTextBold(false);
    setTextItalic(false);
    setTextStrikeOut(false);
    setTextHighlight(false);
    clearLinkFormat();
    textCursor().endEditBlock();
}

void NoteEditor::setFontSize(int fontSize)
{
    QTextCharFormat fontSizeFormat;
    fontSizeFormat.setFontPointSize(fontSize);
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection()) {
        cursor.select(QTextCursor::LineUnderCursor);
        cursor.mergeCharFormat(fontSizeFormat);
    } else {
        mergeCharFormatToSelectionOrWord(fontSizeFormat);
    }
}

void NoteEditor::increaseFontSize()
{
    if (fontPointSize() == Settings::smallFont().pointSize()) {
        setFontSize(Settings::normalFont().pointSize());
    } else if (fontPointSize() == Settings::normalFont().pointSize()) {
        setFontSize(Settings::largeFont().pointSize());
    } else if (fontPointSize() == Settings::largeFont().pointSize()) {
        setFontSize(Settings::hugeFont().pointSize());
    } else if (fontPointSize() == Settings::hugeFont().pointSize()) {
        // Do not increase font size if at largest size
    } else {
        // Allow changing the size of non-standard text pasted in from other applications
        setFontSize(Settings::normalFont().pointSize());
    }
}

void NoteEditor::decreaseFontSize()
{
    if (fontPointSize() == Settings::smallFont().pointSize()) {
        // Do not decrease font size if at smallest size
    } else if (fontPointSize() == Settings::normalFont().pointSize()) {
        setFontSize(Settings::smallFont().pointSize());
    } else if (fontPointSize() == Settings::largeFont().pointSize()) {
        setFontSize(Settings::normalFont().pointSize());
    } else if (fontPointSize() == Settings::hugeFont().pointSize()) {
        setFontSize(Settings::largeFont().pointSize());
    } else {
        // Allow changing the size of non-standard text pasted in from other applications
        setFontSize(Settings::normalFont().pointSize());
    }
}

void NoteEditor::resetFontSize()
{
    QTextCharFormat fontSizeFormat;
    fontSizeFormat.setFontPointSize(Settings::normalFont().pointSize());
    mergeCharFormatToSelectionOrWord(fontSizeFormat);
}

void NoteEditor::indentListLess()
{
    QTextCursor cursor = textCursor();
    QTextCursor workingCursor(cursor);
    workingCursor.setPosition(cursor.selectionStart());
    workingCursor.beginEditBlock();

    QTextCursor endCursor(cursor);
    endCursor.setPosition(cursor.selectionEnd());

    bool moveSucceeded;
    do {
        QTextList *textList = workingCursor.currentList();
        if (textList) {
            QTextListFormat listFormat = textList->format();
            if (listFormat.indent() == 1) {
                if (!cursor.hasSelection()) {
                    removeFromList(workingCursor);
                }
            } else {
                listFormat.setIndent(listFormat.indent() - 1);
                QTextListFormat::Style listStyle;
                switch (listFormat.indent() % 3) {
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
                listFormat.setStyle(listStyle);
                workingCursor.createList(listFormat);
            }
        }
        moveSucceeded = workingCursor.movePosition(QTextCursor::NextBlock);
    } while (workingCursor.position() <= endCursor.block().position() && moveSucceeded);
    workingCursor.endEditBlock();
}

void NoteEditor::indentListMore()
{
    QTextCursor cursor = textCursor();
    QTextCursor workingCursor(cursor);
    workingCursor.setPosition(cursor.selectionStart());
    workingCursor.beginEditBlock();

    QTextCursor endCursor(cursor);
    endCursor.setPosition(cursor.selectionEnd());

    bool moveSucceeded;
    do {
        QTextList *textList = workingCursor.currentList();
        QTextListFormat listFormat;
        if (textList) {
            listFormat = textList->format();
            listFormat.setIndent(listFormat.indent() + 1);
        } else {
            listFormat.setIndent(1);
        }

        QTextListFormat::Style listStyle;
        switch (listFormat.indent() % 3) {
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
        listFormat.setStyle(listStyle);
        workingCursor.createList(listFormat);
        moveSucceeded = workingCursor.movePosition(QTextCursor::NextBlock);
    } while (workingCursor.position() <= endCursor.block().position() && moveSucceeded);
    workingCursor.endEditBlock();
}

void NoteEditor::removeFromList(QTextCursor cursor)
{
    if (cursor.isNull()) {
        cursor = textCursor();
    }
    QTextList *textList = cursor.currentList();
    cursor.beginEditBlock();
    QTextBlockFormat removeIndentFormat;
    removeIndentFormat.setIndent(0);
    textList->remove(cursor.block());
    cursor.mergeBlockFormat(removeIndentFormat);
    cursor.endEditBlock();
}

void NoteEditor::setTabStopWidth()
{
    QFontMetrics metrics(currentFont());
    int pixels = metrics.size(0, "W").width();
    KTextEdit::setTabStopWidth(pixels * tabStopCount);
}

void NoteEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    QString linkUrl = anchorAt(event->pos());

    if (!linkUrl.isEmpty()) {
        QTextCursor cursor = textCursor();
        selectLinkText(&cursor);
        setTextCursor(cursor);
        emit linkTriggered(linkUrl, currentLinkText());
    } else {
        QTextEdit::mouseDoubleClickEvent(event);
    }
}

void NoteEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QTextCursor originalCursor = textCursor();
    QTextCursor cursorAtMouse = cursorForPosition(event->pos());

    if (originalCursor.hasSelection()) {
        if (originalCursor.selectionStart() > cursorAtMouse.position()
            && cursorAtMouse.position() > originalCursor.selectionEnd()) {
            setTextCursor(cursorAtMouse);
        }
    } else {
        setTextCursor(cursorAtMouse);
    }

    if (cursorAtMouse.charFormat().isAnchor()) {
        QMenu *popup = mousePopupMenu();
        QAction *firstAction = popup->actions().first();
        popup->insertAction(firstAction, m_removeLinkAction);
        popup->insertSeparator(firstAction);
        popup->exec(event->globalPos());
    } else {
        KTextEdit::contextMenuEvent(event);
    }
    setTextCursor(originalCursor);
}

void NoteEditor::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Tab:
            if (textCursor().currentList()) {
                indentListMore();
                return;
            }
            break;
        case Qt::Key_Backtab:
            indentListLess();
            return;
        case Qt::Key_Backspace:
            if (!textCursor().hasSelection() && textCursor().currentList()
            && textCursor().atBlockStart()) {
                removeFromList();
                return;
            }
    }
    KTextEdit::keyPressEvent(event);
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        if (textCursor().atBlockEnd()) {
            resetTextFormat();
        }
        if (textCursor().atBlockStart()) {
            QTextCursor previousBlockCursor = textCursor();
            previousBlockCursor.movePosition(QTextCursor::PreviousBlock);
            previousBlockCursor.setBlockCharFormat(textCursor().charFormat());
            previousBlockCursor.mergeBlockCharFormat(createLinkClearFormat());
        }
    }
}

void NoteEditor::setupActions()
{
    m_removeLinkAction = new KAction(i18nc("@action:inmenu", "Remove Link"), this);
    connect(m_removeLinkAction, SIGNAL(triggered()), SLOT(removeCurrentLink()));
}

void NoteEditor::selectLinkText(QTextCursor* cursor) const
{
    QString aHref = cursor->charFormat().anchorHref();

    // Move cursor to start of link
    while (cursor->charFormat().anchorHref() == aHref) {
        if (cursor->atStart()) {
            break;
        }
        cursor->setPosition(cursor->position() - 1);
    }
    if (cursor->charFormat().anchorHref() != aHref) {
        cursor->setPosition(cursor->position() + 1, QTextCursor::KeepAnchor);
    }

    // Move selection to the end of the link
    while (cursor->charFormat().anchorHref() == aHref) {
        if (cursor->atEnd()) {
            break;
        }
        cursor->setPosition(cursor->position() + 1, QTextCursor::KeepAnchor);
    }

    if (cursor->charFormat().anchorHref() != aHref) {
        cursor->setPosition(cursor->position() - 1, QTextCursor::KeepAnchor);
    }
}

void NoteEditor::mergeCharFormatToSelectionOrWord(const QTextCharFormat& charFormat)
{
    QTextCursor cursor = textCursor();
    QTextCursor wordStart(cursor);
    QTextCursor wordEnd(cursor);

    wordStart.movePosition(QTextCursor::StartOfWord);
    wordEnd.movePosition(QTextCursor::EndOfWord);

    if (cursor.position() == wordStart.position() || cursor.position() == wordEnd.position()) {
        cursor.mergeCharFormat(charFormat);
        setTextCursor(cursor);
    } else {
        if (!cursor.hasSelection()) {
            cursor.select(QTextCursor::WordUnderCursor);
        }
        cursor.mergeCharFormat(charFormat);
    }
}

QTextCharFormat NoteEditor::createLinkClearFormat()
{
    QTextCharFormat clearLinkFormat;
    clearLinkFormat.setAnchor(false);
    clearLinkFormat.setAnchorHref(QString());
    clearLinkFormat.setUnderlineStyle(QTextCharFormat::NoUnderline);
    clearLinkFormat.setUnderlineColor(KColorScheme(QPalette::Active, KColorScheme::View).foreground().color());
    clearLinkFormat.setForeground(KColorScheme(QPalette::Active, KColorScheme::View).foreground().color());
    return clearLinkFormat;
}
