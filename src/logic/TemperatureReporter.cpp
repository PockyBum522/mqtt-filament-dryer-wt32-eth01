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
    unsigned long secondsSinceLastReportSent = TemperatureReporter::_thermostatStatus->CurrentSeconds - _lastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > TemperatureReporter::_thermostatStatus->Settings.TemperatureReportMessageSendIntervalSeconds)
    {
        _lastPeripheralOutMessageSentSeconds = _thermostatStatus->CurrentSeconds;                 // Reset time since last message

        std::string outputJson = TemperatureReporter::SerializeReport();

        _mqttLogistics->publish(SECRETS::TOPIC_PERIPHERAL_OUT, outputJson.c_str());

        char temperatureBuffer[7];
        char setpointBuffer[7];

        dtostrf(_thermostatStatus->TemperatureCelsius, 5, 2, temperatureBuffer);
        dtostrf(_thermostatStatus->Setpoint, 5, 2, setpointBuffer);

        _mqttLogistics->publish(SECRETS::TOPIC_JUST_TEMPERATURE_PERIPHERAL_OUT, temperatureBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);

        if (TemperatureReporter::_thermostatStatus->Settings.DebugModeOn)
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
    humidityStream << std::fixed << std::setprecision(2) << _thermostatStatus->CurrentHumidity;
    std::string outHumidityString = humidityStream.str();

    jsonDocument["TemperatureFahrenheit"] = outTemperatureString;
    jsonDocument["HumidityPercentage"] = outHumidityString;

    jsonDocument["Setpoint"] = _thermostatStatus->Setpoint;

    jsonDocument["CurrentSeconds"] = _thermostatStatus->CurrentSeconds;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}
