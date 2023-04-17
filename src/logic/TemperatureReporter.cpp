#include <sstream>
#include <iomanip>
#include "TemperatureReporter.h"
#include "MqttLogistics.h"
#include "ConvertersToString.h"
#include "../secrets/SECRETS.h"

TemperatureReporter::TemperatureReporter(ThermostatStatus* thermostatStatus,
                                         MqttLogistics* mqttLogistics)
{
    _thermostatStatus = thermostatStatus;
    _mqttLogistics = mqttLogistics;
}

void TemperatureReporter::SendTemperatureReportEveryTimeout()
{
   long secondsSinceLastReportSent = _thermostatStatus->CurrentSeconds - _lastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > _thermostatStatus->Settings.TemperatureReportMessageSendIntervalSeconds)
    {
        _lastPeripheralOutMessageSentSeconds = _thermostatStatus->CurrentSeconds;                 // Reset time since last message

        std::string outputJson = SerializeReport();

        _mqttLogistics->publish(SECRETS::TOPIC_JSON_OUT, outputJson.c_str());

        char temperatureBuffer[7];
        char setpointBuffer[7];
        char humidityBuffer[7];
        char pidBuffer[7];

        dtostrf(_thermostatStatus->TemperatureCelsius, 5, 2, temperatureBuffer);
        dtostrf(_thermostatStatus->SetpointTemperatureCelsius, 5, 2, setpointBuffer);
        dtostrf(_thermostatStatus->Humidity, 5, 2, humidityBuffer);
        dtostrf(_thermostatStatus->PidValue, 5, 2, pidBuffer);

        _mqttLogistics->publish(SECRETS::TOPIC_JUST_TEMPERATURE_PERIPHERAL_OUT, temperatureBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_HUMIDITY_PERIPHERAL_OUT, humidityBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_PID_VALUE_PERIPHERAL_OUT, pidBuffer);

        if (_thermostatStatus->Settings.DebugModeOn)
        {
            Serial.print("Sent report: ");
            Serial.println(outputJson.c_str());
        }
    }
}

std::string TemperatureReporter::SerializeReport()
{
    StaticJsonDocument<300> jsonDocument;

    std::stringstream temperatureStream;
    temperatureStream << std::fixed << std::setprecision(2) << _thermostatStatus->TemperatureCelsius;
    std::string outTemperatureString = temperatureStream.str();

    std::stringstream humidityStream;
    humidityStream << std::fixed << std::setprecision(2) << _thermostatStatus->Humidity;
    std::string outHumidityString = humidityStream.str();

    jsonDocument["TemperatureCelsius"] = outTemperatureString;
    jsonDocument["HumidityPercentage"] = outHumidityString;

    jsonDocument["SetpointTemperatureCelsius"] = _thermostatStatus->SetpointTemperatureCelsius;

    jsonDocument["CurrentSeconds"] = _thermostatStatus->CurrentSeconds;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}
