// qt_gui/qml/SensorPanel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property alias title: titleText.text
    property color panelColor: "#2196F3"
    property bool darkTheme: window.darkTheme
    
    color: darkTheme ? "#3f3f3f" : "white"
    border.color: darkTheme ? "#555" : "#ddd"
    border.width: 1
    radius: 8
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // Header
        Rectangle {
            Layout.fillWidth: true
            height: 40
            color: panelColor
            radius: 4
            
            Text {
                id: titleText
                anchors.centerIn: parent
                text: "Sensor Data"
                color: "white"
                font.bold: true
                font.pointSize: 14
            }
        }
        
        // Content
        GridLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            columns: 2
            rowSpacing: 15
            columnSpacing: 20
            
            // Temperature
            Column {
                spacing: 5
                Text {
                    text: "Temperature"
                    color: darkTheme ? "#fff" : "#333"
                    font.pointSize: 11
                    font.bold: true
                }
                Text {
                    text: dataModel.currentSensorData.temperature.toFixed(1) + "°C"
                    color: darkTheme ? "#4CAF50" : "#2E7D32"
                    font.pointSize: 24
                    font.bold: true
                }
            }
            
            // Humidity
            Column {
                spacing: 5
                Text {
                    text: "Humidity"
                    color: darkTheme ? "#fff" : "#333"
                    font.pointSize: 11
                    font.bold: true
                }
                Text {
                    text: dataModel.currentSensorData.humidity.toFixed(1) + "%"
                    color: darkTheme ? "#2196F3" : "#1565C0"
                    font.pointSize: 24
                    font.bold: true
                }
            }
            
            // Pressure
            Column {
                spacing: 5
                Text {
                    text: "Pressure"
                    color: darkTheme ? "#fff" : "#333"
                    font.pointSize: 11
                    font.bold: true
                }
                Text {
                    text: dataModel.currentSensorData.pressure.toFixed(1) + " hPa"
                    color: darkTheme ? "#FF9800" : "#F57C00"
                    font.pointSize: 24
                    font.bold: true
                }
            }
            
            // Sensor ID and timestamp
            Column {
                spacing: 5
                Text {
                    text: "Sensor ID"
                    color: darkTheme ? "#fff" : "#333"
                    font.pointSize: 11
                    font.bold: true
                }
                Text {
                    text: dataModel.currentSensorData.sensorId
                    color: darkTheme ? "#ccc" : "#666"
                    font.pointSize: 12
                }
                Text {
                    text: new Date(dataModel.currentSensorData.timestamp).toLocaleTimeString()
                    color: darkTheme ? "#999" : "#888"
                    font.pointSize: 10
                }
            }
        }
        
        // Quick actions
        RowLayout {
            Layout.fillWidth: true
            
            Button {
                text: "Refresh"
                onClicked: qtgui.refreshData()
            }
            
            Item { Layout.fillWidth: true }
            
            Button {
                text: "Export"
                enabled: dataModel.hasData
                onClicked: {
                    // Could implement export functionality
                    console.log("Export sensor data")
                }
            }
        }
    }
}