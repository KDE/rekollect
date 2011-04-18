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

import Qt 4.7
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: page;
    width: 200
    height: 200
    anchors.fill: parent

    PlasmaCore.DataSource {
        id: dataSource
        engine: "rekollectnotes"
        interval: 0
        connectedSources: sources

        onSourceAdded: {
            connectSource(source)
        }
    }

    PlasmaCore.Theme {
        id: theme
    }

    PlasmaWidgets.Label {
        id:helloLabel
        text: i18nc("@title:window", "Notes")
        font.bold: true
        font.underline: true
    }

    ListView {
        id: noteList
        anchors.top: helloLabel.bottom
        anchors.bottom: newNote.top
        anchors.bottomMargin: 5
        clip: true
        width: parent.width

        Component.onCompleted: {
            print("Test");
            for (mykey in dataSource.data["/home/h1jej01/.kde/share/apps/rekollect/notesdev/newval"]) {
                print("Key: " + mykey);
            }
        }

        PlasmaCore.DataModel {
            id: model
            dataSource: dataSource
        }

        PlasmaCore.SortFilterModel {
            id: sortModel
            sourceModel: model
            sortRole: "modificationTime"
            sortOrder: Qt.DescendingOrder
        }

        model: sortModel

        delegate: TextCommand {
            id: noteItem
            label: documentName
            commandArg: fileName
        }

    }

    TextCommand {
        id: newNote
        anchors.bottom: browseNotes.top
        label: i18nc("@action:button Open a newly created note", "Create new note")
        commandArg: "--new"
    }

    TextCommand {
        id: browseNotes
        anchors.bottom: parent.bottom
        label: i18nc("@action:button Open the note browser", "Browse Notes")
        commandArg: "--browse"
    }
}