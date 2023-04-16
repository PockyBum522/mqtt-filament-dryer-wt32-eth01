#ifndef MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H
#define MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H

#include <ArduinoJson.h>
#include "../models/ThermostatStatus/ThermostatStatus.h"
#include "MqttLogistics.h"

class DebugMessageSender
{
public:
    explicit DebugMessageSender(ThermostatStatus *thermostatStatus, MqttLogistics* mqttLogistics);

    void SendMqttDebugMessagesEveryTimeout();

private:
    ThermostatStatus *_thermostatStatus;
    MqttLogistics *_mqttLogistics;

   long _lastDebugOutMessageSentSeconds = 0;
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H
