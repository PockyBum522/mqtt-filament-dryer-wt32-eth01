#include "DebugMessageSender.h"
#include "MqttLogistics.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"

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
//    unsigned long secondsSinceLastDebugMessageSent = DebugMessageSender::_thermostatStatus->CurrentSeconds - _lastDebugOutMessageSentSeconds;
//
//    if (secondsSinceLastDebugMessageSent > DebugMessageSender::_thermostatStatus->Settings.DebugMessageSendIntervalSeconds)
//    {
//        DebugMessageSender::_lastDebugOutMessageSentSeconds = DebugMessageSender::_thermostatStatus->CurrentSeconds;
//
//        StaticJsonDocument<356> jsonDocument;
//
//        jsonDocument["CurrentSeconds"] = _thermostatStatus->CurrentSeconds;
//        jsonDocument["LastCompressorOffAtSeconds"] = _thermostatStatus->LastCompressorOffAtSeconds;
//        jsonDocument["CompressorDelayIfNegative"] = (long)((long)_thermostatStatus->CurrentSeconds - (long)_thermostatStatus->LastCompressorOffAtSeconds) - (long)_thermostatStatus->Settings.CompressorTimeoutSeconds;
//        jsonDocument["secondsSinceLastDebugMessageSent"] = secondsSinceLastDebugMessageSent;
//
//        jsonDocument["LastSeenMqttCommand"] = _mqttLogistics->getLastCommand();
//
//        jsonDocument["PinCompressorStatus"] = !digitalRead(PIN_YELLOW_COMPRESSOR_CALL);
//        jsonDocument["PinFanStatus"] = !digitalRead(PIN_GREEN_FAN_CALL);
//        jsonDocument["PinReverserValveStatus"] = !digitalRead(PIN_ORANGE_REVERSER_VALVE);
//        jsonDocument["PinAuxHeatStatus"] = !digitalRead(PIN_WHITE_AUX_HEAT);
//        jsonDocument["Relay05Status"] = !digitalRead(PIN_RELAY_05);
//        jsonDocument["Relay06Status"] = !digitalRead(PIN_RELAY_06);
//
//        String outputJson = "";
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
