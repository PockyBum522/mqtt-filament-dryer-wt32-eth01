#include "DebugMessageSender.h"
#include "MqttLogistics.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"

DebugMessageSender::DebugMessageSender(CurrentThermostatStatus* currentThermostatStatus,
                                       MqttLogistics* mqttLogistics)
{
    DebugMessageSender::_currentThermostatStatus = currentThermostatStatus;
    DebugMessageSender::_mqttLogistics = mqttLogistics;
}

void DebugMessageSender::SendMqttDebugMessagesEveryTimeout()
{
    if (!_currentThermostatStatus->CurrentSettings.DebugModeOn)
        return;

    unsigned long secondsSinceLastDebugMessageSent = DebugMessageSender::_currentThermostatStatus->CurrentSeconds - _lastDebugOutMessageSentSeconds;

    if (secondsSinceLastDebugMessageSent > DebugMessageSender::_currentThermostatStatus->CurrentSettings.DebugMessageSendIntervalSeconds)
    {
        DebugMessageSender::_lastDebugOutMessageSentSeconds = DebugMessageSender::_currentThermostatStatus->CurrentSeconds;

        StaticJsonDocument<356> jsonDocument;

        jsonDocument["CurrentSeconds"] = _currentThermostatStatus->CurrentSeconds;
        jsonDocument["LastCompressorOffAtSeconds"] = _currentThermostatStatus->LastCompressorOffAtSeconds;
        jsonDocument["CompressorDelayIfNegative"] = (long)((long)_currentThermostatStatus->CurrentSeconds - (long)_currentThermostatStatus->LastCompressorOffAtSeconds) - (long)_currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;
        jsonDocument["secondsSinceLastDebugMessageSent"] = secondsSinceLastDebugMessageSent;

        jsonDocument["LastSeenMqttCommand"] = _mqttLogistics->getLastCommand();

        jsonDocument["PinCompressorStatus"] = !digitalRead(PIN_YELLOW_COMPRESSOR_CALL);
        jsonDocument["PinFanStatus"] = !digitalRead(PIN_GREEN_FAN_CALL);
        jsonDocument["PinReverserValveStatus"] = !digitalRead(PIN_ORANGE_REVERSER_VALVE);
        jsonDocument["PinAuxHeatStatus"] = !digitalRead(PIN_WHITE_AUX_HEAT);
        jsonDocument["Relay05Status"] = !digitalRead(PIN_RELAY_05);
        jsonDocument["Relay06Status"] = !digitalRead(PIN_RELAY_06);

        String outputJson = "";

        serializeJson(jsonDocument, outputJson);

        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_OUT, outputJson.c_str());

        if (DebugMessageSender::_currentThermostatStatus->CurrentSettings.DebugModeOn)
        {
            Serial.println("Sent debug message");
            Serial.println(outputJson);
        }
    }
}
