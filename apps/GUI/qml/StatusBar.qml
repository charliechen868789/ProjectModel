// qt_gui/qml/StatusBar.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    height: 35
    color: window.darkTheme ? "#404040" : "#f0f0f0"
    border.color: window.darkTheme ? "#555" : "#ccc"
    border.width: 1
    
    RowLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 15
        
        // Connection status
        Row {
            spacing: 5
            
            Rectangle {
                width: 8
                height: 8
                radius: 4
                color: dataModel.hasData ? "#4CAF50" : "#F44336"
                anchors.verticalCenter: parent.verticalCenter
                
                SequentialAnimation on opacity {
                    running: !dataModel.hasData
                    loops: Animation.Infinite
                    NumberAnimation { to: 0.3; duration: 1000 }
                    NumberAnimation { to: 1.0; duration: 1000 }
                }
            }
            
            Text {
                text: dataModel.hasData ? "Connected" : "No Connection"
                color: window.darkTheme ? "#fff" : "#333"
                font.pointSize: 9
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        
        // Separator
        Rectangle {
            width: 1
            height: 20
            color: window.darkTheme ? "#555" : "#ccc"
        }
        
        // Data info
        Text {
            text: dataModel.hasData ? 
                  "Last update: " + new Date(dataModel.currentSensorData.timestamp).toLocaleTimeString() :
                  "Waiting for data..."
            color: window.darkTheme ? "#ccc" : "#666"
            font.pointSize: 9
        }
        
        // Separator
        Rectangle {
            width: 1
            height: 20
            color: window.darkTheme ? "#555" : "#ccc"
        }
        
        // Layout info
        Text {
            text: "Layout: " + layoutManager.layoutName
            color: window.darkTheme ? "#ccc" : "#666"
            font.pointSize: 9
        }
        
        Item { Layout.fillWidth: true }
        
        // System time
        Text {
            id: timeText
            color: window.darkTheme ? "#fff" : "#333"
            font.pointSize: 9
            
            Timer {
                interval: 1000
                repeat: true
                running: true
                onTriggered: timeText.text = new Date().toLocaleTimeString()
            }
            
            Component.onCompleted: text = new Date().toLocaleTimeString()
        }
    }
}