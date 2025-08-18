// DataModel.cpp
#include "DataModel.h"
#include <QDebug>
#include <QDateTime>

SensorDataPoint::SensorDataPoint(QObject* parent) : QObject(parent) {}

AlgorithmResultData::AlgorithmResultData(QObject* parent) : QObject(parent) {}

DataModel::DataModel(QObject* parent) : QObject(parent) {
    currentSensorData_ = new SensorDataPoint(this);
    currentResult_ = new AlgorithmResultData(this);
}

void DataModel::updateSensorData(const QString& sensorId, double temperature, 
                                double humidity, double pressure, qint64 timestamp) {
    currentSensorData_->setSensorId(sensorId);
    currentSensorData_->setTemperature(temperature);
    currentSensorData_->setHumidity(humidity);
    currentSensorData_->setPressure(pressure);
    currentSensorData_->setTimestamp(timestamp);
    
    updateHistoryData(temperature, humidity, pressure, timestamp);
    
    if (!hasData_) {
        hasData_ = true;
        emit hasDataChanged();
    }
    
    emit currentSensorDataChanged();
    emit dataChanged();
    
    qDebug() << "Updated sensor data:" << sensorId << temperature << humidity << pressure;
}

void DataModel::updateAlgorithmResult(const QString& resultId, double comfortIndex,
                                    const QString& alertLevel, const QString& recommendation,
                                    double avgTemp, double avgHumidity, double avgPressure) {
    currentResult_->setResultId(resultId);
    currentResult_->setComfortIndex(comfortIndex);
    currentResult_->setAlertLevel(alertLevel);
    currentResult_->setRecommendation(recommendation);
    
    emit currentResultChanged();
    emit dataChanged();
    
    qDebug() << "Updated algorithm result:" << resultId << comfortIndex << alertLevel;
}

void DataModel::updateHistoryData(double temperature, double humidity, double pressure, qint64 timestamp) {
    QVariantMap dataPoint;
    dataPoint["timestamp"] = timestamp;
    dataPoint["temperature"] = temperature;
    dataPoint["humidity"] = humidity;
    dataPoint["pressure"] = pressure;
    dataPoint["time"] = QDateTime::fromMSecsSinceEpoch(timestamp).toString("hh:mm:ss");
    
    historyBuffer_.push_back(dataPoint);
    trimHistory();
    
    // Update history lists
    temperatureHistory_.clear();
    humidityHistory_.clear();
    pressureHistory_.clear();
    
    for (const auto& point : historyBuffer_) {
        QVariantMap tempPoint, humPoint, pressPoint;
        tempPoint["x"] = point["timestamp"];
        tempPoint["y"] = point["temperature"];
        tempPoint["time"] = point["time"];
        temperatureHistory_.append(tempPoint);
        
        humPoint["x"] = point["timestamp"];
        humPoint["y"] = point["humidity"];
        humPoint["time"] = point["time"];
        humidityHistory_.append(humPoint);
        
        pressPoint["x"] = point["timestamp"];
        pressPoint["y"] = point["pressure"];
        pressPoint["time"] = point["time"];
        pressureHistory_.append(pressPoint);
    }
    
    emit temperatureHistoryChanged();
    emit humidityHistoryChanged();
    emit pressureHistoryChanged();
}

void DataModel::trimHistory() {
    while (historyBuffer_.size() > MAX_HISTORY_SIZE) {
        historyBuffer_.pop_front();
    }
}

void DataModel::clearHistory() {
    historyBuffer_.clear();
    temperatureHistory_.clear();
    humidityHistory_.clear();
    pressureHistory_.clear();
    
    emit temperatureHistoryChanged();
    emit humidityHistoryChanged();
    emit pressureHistoryChanged();
}