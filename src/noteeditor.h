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

#ifndef NOTEEDITOR_H
#define NOTEEDITOR_H

#include <KTextEdit>

class Note;

class QMouseEvent;

class KAction;

class NoteEditor : public KTextEdit
{
    Q_OBJECT

public:
    explicit NoteEditor(Note *note = 0, QWidget *parent = 0);

    static const int tabStopCount = 4;

    void setDocument(Note *note);
    Note *document() const;

    QString currentLinkText() const;

    void setLineMargin(qreal margin);
    qreal lineMargin();

public slots:
    void updateLink(const QString &linkUrl);
    void removeCurrentLink();

    void setTextHighlight(bool highlight);
    void setTextBold(bool bold);
    void setTextItalic(bool italic);
    void setTextStrikeOut(bool strikeOut);

    void clearLinkFormat();
    void resetTextFormat();

    void setFontSize(int fontSize);
    void increaseFontSize();
    void decreaseFontSize();
    void resetFontSize();

    void indentListLess();
    void indentListMore();
    void removeFromList(QTextCursor cursor = QTextCursor());

    void setTabStopWidth();

protected:
    void mouseDoubleClickEvent(QMouseEvent* event);
    void contextMenuEvent(QContextMenuEvent *event);
    void keyPressEvent(QKeyEvent *event);

signals:
    void linkTriggered(const QString &linkUrl, const QString &linkText);

private:
    void setupActions();

    void selectLinkText(QTextCursor *cursor) const;

    void mergeCharFormatToSelectionOrWord(const QTextCharFormat &charFormat);
    QTextCharFormat createLinkClearFormat();

private:
    Note *m_document;
    KAction *m_removeLinkAction;
    qreal m_lineMargin;

};

#endif // NOTEEDITOR_H
