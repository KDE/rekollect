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
#include "notebrowserwindow.h"
#include "notecollection.h"
#include "windowcollection.h"
#include "systemtrayicon.h"
#include "notefactory.h"
#include "noteeditor.h"
#include "note.h"
#include "linkeditdialog.h"
#include "settings.h"
#include "ui_settings_general.h"
#include "ui_settings_display.h"

#include <QtGui/QTableView>
#include <QtGui/QHeaderView>
#include <QtGui/QCloseEvent>
#include <QtGui/QSortFilterProxyModel>
#include <QtCore/QFileInfo>
#include <QtCore/QPointer>

#include <KIcon>
#include <KApplication>
#include <KToolBar>
#include <KActionCollection>
#include <KAction>
#include <KStandardAction>
#include <KLocalizedString>
#include <KStandardDirs>
#include <KMessageBox>
#include <KRun>
#include <KGlobalSettings>
#include <KFileDialog>
#include <KWindowSystem>
#include <KDebug>
#include <kio/jobclasses.h>
#include <kio/scheduler.h>
#include <KSaveFile>
#include <KConfigDialog>
#include <KDirWatch>

NoteBrowserWindow::NoteBrowserWindow(QWidget *parent) : KXmlGuiWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
    setWindowIcon(KIcon("rekollect"));
    m_noteCollection = new NoteCollection(this);
    m_windowCollection = new WindowCollection(this);
    m_dirWatch = new KDirWatch(this);
    setupWindow();
    setupActions();
    loadNoteDetails(m_noteCollection);
    startDirWatch();
}

void NoteBrowserWindow::toggleBrowserWindow()
{
    if (isVisible()) {
        close();
    } else {
        KWindowSystem::unminimizeWindow(winId());
        show();
        KWindowSystem::raiseWindow(winId());
        KWindowSystem::forceActiveWindow(winId());
    }
}

void NoteBrowserWindow::closeEvent(QCloseEvent *event)
{

    // Do not destroy window on close
    hide();
    event->ignore();
}

void NoteBrowserWindow::keyPressEvent(QKeyEvent *event)
{
    QItemSelectionModel *selectionModel = m_noteTableView->selectionModel();
    QModelIndex currentIndex = selectionModel->currentIndex();
    bool trigger = currentIndex.isValid()
        && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter);

    if (trigger) {
        emit itemActivated(currentIndex);
    }

    QWidget::keyPressEvent(event);
}

void NoteBrowserWindow::setupWindow()
{
    m_sortableProxyModel = new QSortFilterProxyModel(this);
    m_sortableProxyModel->setSourceModel(m_noteCollection);
    m_sortableProxyModel->setDynamicSortFilter(true);
    m_sortableProxyModel->setSortRole(Qt::EditRole);
    m_noteTableView = new QTableView;
    m_noteTableView->setModel(m_sortableProxyModel);
    m_noteTableView->resizeColumnsToContents();
    m_noteTableView->horizontalHeader()->stretchLastSection();
    m_noteTableView->setEditTriggers(QTableView::NoEditTriggers);
    m_noteTableView->setSelectionMode(QTableView::ExtendedSelection);
    m_noteTableView->setSelectionBehavior(QTableView::SelectRows);
    m_noteTableView->setSortingEnabled(true);
    m_noteTableView->sortByColumn(NoteCollection::ModificationTimeColumn, Qt::DescendingOrder);
    connectClickSignals();
    connect(this, SIGNAL(itemActivated(QModelIndex)), SLOT(openNote(QModelIndex)));
    connect(KGlobalSettings::self(), SIGNAL(settingsChanged(int)), SLOT(globalSettingsChanged(int)));
    setCentralWidget(m_noteTableView);
    setupTray();
}

