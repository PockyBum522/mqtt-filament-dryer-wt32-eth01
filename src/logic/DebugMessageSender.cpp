#include "DebugMessageSender.h"
#include "MqttLogistics.h"
#include "secrets/SECRETS.h"
#include "../models/PinDefinitions.h"
#include "../secrets/SECRETS.h"

DebugMessageSender::DebugMessageSender(ThermostatStatus* thermostatStatus,
                                       MqttLogistics* mqttLogistics)
{
    DebugMessageSender::_thermostatStatus = thermostatStatus;
    DebugMessageSender::_mqttLogistics = mqttLogistics;
}

void DebugMessageSender::SendMqttDebugMessagesEveryTimeout()
{
//    if (!_thermostatStatus->Settings.DebugModeOn)
//        return;
//
//   long secondsSinceLastDebugMessageSent = DebugMessageSender::_thermostatStatus->CurrentSeconds - _lastDebugOutMessageSentSeconds;
//
//    if (secondsSinceLastDebugMessageSent > DebugMessageSender::_thermostatStatus->Settings.DebugMessageSendIntervalSeconds)
//    {
//        String outputJson = "";
//
//        DebugMessageSender::_lastDebugOutMessageSentSeconds = DebugMessageSender::_thermostatStatus->CurrentSeconds;
//
//        StaticJsonDocument<356> jsonDocument;
//
//        jsonDocument["CurrentSeconds"] = _thermostatStatus->CurrentSeconds;
//        jsonDocument["secondsSinceLastDebugMessageSent"] = secondsSinceLastDebugMessageSent;
//
//        jsonDocument["LastSeenMqttCommand"] = _mqttLogistics->getLastCommand();
//
//        jsonDocument["Setpoint"] = _thermostatStatus->Setpoint;
//
//        jsonDocument["PIN_RELAY_01"] = !digitalRead(PIN_RELAY_01);
//        jsonDocument["PIN_RELAY_02"] = !digitalRead(PIN_RELAY_02);
//        jsonDocument["PIN_RELAY_03"] = !digitalRead(PIN_RELAY_03);
//        jsonDocument["PIN_RELAY_04"] = !digitalRead(PIN_RELAY_04);
//
//        serializeJson(jsonDocument, outputJson);
//
//        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_OUT, outputJson.c_str());
//
//        if (DebugMessageSender::_thermostatStatus->Settings.DebugModeOn)
//        {
//            Serial.println("Sent debug message");
//            Serial.println(outputJson);
//        }
//    }
}
