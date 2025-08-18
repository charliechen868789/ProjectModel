// QtGUI.cpp
#include "QtGUI.h"
#include "sensor_data.pb.h"
#include <QGuiApplication>
#include <QQmlContext>
#include <QDebug>
#include <iostream>

QtGUI::QtGUI(QGuiApplication* app) 
    : AppTemplate("QtGUI", 20003), app_(app), engine_(nullptr), 
      dataModel_(nullptr), layoutManager_(nullptr), refreshTimer_(nullptr) {
}

QtGUI::~QtGUI() {
    cleanup();
}

void QtGUI::initialize() {
    std::cout << "[QtGUI] Initializing Qt GUI application" << std::endl;
    
    // Create data model and layout manager
    dataModel_ = new DataModel(this);
    layoutManager_ = new LayoutManager(this);
    
    // Register event handlers
    registerHandler("sensor.data", [this](const std::string& eventType, const std::string& data) {
        handleSensorData(eventType, data);
    });
    
    registerHandler("algorithm.result", [this](const std::string& eventType, const std::string& data) {
        handleAlgorithmResult(eventType, data);
    });
    
    registerHandler("layout.update", [this](const std::string& eventType, const std::string& data) {
        handleLayoutUpdate(eventType, data);
    });
    
    // Connect to other applications
    connectToPeer("127.0.0.1", 20001); // VirtualSensor
    connectToPeer("127.0.0.1", 20002); // Algorithm
    connectToPeer("127.0.0.1", 20004); // WebHandler
    
    setupQmlEngine();
    
    // Setup refresh timer
    refreshTimer_ = new QTimer(this);
    connect(refreshTimer_, &QTimer::timeout, this, &QtGUI::refreshData);
    refreshTimer_->start(1000); // Update every second
}

void QtGUI::run() {
    std::cout << "[QtGUI] Starting Qt GUI..." << std::endl;
    
    if (engine_) {
        engine_->load(QUrl(QStringLiteral("qrc:/EnvironmentMonitor/qt_gui/qml/main.qml")));
        
        if (engine_->rootObjects().isEmpty()) {
            std::cerr << "[QtGUI] Failed to load QML" << std::endl;
            return;
        }
    }
    
    // Qt application event loop runs in main thread
}

void QtGUI::cleanup() {
    if (refreshTimer_) {
        refreshTimer_->stop();
    }
    
    if (engine_) {
        delete engine_;
        engine_ = nullptr;
    }
    
    std::cout << "[QtGUI] Cleaned up Qt GUI" << std::endl;
}

void QtGUI::setupQmlEngine() {
    engine_ = new QQmlApplicationEngine(this);
    
    // Register QML types
    qmlRegisterType<DataModel>("EnvironmentMonitor", 1, 0, "DataModel");
    qmlRegisterType<LayoutManager>("EnvironmentMonitor", 1, 0, "LayoutManager");
    
    // Set context properties
    engine_->rootContext()->setContextProperty("dataModel", dataModel_);
    engine_->rootContext()->setContextProperty("layoutManager", layoutManager_);
    engine_->rootContext()->setContextProperty("qtgui", this);
    
    connect(dataModel_, &DataModel::dataChanged, this, &QtGUI::onDataModelChanged);
}

void QtGUI::handleSensorData(const std::string& eventType, const std::string& data) {
    SensorData sensorData;
    if (!sensorData.ParseFromString(data)) {
        return;
    }
    
    QMetaObject::invokeMethod(dataModel_, "updateSensorData", Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(sensorData.sensor_id())),
        Q_ARG(double, sensorData.temperature()),
        Q_ARG(double, sensorData.humidity()),
        Q_ARG(double, sensorData.pressure()),
        Q_ARG(qint64, sensorData.timestamp()));
}

void QtGUI::handleAlgorithmResult(const std::string& eventType, const std::string& data) {
    //AlgorithmResult result;
    //if (!result.ParseFromString(data)) {
    //    return;
    //}
    
    //QMetaObject::invokeMethod(dataModel_, "updateAlgorithmResult", Qt::QueuedConnection,
    //    Q_ARG(QString, QString::fromStdString(result.result_id())),
    //    Q_ARG(double, result.comfort_index()),
    //    Q_ARG(QString, QString::fromStdString(result.alert_level())),
    //    Q_ARG(QString, QString::fromStdString(result.recommendation())),
    //    Q_ARG(double, result.avg_temperature()),
    //    Q_ARG(double, result.avg_humidity()),
    //    Q_ARG(double, result.avg_pressure()));
}

void QtGUI::handleLayoutUpdate(const std::string& eventType, const std::string& data) {
    QMetaObject::invokeMethod(this, "updateLayout", Qt::QueuedConnection,
        Q_ARG(QString, QString::fromStdString(data)));
}

void QtGUI::refreshData() {
    // Trigger data refresh if needed
    emit dataModel_->refreshRequested();
}

void QtGUI::sendCommand(const QString& command, const QString& data) {
    std::string cmdStr = command.toStdString();
    std::string dataStr = data.toStdString();
    
    broadcast("gui.command", cmdStr + ":" + dataStr);
    qDebug() << "Sent command:" << command << "with data:" << data;
}

void QtGUI::updateLayout(const QString& layoutJson) {
    layoutManager_->updateLayout(layoutJson);
}

void QtGUI::onDataModelChanged() {
    // Handle data model changes if needed
}