void NoteBrowserWindow::setupActions()
{

    m_newNoteAction = new KAction(i18nc("@action:inmenu", "Create Note"), this);
    m_newNoteAction->setIcon(KIcon("document-new"));
    m_newNoteAction->setObjectName("new");
    m_newNoteAction->setGlobalShortcut(KShortcut(Qt::ALT + Qt::Key_F9));
    connect(m_newNoteAction, SIGNAL(triggered()), SLOT(createNewNote()));
    actionCollection()->addAction("new", m_newNoteAction);

    KStandardAction::preferences(this, SLOT(showPreferences()), actionCollection());
    KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

    m_globalShowBrowserAction = new KAction(i18nc("@action", "Display Note Browser"), this);
    m_globalShowBrowserAction->setObjectName("globalShowBrowser");
    connect(m_globalShowBrowserAction, SIGNAL(triggered()), SLOT(toggleBrowserWindow()));
    m_globalShowBrowserAction->setGlobalShortcut(KShortcut(Qt::ALT + Qt::Key_F10));
    actionCollection()->addAction("globalShowBrowser", m_globalShowBrowserAction);

    toolBar()->setVisible(false);
    setupGUI();
}

void NoteBrowserWindow::setupTray()
{

    m_systemTrayIcon = new SystemTrayIcon(this);
    m_systemTrayIcon->show();
    for (int i = 0; i < m_noteCollection->rowCount(); ++i)
    {
        QString fileName = m_noteCollection->item(i, NoteCollection::FileNameColumn)->text();
        QString documentName = m_noteCollection->item(i, NoteCollection::DocumentNameColumn)->text();
        m_systemTrayIcon->addDocumentAction(fileName, documentName);
    }
    connect(m_noteCollection, SIGNAL(noteAdded(QString,QString)), m_systemTrayIcon, SLOT(addDocumentAction(QString,QString)));
    connect(m_noteCollection, SIGNAL(noteRemoved(QString)), m_systemTrayIcon, SLOT(removeDocumentAction(QString)));
    connect(m_systemTrayIcon, SIGNAL(openNoteRequested(QString)), SLOT(openNote(QString)));
    connect(m_systemTrayIcon, SIGNAL(createNoteRequested()), SLOT(createNewNote()));
}

void NoteBrowserWindow::openNote(const QModelIndex &index)
{
    const Qt::KeyboardModifiers modifier = QApplication::keyboardModifiers();
    if ((modifier & Qt::ShiftModifier) || (modifier & Qt::ControlModifier)) {
        return;
    }

    QModelIndex realIndex = m_sortableProxyModel->mapToSource(index);
    QString fileName = m_noteCollection->fileNameAt(realIndex);
    openNote(fileName);
}

void NoteBrowserWindow::openNote(const QString &fileName)
{
    if (m_windowCollection->windowExists(fileName)) {
        m_windowCollection->showWindow(fileName);
    } else {
        Note *note = NoteFactory::openExistingNote(fileName);
        NoteWindow *window = new NoteWindow(note);
        m_windowCollection->addWindow(note->fileName(), window);
        connectWindowSignals(window);
        window->show();
    }
}

QString NoteBrowserWindow::createNewNote(const QString &newNoteName)
{
    Note *note = NoteFactory::createNewNote(newNoteName);
    m_noteCollection->addNote(note);
    NoteWindow *window = new NoteWindow(note);
    m_windowCollection->addWindow(note->fileName(), window);
    connectWindowSignals(window);
    window->saveNote();
    window->show();
    return note->fileName();
}

void NoteBrowserWindow::linkTriggered(const QString &linkUrl, const QString &linkName)
{
    if (linkUrl.startsWith(QLatin1String("rekollect:"))) {
        QString fileName = linkUrl.section(':', 1);
        QString fullFilePath = KStandardDirs::locateLocal("app_notes", fileName);
        if (m_noteCollection->noteExists(fullFilePath)) {
            openNote(fullFilePath);
        } else {
            NoteEditor *editor = qobject_cast<NoteEditor *>(sender());
            int result = KMessageBox::questionYesNoCancel(
                        editor, i18nc("@info Prompt to handle a missing note",
                                      "<filename>%1</filename> does not exist. Do you "
                                      "want to remove or edit the link?", linkName),
                        i18nc("@title:window", "Question"),
                        KGuiItem(i18nc("@action:button", "Remove Link")),
                        KGuiItem(i18nc("@action:button", "Edit Link"))
            );

            if (result == KMessageBox::Yes) {
                // Remove Link button is selected
                editor->removeCurrentLink();
            } else if (result == KMessageBox::No) {
                // Create New Note is selected
                editLink(editor);
            }
        }
    } else {
        KRun *run = new KRun(linkUrl, qobject_cast<QWidget *>(parent()));
        Q_UNUSED(run)
    }
}

