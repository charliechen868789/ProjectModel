// LayoutManager.cpp
#include "LayoutManager.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

LayoutManager::LayoutManager(QObject* parent) : QObject(parent) {
    setDefaultLayout();
}

void LayoutManager::updateLayout(const QString& layoutJson) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(layoutJson.toUtf8(), &error);
    
    if (error.error != QJsonParseError::NoError) {
        qWarning() << "Failed to parse layout JSON:" << error.errorString();
        return;
    }
    
    QJsonObject layoutObj = doc.object();
    if (!validateLayout(layoutObj)) {
        qWarning() << "Invalid layout structure";
        return;
    }
    
    currentLayout_ = layoutObj.toVariantMap();
    isCustomLayout_ = true;
    layoutName_ = layoutObj.value("name").toString("Custom Layout");
    
    emit currentLayoutChanged();
    emit isCustomLayoutChanged();
    emit layoutNameChanged();
    emit layoutUpdateRequested(currentLayout_);
    
    qDebug() << "Layout updated:" << layoutName_;
}

void LayoutManager::loadDefaultLayout() {
    setDefaultLayout();
    isCustomLayout_ = false;
    layoutName_ = "Default";
    
    emit currentLayoutChanged();
    emit isCustomLayoutChanged();
    emit layoutNameChanged();
}

void LayoutManager::saveCurrentLayout(const QString& name) {
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    QDir().mkpath(configDir);
    
    QString fileName = configDir + "/layout_" + name + ".json";
    QFile file(fileName);
    
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc = QJsonDocument::fromVariant(currentLayout_);
        file.write(doc.toJson());
        file.close();
        qDebug() << "Layout saved to:" << fileName;
    } else {
        qWarning() << "Failed to save layout to:" << fileName;
    }
}

void LayoutManager::setDefaultLayout() {
    QVariantMap layout;
    layout["name"] = "Default Layout";
    layout["version"] = "1.0";
    
    // Panel configurations
    QVariantMap panels;
    
    // Sensor panel
    QVariantMap sensorPanel;
    sensorPanel["type"] = "sensor";
    sensorPanel["title"] = "Sensor Data";
    sensorPanel["position"] = QVariantMap{{"x", 0}, {"y", 0}, {"width", 1}, {"height", 1}};
    sensorPanel["visible"] = true;
    sensorPanel["color"] = "#2196F3";
    panels["sensor"] = sensorPanel;
    
    // Algorithm panel
    QVariantMap algorithmPanel;
    algorithmPanel["type"] = "algorithm";
    algorithmPanel["title"] = "Analysis Results";
    algorithmPanel["position"] = QVariantMap{{"x", 1}, {"y", 0}, {"width", 1}, {"height", 1}};
    algorithmPanel["visible"] = true;
    algorithmPanel["color"] = "#4CAF50";
    panels["algorithm"] = algorithmPanel;
    
    // Chart panel
    QVariantMap chartPanel;
    chartPanel["type"] = "chart";
    chartPanel["title"] = "Historical Data";
    chartPanel["position"] = QVariantMap{{"x", 0}, {"y", 1}, {"width", 2}, {"height", 1}};
    chartPanel["visible"] = true;
    chartPanel["color"] = "#FF9800";
    chartPanel["chartType"] = "line";
    panels["chart"] = chartPanel;
    
    layout["panels"] = panels;
    
    // Grid configuration
    QVariantMap grid;
    grid["columns"] = 2;
    grid["rows"] = 2;
    grid["margin"] = 10;
    grid["spacing"] = 5;
    layout["grid"] = grid;
    
    currentLayout_ = layout;
}

bool LayoutManager::validateLayout(const QJsonObject& layout) {
    // Basic validation - check required fields
    if (!layout.contains("name") || !layout.contains("panels")) {
        return false;
    }
    
    QJsonObject panels = layout.value("panels").toObject();
    for (auto it = panels.begin(); it != panels.end(); ++it) {
        QJsonObject panel = it.value().toObject();
        if (!panel.contains("type") || !panel.contains("position")) {
            return false;
        }
    }
    
    return true;
}