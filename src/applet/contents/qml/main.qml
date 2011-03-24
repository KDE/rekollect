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
        text: "Hello World!"
    }

    ListView {
        id: noteList
        anchors.top: helloLabel.bottom
        anchors.bottom: parent.bottom
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
            sortRole: "fileName"
            sortOrder: Qt.AscendingOrder
        }
        model: sortModel

        delegate: Item {
            id: noteItem
            height: 20
            width: parent.width

            MouseArea {
                anchors.fill: parent
                onClicked: { print(fileName) }
            }

            Text {
                id: documentNameText
                text: fileName
                anchors.fill: parent
                color: "blue"
                font.underline: true
            }
        }
    }
}