void NoteBrowserWindow::editLink(NoteEditor *editor)
{
    QPointer<LinkEditDialog> dialog = new LinkEditDialog(m_noteCollection,
                                                        editor->currentLinkText().trimmed(),
                                                        editor->document()->fileName(), editor);
    if (dialog->exec()) {
        QString fileName;
        switch (dialog->linkType()) {
        case LinkEditDialog::NewNote:
            fileName = createNewNote(editor->currentLinkText().trimmed());
            break;
        case LinkEditDialog::ExistingNote:
            fileName = dialog->linkFileName();
            break;
        }
        QFileInfo fileInfo(fileName);
        QString linkUrl = QString("rekollect:%1").arg(fileInfo.fileName());
        editor->updateLink(linkUrl);
    }
    delete dialog;
}

void NoteBrowserWindow::showPreferences()
{
    if (KConfigDialog::showDialog("settings")) {
        return;
    }

    KConfigDialog *dialog = new KConfigDialog(this, "settings", Settings::self());
    QWidget *generalSettingsDlg = new QWidget;
    ui_generalSettings = new Ui::GeneralSettingsWidget;
    ui_generalSettings->setupUi(generalSettingsDlg);
    dialog->addPage(generalSettingsDlg, i18nc("@title:group", "General"), "configure");
    QWidget *displaySettingsDlg = new QWidget;
    ui_displaySettings = new Ui::DisplaySettingsWidget;
    ui_displaySettings->setupUi(displaySettingsDlg);
    dialog->addPage(displaySettingsDlg, i18nc("@title:group", "Display"), "configure");
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(dialog, SIGNAL(settingsChanged(QString)), m_windowCollection, SLOT(updateOpenWindows()));
    dialog->show();
}

void NoteBrowserWindow::globalSettingsChanged(int category)
{
    Q_UNUSED(category);
    disconnect(m_noteTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(openNote(QModelIndex)));
    disconnect(m_noteTableView, SIGNAL(clicked(QModelIndex)), this, SLOT(openNote(QModelIndex)));
    connectClickSignals();
}

void NoteBrowserWindow::reloadNoteDetails()
{
    m_noteCollection->clear();
    loadNoteDetails(m_noteCollection);
    m_windowCollection->updateOpenWindows();
}

void NoteBrowserWindow::loadNoteDetails(NoteCollection *collection)
{
    QStringList noteList = KGlobal::dirs()->findAllResources("app_notes", QString());
    foreach (const QString &notePath, noteList) {
        Note *note = NoteFactory::openExistingNote(notePath);
        if (note) {
            collection->addNote(note);
        }
    }
}

void NoteBrowserWindow::startDirWatch()
{
    QStringList noteDirs = KGlobal::dirs()->findDirs("app_notes", QString());
    foreach (const QString &noteDir, noteDirs) {
        m_dirWatch->addDir(noteDir);
    }
    connect(m_dirWatch, SIGNAL(dirty(QString)), this, SLOT(reloadNoteDetails()));
}

void NoteBrowserWindow::connectWindowSignals(NoteWindow *window)
{
    connect(window->editor()->document(), SIGNAL(documentNameChanged(QString,QString)), m_noteCollection, SLOT(documentNameChanged(QString,QString)));
    connect(window->editor()->document(), SIGNAL(documentNameChanged(QString,QString)), m_systemTrayIcon, SLOT(renameDocumentAction(QString,QString)));
    connect(window, SIGNAL(noteDeleted(QString)), m_noteCollection, SLOT(removeNote(QString)));
    connect(window, SIGNAL(documentModified(QString,QDateTime)), m_noteCollection, SLOT(documentModified(QString,QDateTime)));
    connect(window, SIGNAL(newNoteRequested()), SLOT(createNewNote()));
    connect(window->editor(), SIGNAL(linkTriggered(QString,QString)), SLOT(linkTriggered(QString,QString)));
    connect(window, SIGNAL(editLinkRequested(NoteEditor *)), SLOT(editLink(NoteEditor *)));
}

void NoteBrowserWindow::connectClickSignals()
{
    if (KGlobalSettings::singleClick()) {
        connect(m_noteTableView, SIGNAL(clicked(QModelIndex)), SLOT(openNote(QModelIndex)));
    } else {
        connect(m_noteTableView, SIGNAL(doubleClicked(QModelIndex)), SLOT(openNote(QModelIndex)));
    }
}
