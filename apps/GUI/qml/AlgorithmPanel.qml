// AlgorithmPanel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    property alias title: titleText.text
    property color panelColor: "#4CAF50"
    property bool darkTheme: window.darkTheme
    property string chartType    // Added property to fix initial property error

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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter   // Fixed anchor issue
                text: "Analysis Results"
                color: "white"
                font.bold: true
                font.pointSize: 14
            }
        }

        // Content
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            // Comfort Index with circular progress
            Item {
                Layout.fillWidth: true
                height: 150

                // Circular progress background
                Rectangle {
                    id: progressBackground
                    width: 120
                    height: 120
                    radius: 60
                    anchors.centerIn: parent   // Safe: not in layout
                    color: "transparent"
                    border.width: 8
                    border.color: darkTheme ? "#555" : "#ddd"
                }

                // Circular progress foreground
                Canvas {
                    id: progressCanvas
                    width: 120
                    height: 120
                    anchors.centerIn: parent  // Safe: not in layout

                    property real progress: dataModel.currentResult ? dataModel.currentResult.comfortIndex / 100 : 0

                    onProgressChanged: requestPaint()

                    onPaint: {
                        var ctx = getContext("2d");
                        var centerX = width / 2;
                        var centerY = height / 2;
                        var radius = 52;

                        ctx.clearRect(0, 0, width, height);

                        ctx.beginPath();
                        ctx.arc(centerX, centerY, radius, -Math.PI / 2,
                               -Math.PI / 2 + (progress * 2 * Math.PI));
                        ctx.lineWidth = 8;
                        ctx.strokeStyle = getComfortColor(dataModel.currentResult ? dataModel.currentResult.comfortIndex : 0);
                        ctx.stroke();
                    }

                    function getComfortColor(index) {
                        if (index >= 80) return "#4CAF50";
                        if (index >= 60) return "#FF9800";
                        if (index >= 40) return "#FF5722";
                        return "#F44336";
                    }
                }

                // Comfort index text in center
                Column {
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter   // Fixed anchors
                    spacing: 0

                    Text {
                        text: dataModel.currentResult ? Math.round(dataModel.currentResult.comfortIndex) : "0"
                        color: darkTheme ? "#fff" : "#333"
                        font.pointSize: 24
                        font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Text {
                        text: "Comfort"
                        color: darkTheme ? "#ccc" : "#666"
                        font.pointSize: 10
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // Alert level
            Rectangle {
                Layout.fillWidth: true
                height: 60
                radius: 8
                color: {
                    switch(dataModel.currentResult ? dataModel.currentResult.alertLevel : "") {
                        case "GOOD": return "#4CAF50"
                        case "MODERATE": return "#FF9800"
                        case "POOR": return "#FF5722"
                        case "CRITICAL": return "#F44336"
                        default: return "#999"
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    spacing: 5

                    Text {
                        text: "Alert Level"
                        color: "white"
                        font.pointSize: 12
                        opacity: 0.9
                        Layout.alignment: Qt.AlignHCenter
                    }
                    Text {
                        text: dataModel.currentResult ? dataModel.currentResult.alertLevel : ""
                        color: "white"
                        font.pointSize: 16
                        font.bold: true
                        Layout.alignment: Qt.AlignHCenter
                    }
                }
            }

            // Recommendation
            ScrollView {
                Layout.fillWidth: true
                Layout.fillHeight: true

                TextArea {
                    text: dataModel.currentResult ? dataModel.currentResult.recommendation : ""
                    color: darkTheme ? "#fff" : "#333"
                    wrapMode: TextArea.WordWrap
                    readOnly: true
                    selectByMouse: true
                    background: Rectangle {
                        color: darkTheme ? "#333" : "#f9f9f9"
                        radius: 4
                        border.color: darkTheme ? "#555" : "#ddd"
                    }
                }
            }
        }
    }
}
