// qt_gui/qml/main.qml
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import EnvironmentMonitor 1.0

ApplicationWindow {
    id: window
    visible: true
    width: 1200
    height: 800
    title: "Environment Monitor"
    
    property bool darkTheme: true
    
    color: darkTheme ? "#2b2b2b" : "#f5f5f5"
    
    MenuBar {
        Menu {
            title: "View"
            MenuItem {
                text: "Toggle Theme"
                onTriggered: darkTheme = !darkTheme
            }
            MenuItem {
                text: "Reset Layout"
                onTriggered: layoutManager.loadDefaultLayout()
            }
        }
        Menu {
            title: "Data"
            MenuItem {
                text: "Clear History"
                onTriggered: dataModel.clearHistory()
            }
            MenuItem {
                text: "Refresh"
                onTriggered: qtgui.refreshData()
            }
        }
    }
    
    StatusBar {
        id: statusBar
        anchors.bottom: parent.bottom
        width: parent.width
    }
    
    DynamicLayout {
        id: dynamicLayout
        anchors.top: parent.top
        anchors.bottom: statusBar.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10
        
        layoutConfig: layoutManager.currentLayout
    }
    
    // Connection status indicator
    Rectangle {
        id: connectionIndicator
        width: 12
        height: 12
        radius: 6
        color: dataModel.hasData ? "#4CAF50" : "#F44336"
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 15
        
        ToolTip.visible: connectionMouseArea.containsMouse
        ToolTip.text: dataModel.hasData ? "Connected" : "No Data"
        
        MouseArea {
            id: connectionMouseArea
            anchors.fill: parent
            hoverEnabled: true
        }
        
        SequentialAnimation on opacity {
            running: !dataModel.hasData
            loops: Animation.Infinite
            NumberAnimation { to: 0.3; duration: 1000 }
            NumberAnimation { to: 1.0; duration: 1000 }
        }
    }
    
    // Notifications area
    Item {
        id: notificationArea
        anchors.bottom: statusBar.top
        anchors.right: parent.right
        anchors.margins: 20
        width: 300
        height: 100
        
        // This could be expanded to show actual notifications
    }
}