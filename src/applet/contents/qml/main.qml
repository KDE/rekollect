import Qt 4.7
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: page;
    width: 200
    height: 200
    anchors.fill: parent

    PlasmaCore.DataSource {
        id: rekollectNotesEngineSource
        engine: "rekollectnotes"
        interval: 0

        onDataChanged: {
        }
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

        model: PlasmaCore.DataModel {
            dataSource: rekollectNotesEngineSource
        }

        delegate: Item {
            id: noteItem
            height: 20
            width: parent.width

            Text {
                id: documentNameText
                text: documentname
                anchors.fill: parent
            }
        }
    }
}