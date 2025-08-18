// LayoutManager.h
#pragma once
#include <QObject>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>

class LayoutManager : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantMap currentLayout READ currentLayout NOTIFY currentLayoutChanged)
    Q_PROPERTY(bool isCustomLayout READ isCustomLayout NOTIFY isCustomLayoutChanged)
    Q_PROPERTY(QString layoutName READ layoutName NOTIFY layoutNameChanged)

public:
    explicit LayoutManager(QObject* parent = nullptr);
    
    QVariantMap currentLayout() const { return currentLayout_; }
    bool isCustomLayout() const { return isCustomLayout_; }
    QString layoutName() const { return layoutName_; }

public slots:
    void updateLayout(const QString& layoutJson);
    void loadDefaultLayout();
    void saveCurrentLayout(const QString& name);

signals:
    void currentLayoutChanged();
    void isCustomLayoutChanged();
    void layoutNameChanged();
    void layoutUpdateRequested(const QVariantMap& layout);

private:
    void setDefaultLayout();
    bool validateLayout(const QJsonObject& layout);

    QVariantMap currentLayout_;
    bool isCustomLayout_ = false;
    QString layoutName_ = "Default";
};