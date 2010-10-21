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
#ifndef NOTEBROWSERWINDOW_H
#define NOTEBROWSERWINDOW_H

#include <KXmlGuiWindow>

class KUrl;
class NoteEditor;
class Note;
class NoteCollection;
class NoteWindow;
class WindowCollection;
class SystemTrayIcon;

namespace Ui {
    class GeneralSettingsWidget;
}

class QSortFilterProxyModel;
class QTableView;
class QModelIndex;
class QCloseEvent;

class KAction;
class KJob;

class NoteBrowserWindow : public KXmlGuiWindow
{
    Q_OBJECT
public:
    NoteBrowserWindow(QWidget *parent = 0);

public slots:
    void toggleBrowserWindow();

signals:
    void itemActivated(const QModelIndex &index);

protected:
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent* event);

private:
    void setupWindow();
    void setupActions();
    void setupTray();

private slots:
    void openNote(const QModelIndex &index);
    void openNote(const QString &fileName);
    QString createNewNote(const QString &newNoteName = QString());
    void linkTriggered(const QString &linkUrl, const QString &linkName);
    void editLink(NoteEditor *editor);
    void showPreferences();
    void globalSettingsChanged(int category);

private:
    void loadNoteDetails(NoteCollection *collection);
    void connectWindowSignals(NoteWindow *window);
    void connectClickSignals();

    QTableView *m_noteTableView;
    QSortFilterProxyModel *m_sortableProxyModel;
    NoteCollection *m_noteCollection;
    WindowCollection *m_windowCollection;

    KAction *m_newNoteAction;
    KAction *m_globalShowBrowserAction;

    SystemTrayIcon *m_systemTrayIcon;

    Ui::GeneralSettingsWidget *ui_generalSettings;

};

#endif // NOTEBROWSERWINDOW_H
