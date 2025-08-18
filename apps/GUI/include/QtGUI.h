#pragma once
#include "AppTemplate.h"
#include "DataModel.h"
#include "LayoutManager.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTimer>
#include <memory>

class QtGUI : public QObject, public AppTemplate {
    Q_OBJECT

public:
    QtGUI(QGuiApplication* app);
    ~QtGUI() override;

protected:
    void initialize() override;
    void run() override;
    void cleanup() override;

public slots:
    void refreshData();
    void sendCommand(const QString& command, const QString& data);
    void updateLayout(const QString& layoutJson);

private slots:
    void onDataModelChanged();

private:
    void handleSensorData(const std::string& eventType, const std::string& data);
    void handleAlgorithmResult(const std::string& eventType, const std::string& data);
    void handleLayoutUpdate(const std::string& eventType, const std::string& data);
    void setupQmlEngine();

    QGuiApplication* app_;
    QQmlApplicationEngine* engine_;
    DataModel* dataModel_;
    LayoutManager* layoutManager_;
    QTimer* refreshTimer_;
    std::mutex dataMutex_;
};