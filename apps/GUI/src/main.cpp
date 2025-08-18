// main.cpp for Qt GUI
#include "QtGUI.h"
#include <QGuiApplication>
#include <QCoreApplication> 
#include <QQmlApplicationEngine>
#include <QIcon>
#include <csignal>
#include <memory>

std::unique_ptr<QtGUI> qtGuiApp;

void signalHandler(int) {
    if (qtGuiApp) {
        qtGuiApp->stop();
    }
    QCoreApplication::quit();
}

int main(int argc, char *argv[]) {
    QGuiApplication  app(argc, argv);
    
    app.setApplicationName("Environment Monitor");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Environmental Solutions");
    
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    qtGuiApp = std::make_unique<QtGUI>(&app);
    qtGuiApp->start();
    
    int result = app.exec();
    
    if (qtGuiApp) {
        qtGuiApp->stop();
    }
    
    return result;
}