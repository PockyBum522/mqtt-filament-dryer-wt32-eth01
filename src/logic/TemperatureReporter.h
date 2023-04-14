#ifndef MQTT_THERMOSTAT_WT32_ETH01_TEMPERATURE_REPORTER_H
#define MQTT_THERMOSTAT_WT32_ETH01_TEMPERATURE_REPORTER_H

#include <ArduinoJson.h>                 // PubSubClient - https://pubsubclient.knolleary.net/api
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "logic/MqttLogistics.h"

class TemperatureReporter
{
public:
    explicit TemperatureReporter(CurrentThermostatStatus* currentThermostatStatus,
                                 MqttLogistics* mqttLogistics);

    void SendTemperatureReportEveryTimeout();

private:
    CurrentThermostatStatus* _currentThermostatStatus;
    MqttLogistics* _mqttLogistics;

    unsigned long _lastPeripheralOutMessageSentSeconds = 0;

    std::string SerializeReport();

    String getThermostatModeAsString(ThermostatModeStates thermostatMode);
    String getFanModeAsString(FanStates thermostatMode);
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_TEMPERATURE_REPORTER_H
