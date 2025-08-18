// qt_gui/qml/DynamicLayout.qml
import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var layoutConfig: ({})
    
    onLayoutConfigChanged: {
        updateLayout()
    }
    
    function updateLayout() {
        // Clear existing items
        for (var i = container.children.length - 1; i >= 0; i--) {
            container.children[i].destroy()
        }
        
        if (!layoutConfig || !layoutConfig.panels) {
            return
        }
        
        var panels = layoutConfig.panels
        var grid = layoutConfig.grid || { columns: 2, rows: 2, margin: 10, spacing: 5 }
        
        container.columns = grid.columns
        container.rows = grid.rows
        container.columnSpacing = grid.spacing
        container.rowSpacing = grid.spacing
        
        // Create panels based on configuration
        for (var panelKey in panels) {
            var panelConfig = panels[panelKey]
            if (!panelConfig.visible) continue
            
            var component = getPanelComponent(panelConfig.type)
            if (component) {
                var panel = component.createObject(container, {
                    "title": panelConfig.title || panelConfig.type,
                    "panelColor": panelConfig.color || "#2196F3",
                    "chartType": panelConfig.chartType || "line"
                })
                
                if (panel && panelConfig.position) {
                    var pos = panelConfig.position
                    panel.Layout.column = pos.x || 0
                    panel.Layout.row = pos.y || 0
                    panel.Layout.columnSpan = pos.width || 1
                    panel.Layout.rowSpan = pos.height || 1
                }
            }
        }
    }
    
    function getPanelComponent(type) {
        switch(type) {
            case "sensor":
                return sensorPanelComponent
            case "algorithm":
                return algorithmPanelComponent
            case "chart":
                return chartPanelComponent
            default:
                return null
        }
    }
    
    GridLayout {
        id: container
        anchors.fill: parent
        anchors.margins: layoutConfig.grid ? layoutConfig.grid.margin : 10
    }
    
    Component {
        id: sensorPanelComponent
        SensorPanel {}
    }
    
    Component {
        id: algorithmPanelComponent
        AlgorithmPanel {}
    }
    
    Component {
        id: chartPanelComponent
        ChartPanel {}
    }
    
    Component.onCompleted: {
        updateLayout()
    }
    
    // Handle layout updates from layout manager
    Connections {
        target: layoutManager
        function onLayoutUpdateRequested(layout) {
            layoutConfig = layout
        }
    }
}