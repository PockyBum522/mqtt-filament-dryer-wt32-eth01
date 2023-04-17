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
    if (!_thermostatStatus->Settings.DebugModeOn)
        return;

   long secondsSinceLastDebugMessageSent = _thermostatStatus->CurrentSeconds - _lastDebugOutMessageSentSeconds;

    if (secondsSinceLastDebugMessageSent > _thermostatStatus->Settings.DebugMessageSendIntervalSeconds)
    {
        String outputJson = "";

        DebugMessageSender::_lastDebugOutMessageSentSeconds = _thermostatStatus->CurrentSeconds;

        StaticJsonDocument<556> jsonDocument;

        jsonDocument["CurrentSeconds"] = _thermostatStatus->CurrentSeconds;
        jsonDocument["secondsSinceLastDebugMessageSent"] = secondsSinceLastDebugMessageSent;

        jsonDocument["LastSeenMqttCommand"] = _mqttLogistics->getLastCommand();

        jsonDocument["SetpointTemperatureCelsius"] = _thermostatStatus->SetpointTemperatureCelsius;

        jsonDocument["PIN_HEATER_CONTROL"] = digitalRead(PIN_HEATER_CONTROL);
        jsonDocument["PIN_FAN_CONTROL"] = digitalRead(PIN_FAN_CONTROL);
        jsonDocument["PIN_BEEPER_CONTROL"] = digitalRead(PIN_BEEPER_CONTROL);


        serializeJson(jsonDocument, outputJson);

        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_OUT, outputJson.c_str());

        if (_thermostatStatus->Settings.DebugModeOn)
        {
            Serial.println("Sent debug message");
            Serial.println(outputJson);
        }
    }
}
