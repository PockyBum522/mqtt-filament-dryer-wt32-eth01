#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_TEMPERATURE_REPORTER_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_TEMPERATURE_REPORTER_H

#include <ArduinoJson.h>                 // PubSubClient - https://pubsubclient.knolleary.net/api
#include "MqttLogistics.h"
#include "../models/ThermostatStatus/ThermostatStatus.h"

class TemperatureReporter
{
public:
    explicit TemperatureReporter(ThermostatStatus* thermostatStatus,
                                 MqttLogistics* mqttLogistics);

    void SendTemperatureReportEveryTimeout();

private:
    ThermostatStatus* _thermostatStatus;
    MqttLogistics* _mqttLogistics;

   long long _lastPeripheralOutMessageSentSeconds = 0;

    std::string SerializeReport();
};


#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_TEMPERATURE_REPORTER_H
