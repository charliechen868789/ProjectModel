// qt_gui/qml/ChartPanel.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtCharts 2.15

Rectangle {
    id: root
    property alias title: titleText.text
    property color panelColor: "#FF9800"
    property bool darkTheme: window.darkTheme
    property string chartType: "line"
    
    color: darkTheme ? "#3f3f3f" : "white"
    border.color: darkTheme ? "#555" : "#ddd"
    border.width: 1
    radius: 8
    
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 10
        
        // Header with controls
        RowLayout {
            Layout.fillWidth: true
            
            Rectangle {
                Layout.preferredWidth: 200
                height: 40
                color: panelColor
                radius: 4
                
                Text {
                    id: titleText
                    anchors.centerIn: parent
                    text: "Historical Data"
                    color: "white"
                    font.bold: true
                    font.pointSize: 14
                }
            }
            
            Item { Layout.fillWidth: true }
            
            ComboBox {
                id: chartTypeCombo
                model: ["Temperature", "Humidity", "Pressure", "All"]
                currentIndex: 3
                onCurrentTextChanged: updateChart()
            }
            
            Button {
                text: "Clear"
                onClicked: dataModel.clearHistory()
            }
        }
        
        // Chart area
        ChartView {
            id: chartView
            Layout.fillWidth: true
            Layout.fillHeight: true
            backgroundColor: darkTheme ? "#2b2b2b" : "white"
            titleColor: darkTheme ? "#fff" : "#333"
            theme: darkTheme ? ChartView.ChartThemeDark : ChartView.ChartThemeLight
            
            legend.visible: chartTypeCombo.currentText === "All"
            legend.alignment: Qt.AlignBottom
            
            // Date/Time axis
            DateTimeAxis {
                id: axisX
                format: "hh:mm:ss"
                titleText: "Time"
            }
            
            // Value axis
            ValueAxis {
                id: axisY
                titleText: getAxisTitle()
                
                function getAxisTitle() {
                    switch(chartTypeCombo.currentText) {
                        case "Temperature": return "Temperature (°C)"
                        case "Humidity": return "Humidity (%)"
                        case "Pressure": return "Pressure (hPa)"
                        default: return "Values"
                    }
                }
            }
            
            // Temperature line
            LineSeries {
                id: tempSeries
                name: "Temperature (°C)"
                axisX: axisX
                axisY: axisY
                color: "#F44336"
                width: 2
                visible: chartTypeCombo.currentText === "Temperature" || chartTypeCombo.currentText === "All"
            }
            
            // Humidity line
            LineSeries {
                id: humiditySeries
                name: "Humidity (%)"
                axisX: axisX
                axisY: axisY
                color: "#2196F3"
                width: 2
                visible: chartTypeCombo.currentText === "Humidity" || chartTypeCombo.currentText === "All"
            }
            
            // Pressure line (scaled for display)
            LineSeries {
                id: pressureSeries
                name: "Pressure (hPa/10)"
                axisX: axisX
                axisY: axisY
                color: "#FF9800"
                width: 2
                visible: chartTypeCombo.currentText === "Pressure" || chartTypeCombo.currentText === "All"
            }
        }
        
        // Chart update connections
        Connections {
            target: dataModel
            function onTemperatureHistoryChanged() {
                updateTemperatureData()
            }
            function onHumidityHistoryChanged() {
                updateHumidityData()
            }
            function onPressureHistoryChanged() {
                updatePressureData()
            }
        }
    }
    
    function updateChart() {
        updateTemperatureData()
        updateHumidityData()
        updatePressureData()
        updateAxisRanges()
    }
    
    function updateTemperatureData() {
        tempSeries.clear()
        var data = dataModel.temperatureHistory
        for (var i = 0; i < data.length; i++) {
            tempSeries.append(data[i].x, data[i].y)
        }
    }
    
    function updateHumidityData() {
        humiditySeries.clear()
        var data = dataModel.humidityHistory
        for (var i = 0; i < data.length; i++) {
            humiditySeries.append(data[i].x, data[i].y)
        }
    }
    
    function updatePressureData() {
        pressureSeries.clear()
        var data = dataModel.pressureHistory
        for (var i = 0; i < data.length; i++) {
            // Scale pressure for better visualization when showing all data
            var scaledValue = chartTypeCombo.currentText === "All" ? 
                              data[i].y / 10 : data[i].y
            pressureSeries.append(data[i].x, scaledValue)
        }
    }
    
    function updateAxisRanges() {
        if (dataModel.temperatureHistory.length > 0) {
            var now = Date.now()
            axisX.min = new Date(now - 60000) // Last minute
            axisX.max = new Date(now)
            
            // Auto-scale Y axis based on visible data
            switch(chartTypeCombo.currentText) {
                case "Temperature":
                    axisY.min = 15
                    axisY.max = 35
                    break
                case "Humidity":
                    axisY.min = 0
                    axisY.max = 100
                    break
                case "Pressure":
                    axisY.min = 950
                    axisY.max = 1050
                    break
                default:
                    axisY.min = 0
                    axisY.max = 120
            }
        }
    }
    
    Component.onCompleted: {
        updateChart()
    }
}