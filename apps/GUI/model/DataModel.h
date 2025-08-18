// DataModel.h
#pragma once
#include <QObject>
#include <QAbstractListModel>
#include <QQmlListProperty>
#include <QVariantMap>
#include <QTimer>
#include <deque>

class SensorDataPoint : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString sensorId READ sensorId WRITE setSensorId NOTIFY sensorIdChanged)
    Q_PROPERTY(double temperature READ temperature WRITE setTemperature NOTIFY temperatureChanged)
    Q_PROPERTY(double humidity READ humidity WRITE setHumidity NOTIFY humidityChanged)
    Q_PROPERTY(double pressure READ pressure WRITE setPressure NOTIFY pressureChanged)
    Q_PROPERTY(qint64 timestamp READ timestamp WRITE setTimestamp NOTIFY timestampChanged)

public:
    explicit SensorDataPoint(QObject* parent = nullptr);
    
    QString sensorId() const { return sensorId_; }
    void setSensorId(const QString& id) { 
        if (sensorId_ != id) { 
            sensorId_ = id; 
            emit sensorIdChanged(); 
        } 
    }
    
    double temperature() const { return temperature_; }
    void setTemperature(double temp) { 
        if (qAbs(temperature_ - temp) > 0.01) { 
            temperature_ = temp; 
            emit temperatureChanged(); 
        } 
    }
    
    double humidity() const { return humidity_; }
    void setHumidity(double hum) { 
        if (qAbs(humidity_ - hum) > 0.01) { 
            humidity_ = hum; 
            emit humidityChanged(); 
        } 
    }
    
    double pressure() const { return pressure_; }
    void setPressure(double press) { 
        if (qAbs(pressure_ - press) > 0.01) { 
            pressure_ = press; 
            emit pressureChanged(); 
        } 
    }
    
    qint64 timestamp() const { return timestamp_; }
    void setTimestamp(qint64 ts) { 
        if (timestamp_ != ts) { 
            timestamp_ = ts; 
            emit timestampChanged(); 
        } 
    }

signals:
    void sensorIdChanged();
    void temperatureChanged();
    void humidityChanged();
    void pressureChanged();
    void timestampChanged();

private:
    QString sensorId_;
    double temperature_ = 0.0;
    double humidity_ = 0.0;
    double pressure_ = 0.0;
    qint64 timestamp_ = 0;
};

class AlgorithmResultData : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString resultId READ resultId WRITE setResultId NOTIFY resultIdChanged)
    Q_PROPERTY(double comfortIndex READ comfortIndex WRITE setComfortIndex NOTIFY comfortIndexChanged)
    Q_PROPERTY(QString alertLevel READ alertLevel WRITE setAlertLevel NOTIFY alertLevelChanged)
    Q_PROPERTY(QString recommendation READ recommendation WRITE setRecommendation NOTIFY recommendationChanged)

public:
    explicit AlgorithmResultData(QObject* parent = nullptr);
    
    QString resultId() const { return resultId_; }
    void setResultId(const QString& id) { 
        if (resultId_ != id) { 
            resultId_ = id; 
            emit resultIdChanged(); 
        } 
    }
    
    double comfortIndex() const { return comfortIndex_; }
    void setComfortIndex(double index) { 
        if (qAbs(comfortIndex_ - index) > 0.01) { 
            comfortIndex_ = index; 
            emit comfortIndexChanged(); 
        } 
    }
    
    QString alertLevel() const { return alertLevel_; }
    void setAlertLevel(const QString& level) { 
        if (alertLevel_ != level) { 
            alertLevel_ = level; 
            emit alertLevelChanged(); 
        } 
    }
    
    QString recommendation() const { return recommendation_; }
    void setRecommendation(const QString& rec) { 
        if (recommendation_ != rec) { 
            recommendation_ = rec; 
            emit recommendationChanged(); 
        } 
    }

signals:
    void resultIdChanged();
    void comfortIndexChanged();
    void alertLevelChanged();
    void recommendationChanged();

private:
    QString resultId_;
    double comfortIndex_ = 0.0;
    QString alertLevel_;
    QString recommendation_;
};

class DataModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(SensorDataPoint* currentSensorData READ currentSensorData NOTIFY currentSensorDataChanged)
    Q_PROPERTY(AlgorithmResultData* currentResult READ currentResult NOTIFY currentResultChanged)
    Q_PROPERTY(QVariantList temperatureHistory READ temperatureHistory NOTIFY temperatureHistoryChanged)
    Q_PROPERTY(QVariantList humidityHistory READ humidityHistory NOTIFY humidityHistoryChanged)
    Q_PROPERTY(QVariantList pressureHistory READ pressureHistory NOTIFY pressureHistoryChanged)
    Q_PROPERTY(bool hasData READ hasData NOTIFY hasDataChanged)

public:
    explicit DataModel(QObject* parent = nullptr);
    
    SensorDataPoint* currentSensorData() const { return currentSensorData_; }
    AlgorithmResultData* currentResult() const { return currentResult_; }
    
    QVariantList temperatureHistory() const { return temperatureHistory_; }
    QVariantList humidityHistory() const { return humidityHistory_; }
    QVariantList pressureHistory() const { return pressureHistory_; }
    
    bool hasData() const { return hasData_; }

public slots:
    void updateSensorData(const QString& sensorId, double temperature, 
                         double humidity, double pressure, qint64 timestamp);
    void updateAlgorithmResult(const QString& resultId, double comfortIndex,
                             const QString& alertLevel, const QString& recommendation,
                             double avgTemp, double avgHumidity, double avgPressure);
    void clearHistory();

signals:
    void currentSensorDataChanged();
    void currentResultChanged();
    void temperatureHistoryChanged();
    void humidityHistoryChanged();
    void pressureHistoryChanged();
    void hasDataChanged();
    void dataChanged();
    void refreshRequested();

private:
    void updateHistoryData(double temperature, double humidity, double pressure, qint64 timestamp);
    void trimHistory();

    SensorDataPoint* currentSensorData_;
    AlgorithmResultData* currentResult_;
    
    QVariantList temperatureHistory_;
    QVariantList humidityHistory_;
    QVariantList pressureHistory_;
    
    std::deque<QVariantMap> historyBuffer_;
    bool hasData_ = false;
    
    static constexpr int MAX_HISTORY_SIZE = 50;
};