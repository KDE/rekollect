/*
    Rekollect: A note taking application
    Copyright (C) 2009, 2010, 2011  Jason Jackson <jacksonje@gmail.com>

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
#include "notewindow.h"
#include "noteeditor.h"
#include "note.h"
#include "io/native/notewriter.h"
#include "io/textile/textilewriter.h"

#include <QtGui/QActionGroup>
#include <QtCore/QSignalMapper>
#include <QtCore/QFileInfo>
#include <QtGui/QPrinter>
#include <QtGui/QPrintDialog>

#include <KActionMenu>
#include <KToolBar>
#include <KIcon>
#include <KAction>
#include <KStandardAction>
#include <KLocalizedString>
#include <KMessageBox>
#include <KInputDialog>
#include <KIO/NetAccess>
#include <KSaveFile>
#include <KConfigGroup>
#include <KSharedConfig>
#include <KFileDialog>
#include <KPrintPreview>
#include <kdeprintdialog.h>
#include <kio/jobclasses.h>
#include <kio/scheduler.h>

NoteWindow::NoteWindow(Note *note, QWidget *parent)
    : KMainWindow(parent)
{
    setCaption(note->documentName());
    m_noteEditor = new NoteEditor(note, this);
    connect(m_noteEditor, SIGNAL(textChanged()), SLOT(saveNote()));
    note->setIndentWidth(24);
    connect(note, SIGNAL(documentNameChanged(QString,QString)), SLOT(documentNameChanged(QString,QString)));
    setCentralWidget(m_noteEditor);
    setWindowIcon(KIcon("rekollect"));
    createActions();
    createToolbar();
    restoreWindowSettings();
}

NoteWindow::~NoteWindow()
{
    storeWindowSettings();
    emit closeRequested(m_noteEditor->document()->fileName());
}

NoteEditor* NoteWindow::editor()
{
    return m_noteEditor;
}

void NoteWindow::deleteNote()
{
    if (KMessageBox::questionYesNo(this, i18nc("@info", "Are you sure you want to delete this note?"),
                                   i18nc("@title:window", "Delete Note")) == KMessageBox::Yes) {
        QString noteFileName = m_noteEditor->document()->fileName();
        if (KIO::NetAccess::del(noteFileName, 0)) {
            emit noteDeleted(noteFileName);
            close();

        } else {
            KMessageBox::error(this,
                               i18nc("@info Error message",
                                     "An error occurred deleting this note. The note is not deleted."),
                               i18nc("@title:window", "Error"));
        }
    }
}

void NoteWindow::saveNote()
{
    Note *note = editor()->document();
    KSaveFile saveFile(note->fileName());
    saveFile.open();

    NoteWriter writer(note->rootFrame(), note->tags());
    if (!writer.writeFile(&saveFile)) {
        KMessageBox::error(0,
                           i18nc("@info Error message", "An error occurred saving the file."),
                           i18nc("@title:window", "Save Error"));
    }

    bool isSuccessful = saveFile.finalize();
    saveFile.close();

    if (isSuccessful) {
        emit documentModified(note->fileName(), note->modificationTime().dateTime());
    } else {
        KMessageBox::error(0,
                           i18nc("@info Error message", "An error occurred saving the file."),
                           i18nc("@title:window", "Save Error"));
    }
}

void NoteWindow::createActions()
{
    m_undoAction = KStandardAction::undo(m_noteEditor, SLOT(undo()), this);
    m_undoAction->setEnabled(m_noteEditor->document()->isUndoAvailable());
    connect(m_noteEditor, SIGNAL(undoAvailable(bool)), m_undoAction, SLOT(setEnabled(bool)));

    m_redoAction = KStandardAction::redo(m_noteEditor, SLOT(redo()), this);
    m_redoAction->setEnabled(m_noteEditor->document()->isRedoAvailable());
    connect(m_noteEditor, SIGNAL(redoAvailable(bool)), m_redoAction, SLOT(setEnabled(bool)));

    m_linkUrlAction = new KAction(KIcon("insert-link"), i18nc("@action:intoolbar", "Link to URL"),
                                  this);
    connect(m_linkUrlAction, SIGNAL(triggered()), SLOT(linkToUrl()));

    m_removeLinkAction = new KAction(i18nc("@action:intoolbar", "Remove Link"), this);
    connect(m_removeLinkAction, SIGNAL(triggered()), m_noteEditor, SLOT(removeCurrentLink()));

    m_boldAction = new KAction(KIcon("format-text-bold"),
                               i18nc("@option:check Text formatting option", "Bold"), this);
    m_boldAction->setCheckable(true);
    m_boldAction->setShortcut(Qt::CTRL + Qt::Key_B);
    QFont boldFont;
    boldFont.setBold(true);
    m_boldAction->setFont(boldFont);
    connect(m_boldAction, SIGNAL(triggered(bool)), m_noteEditor, SLOT(setTextBold(bool)));

    m_italicAction = new KAction(KIcon("format-text-italic"),
                                 i18nc("@option:check Text formatting option", "Italic"), this);
    m_italicAction->setCheckable(true);
    m_italicAction->setShortcut(Qt::CTRL + Qt::Key_I);
    QFont italicFont;
    italicFont.setItalic(true);
    m_italicAction->setFont(italicFont);
    connect(m_italicAction, SIGNAL(triggered(bool)), m_noteEditor, SLOT(setTextItalic(bool)));

    m_strikeOutAction = new KAction(KIcon("format-text-strikethrough"),
                                    i18nc("@option:check Text formatting option", "Strike Out"), this);
    m_strikeOutAction->setCheckable(true);
    m_strikeOutAction->setShortcut(Qt::CTRL + Qt::Key_L);
    QFont strikeOutFont;
    strikeOutFont.setStrikeOut(true);
    m_strikeOutAction->setFont(strikeOutFont);
    connect(m_strikeOutAction, SIGNAL(triggered(bool)), m_noteEditor, SLOT(setTextStrikeOut(bool)));

    m_highlightAction = new KAction(i18nc("@option:check Text formatting option", "Highlight"), this);
    m_highlightAction->setCheckable(true);
    m_highlightAction->setShortcut(Qt::CTRL + Qt::Key_H);
    connect(m_highlightAction , SIGNAL(triggered(bool)), m_noteEditor, SLOT(setTextHighlight(bool)));

    m_resetFormatAction = new KAction(i18nc("@action:inmenu Reset all text formatting to normal values",
                                           "Reset Format"), this);
    connect(m_resetFormatAction, SIGNAL(triggered()), m_noteEditor, SLOT(resetTextFormat()));

    m_smallTextAction = new KAction(i18nc("@option:radio Font size", "S&mall"), this);
    m_smallTextAction->setCheckable(true);
    QFont smallTextFont;
    smallTextFont.setPointSize(Note::SmallFont);
    m_smallTextAction->setFont(smallTextFont);

    m_normalTextAction = new KAction(i18nc("@option:radio Font size", "&Normal"), this);
    m_normalTextAction->setCheckable(true);
    QFont normalTextFont;
    normalTextFont.setPointSize(Note::NormalFont);
    m_normalTextAction->setFont(normalTextFont);

    m_largeTextAction = new KAction(i18nc("@option:radio Font size", "&Large"), this);
    m_largeTextAction->setCheckable(true);
    QFont largeTextFont;
    largeTextFont.setPointSize(Note::LargeFont);
    m_largeTextAction->setFont(largeTextFont);

    m_hugeTextAction = new KAction(i18nc("@option:radio Font size", "&Huge"), this);
    m_hugeTextAction->setCheckable(true);
    QFont hugeTextFont;
    hugeTextFont.setPointSize(Note::HugeFont);
    m_hugeTextAction->setFont(hugeTextFont);

    m_fontSizeSignalMapper = new QSignalMapper(this);
    connect(m_smallTextAction, SIGNAL(triggered()), m_fontSizeSignalMapper, SLOT(map()));
    m_fontSizeSignalMapper->setMapping(m_smallTextAction, Note::SmallFont);
    connect(m_normalTextAction, SIGNAL(triggered()), m_fontSizeSignalMapper, SLOT(map()));
    m_fontSizeSignalMapper->setMapping(m_normalTextAction, Note::NormalFont);
    connect(m_largeTextAction, SIGNAL(triggered()), m_fontSizeSignalMapper, SLOT(map()));
    m_fontSizeSignalMapper->setMapping(m_largeTextAction, Note::LargeFont);
    connect(m_hugeTextAction, SIGNAL(triggered()), m_fontSizeSignalMapper, SLOT(map()));
    m_fontSizeSignalMapper->setMapping(m_hugeTextAction, Note::HugeFont);
    connect(m_fontSizeSignalMapper, SIGNAL(mapped(int)), m_noteEditor, SLOT(setFontSize(int)));

    m_fontSizeGroup = new QActionGroup(this);
    m_fontSizeGroup->addAction(m_smallTextAction);
    m_fontSizeGroup->addAction(m_normalTextAction);
    m_fontSizeGroup->addAction(m_largeTextAction);
    m_fontSizeGroup->addAction(m_hugeTextAction);
    m_normalTextAction->setChecked(true);

    m_increaseFontSizeAction = new KAction(KIcon("format-font-size-more"),
                                           i18nc("@action:intoolbar Font size", "Increase Font Size"),
                                           this);
    m_increaseFontSizeAction->setShortcut(Qt::CTRL + Qt::Key_Plus);
    connect(m_increaseFontSizeAction , SIGNAL(triggered()), m_noteEditor, SLOT(increaseFontSize()));

    m_decreaseFontSizeAction = new KAction(KIcon("format-font-size-less"),
                                           i18nc("@action:intoolbar Font size", "Decrease Font Size"),
                                           this);
    m_decreaseFontSizeAction->setShortcut(Qt::CTRL + Qt::Key_Minus);
    connect(m_decreaseFontSizeAction , SIGNAL(triggered()), m_noteEditor, SLOT(decreaseFontSize()));

    m_increaseIndentAction = new KAction(KIcon("format-indent-more"),
                                         i18nc("@action:intoolbar", "Increase Indent"), this);
    m_increaseIndentAction->setShortcut(Qt::ALT + Qt::Key_Right);
    connect(m_increaseIndentAction, SIGNAL(triggered()), m_noteEditor, SLOT(indentListMore()));

    m_decreaseIndentAction = new KAction(KIcon("format-indent-less"),
                                         i18nc("@action:intoolbar", "Decrease Indent"), this);
    m_decreaseIndentAction->setShortcut(Qt::ALT + Qt::Key_Left);
    connect(m_decreaseIndentAction, SIGNAL(triggered()), m_noteEditor, SLOT(indentListLess()));

    m_deleteNoteAction = new KAction(KIcon("edit-delete"),
                                     i18nc("@action:intoolbar Delete this note", "&Delete"), this);
    connect(m_deleteNoteAction, SIGNAL(triggered()), SLOT(deleteNote()));

    m_exportAsHtmlAction = new KAction(i18nc("@action:inmenu Export note as HTML", "as HTML..."), this);
    connect(m_exportAsHtmlAction, SIGNAL(triggered()), SLOT(exportAsHtml()));

    m_exportAsTextileAction = new KAction(i18nc("@action:inmenu Export note as Textile", "as Textile..."), this);
    connect(m_exportAsTextileAction, SIGNAL(triggered()), SLOT(exportAsTextile()));
}

void NoteWindow::createToolbar()
{
    m_searchActionMenu = new KActionMenu(KIcon("edit-find"),
                                         i18nc("@action:intoolbar Search text in this note", "&Search"),
                                         this);
    m_searchActionMenu->setDelayed(false);
    m_searchActionMenu->addAction(KStandardAction::find(m_noteEditor, SLOT(slotFind()), this));
    m_searchActionMenu->addAction(KStandardAction::findNext(m_noteEditor, SLOT(slotFindNext()), this));
    toolBar()->addAction(m_searchActionMenu);

    m_linkActionMenu = new KActionMenu(KIcon("insert-link"),
                                       i18nc("@action:intoolbar Create or edit a link", "&Link to Note"),
                                       this);
    connect(m_linkActionMenu, SIGNAL(triggered()), SLOT(editLink()));
    m_linkActionMenu->addAction(m_linkUrlAction);
    m_linkActionMenu->addAction(m_removeLinkAction);
    toolBar()->addAction(m_linkActionMenu);

    m_formatActionMenu = new KActionMenu(KIcon("preferences-desktop-font"),
                                         i18nc("@title:menu Text formatting actions", "&Format"),
                                         this);
    m_formatActionMenu->setDelayed(false);
    toolBar()->addAction(m_formatActionMenu);

    m_formatActionMenu->addAction(m_undoAction);
    m_formatActionMenu->addAction(m_redoAction);
    m_formatActionMenu->addSeparator();

    m_formatActionMenu->addAction(m_boldAction);
    m_formatActionMenu->addAction(m_italicAction);
    m_formatActionMenu->addAction(m_strikeOutAction);
    m_formatActionMenu->addAction(m_highlightAction);

    m_formatActionMenu->addSeparator();
    m_formatActionMenu->addAction(m_resetFormatAction);

    m_formatActionMenu->addSeparator()->setText(i18nc("@title:group", "Font Size"));
    m_formatActionMenu->addAction(m_smallTextAction);
    m_formatActionMenu->addAction(m_normalTextAction);
    m_formatActionMenu->addAction(m_largeTextAction);
    m_formatActionMenu->addAction(m_hugeTextAction);

    m_formatActionMenu->addAction(m_increaseFontSizeAction);
    m_formatActionMenu->addAction(m_decreaseFontSizeAction);

    m_formatActionMenu->addSeparator();
    m_formatActionMenu->addAction(m_increaseIndentAction);
    m_formatActionMenu->addAction(m_decreaseIndentAction);

    m_actionActionMenu = new KActionMenu(KIcon("quickopen"),
                                         i18nc("@action:intoolbar Additional ways to operation on the document", "Actions"),
                                         this);
    m_actionActionMenu->setDelayed(false);
    m_actionActionMenu->addAction(KStandardAction::print(this, SLOT(printRequested()), this));
    m_actionActionMenu->addAction(KStandardAction::printPreview(this, SLOT(printPreviewRequested()), this));
    m_actionActionMenu->addSeparator();
    m_actionActionMenu->addAction(KStandardAction::openNew(this, SIGNAL(newNoteRequested()), this));
    m_actionActionMenu->addAction(m_deleteNoteAction);
    m_actionActionMenu->addSeparator()->setText(i18nc("@title:group File export options", "Export as ..."));
    m_actionActionMenu->addAction(m_exportAsHtmlAction);
    m_actionActionMenu->addAction(m_exportAsTextileAction);

    toolBar()->addAction(m_actionActionMenu);

    connect(m_noteEditor, SIGNAL(currentCharFormatChanged(QTextCharFormat)),
            SLOT(updateFormatMenu(QTextCharFormat)));
}

void NoteWindow::storeWindowSettings()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup windowsGroup = config->group("windows");

    QString fileName = editor()->document()->fileName();
    QFileInfo fileInfo(fileName);
    KConfigGroup noteGroup = windowsGroup.group(QString("%1").arg(fileInfo.fileName()));
    saveWindowSize(noteGroup);
    noteGroup.writeEntry("Position", pos());
    config->sync();
}

void NoteWindow::restoreWindowSettings()
{
    KSharedConfigPtr config = KGlobal::config();
    KConfigGroup windowsGroup = config->group("windows");

    QString fileName = editor()->document()->fileName();
    QFileInfo fileInfo(fileName);
    KConfigGroup noteGroup = windowsGroup.group(QString("%1").arg(fileInfo.fileName()));
    restoreWindowSize(noteGroup);
    QPoint position = noteGroup.readEntry("Position", QPoint());
    if (!position.isNull()) {
        move(noteGroup.readEntry("Position", QPoint()));
    }
}

void NoteWindow::documentNameChanged(const QString &fileName, const QString &documentName)
{
    Q_UNUSED(fileName)
    setWindowTitle(i18nc("@title:window", "%1 - Rekollect", documentName));
}

void NoteWindow::linkToUrl()
{
    bool ok;
    QString url = KInputDialog::getText(i18nc("@title:window", "Link to URL"),
                                        i18nc("@label:textbox", "Please enter the URL to Link to:"),
                                        QString(), &ok, this);
    if (ok) {
        m_noteEditor->updateLink(url);
    }
}

void NoteWindow::editLink()
{
    emit editLinkRequested(editor());
}

void NoteWindow::updateFormatMenu(const QTextCharFormat &charFormat)
{
    QTextCharFormat charFormat_ = charFormat;
    QTextCursor cursor = m_noteEditor->textCursor();
    if (cursor.hasSelection() && cursor.position() == cursor.selectionStart()) {
        QTextCursor endCursor(cursor);
        endCursor.setPosition(cursor.selectionEnd());
        charFormat_ = endCursor.charFormat();
    }

    m_boldAction->setChecked(charFormat_.fontWeight() == QFont::Bold);
    m_italicAction->setChecked(charFormat_.fontItalic());
    m_strikeOutAction->setChecked(charFormat_.fontStrikeOut());
    m_highlightAction->setChecked(charFormat_.background() == Qt::yellow);

    switch (static_cast<int>(charFormat.fontPointSize())) {
        case Note::SmallFont:
            m_smallTextAction->setChecked(true);
            break;
        case Note::NormalFont:
            m_normalTextAction->setChecked(true);
            break;
        case Note::LargeFont:
            m_largeTextAction->setChecked(true);
            break;
        case Note::HugeFont:
            m_hugeTextAction->setChecked(true);
            break;
    }
}

void NoteWindow::exportAsHtml()
{
    KUrl saveFile = KFileDialog::getSaveUrl(
        KUrl("kfiledialog:///"), QString("text/html"), this,
        i18nc("@title", "Select a destination directory")
    );

    QByteArray htmlOutput;
    htmlOutput.append(m_noteEditor->document()->toHtml().toUtf8());

    KIO::StoredTransferJob *job = KIO::storedPut(htmlOutput, saveFile, -1, KIO::Overwrite);
    connect(job, SIGNAL(result(KJob*)), SLOT(slotPutResult(KJob*)));
}

void NoteWindow::exportAsTextile()
{
    Note *note = editor()->document();

    KUrl saveFileUrl = KFileDialog::getSaveUrl(
        KUrl("kfiledialog:///"), QString("text/plain"), this,
        i18nc("@title", "Select a destination directory")
    );

    if (!saveFileUrl.isEmpty()) {
        KSaveFile saveFile(saveFileUrl.path());
        saveFile.open();

        TextileWriter writer(note->rootFrame(), note->tags());
        if (!writer.writeFile(&saveFile)) {
            KMessageBox::error(0,
                            i18nc("@info Error message", "An error occurred saving the file."),
                            i18nc("@title:window", "Save Error"));
        }

        bool isSuccessful = saveFile.finalize();
        saveFile.close();

        if (!isSuccessful) {
            KMessageBox::error(0,
                            i18nc("@info Error message", "An error occurred saving the file."),
                            i18nc("@title:window", "Save Error"));
        }
    }
}

void NoteWindow::slotPutResult(KJob *job)
{
    if (job->error()) {
        KMessageBox::error(
            this,
            i18nc("@info", "An error occurred exporting the selected files.\n%1", job->errorString()),
            i18nc("@title", "Error exporting file.")
        );
    }
}

void NoteWindow::printRequested()
{
    QPrinter printer;
    QPrintDialog *printDialog = KdePrint::createPrintDialog(&printer, this);
    printDialog->setWindowTitle(i18nc("@title:window", "Print Note"));
    if (m_noteEditor->textCursor().hasSelection()) {
        printDialog->setOption(QPrintDialog::PrintSelection, true);
    }

    if (printDialog->exec() == QDialog::Accepted) {
        m_noteEditor->print(&printer);
    }
}

void NoteWindow::printPreviewRequested()
{
    QPrinter printer;
    KPrintPreview preview(&printer);
    m_noteEditor->document()->print(&printer);
    preview.exec();
}
