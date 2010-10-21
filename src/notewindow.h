/*
    Rekollect: A note taking application
    Copyright (C) 2009, 2010  Jason Jackson <jacksonje@gmail.com>

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
#ifndef NOTEWINDOW_H
#define NOTEWINDOW_H

#include <KMainWindow>

class NoteEditor;
class Note;

class QSignalMapper;
class QActionGroup;
class QTextCharFormat;
class QDateTime;

class KAction;
class KActionMenu;
class KJob;

class NoteWindow : public KMainWindow
{
    Q_OBJECT
public:
    explicit NoteWindow(Note *note, QWidget *parent = 0);
    ~NoteWindow();

    NoteEditor *editor();

public slots:
    void deleteNote();
    void saveNote();

signals:
    void closeRequested(const QString &fileName);
    void noteDeleted(const QString &fileName);
    void newNoteRequested();
    void documentModified(const QString &fileName, const QDateTime &modificationTime);
    void editLinkRequested(NoteEditor *editor);

private:
    void createActions();
    void createToolbar();
    void storeWindowSettings();
    void restoreWindowSettings();

private slots:
    void documentNameChanged(const QString &fileName, const QString &documentName);
    void linkToUrl();
    void editLink();
    void updateFormatMenu(const QTextCharFormat &charFormat);
    void exportAsHtml();
    void slotPutResult(KJob *job);

    void printRequested();
    void printPreviewRequested();


private:
    NoteEditor *m_noteEditor;

    KActionMenu *m_formatActionMenu;
    KActionMenu *m_searchActionMenu;
    KActionMenu *m_linkActionMenu;
    KActionMenu *m_actionActionMenu;
    KAction *m_linkUrlAction;
    KAction *m_removeLinkAction;
    KAction *m_resetFormatAction;
    KAction *m_boldAction;
    KAction *m_italicAction;
    KAction *m_strikeOutAction;
    KAction *m_highlightAction;
    KAction *m_smallTextAction;
    KAction *m_normalTextAction;
    KAction *m_largeTextAction;
    KAction *m_hugeTextAction;
    QActionGroup *m_fontSizeGroup;
    QSignalMapper *m_fontSizeSignalMapper;
    KAction *m_increaseFontSizeAction;
    KAction *m_decreaseFontSizeAction;
    KAction *m_increaseIndentAction;
    KAction *m_decreaseIndentAction;
    KAction *m_undoAction;
    KAction *m_redoAction;
    KAction *m_deleteNoteAction;
    KAction *m_exportAsHtmlAction;


};

#endif // NOTEWINDOW_H
