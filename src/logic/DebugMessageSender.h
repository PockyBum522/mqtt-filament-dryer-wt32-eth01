#ifndef MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H
#define MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H

#include <ArduinoJson.h>                 // PubSubClient - https://pubsubclient.knolleary.net/api
#include "logic/MqttLogistics.h"

class DebugMessageSender
{
public:
    explicit DebugMessageSender(CurrentThermostatStatus *currentThermostatStatus, MqttLogistics* mqttLogistics);

    void SendMqttDebugMessagesEveryTimeout();

private:
    CurrentThermostatStatus *_currentThermostatStatus;
    MqttLogistics *_mqttLogistics;

    unsigned long _lastDebugOutMessageSentSeconds = 0;
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_DEBUG_MESSAGE_SENDER_H
