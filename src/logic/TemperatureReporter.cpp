#include <sstream>
#include <iomanip>
#include "TemperatureReporter.h"
#include "MqttLogistics.h"
#include "secrets/SECRETS.h"
#include "ConvertersToString.h"

TemperatureReporter::TemperatureReporter(CurrentThermostatStatus* currentThermostatStatus,
                                         MqttLogistics* mqttLogistics)
{
    _currentThermostatStatus = currentThermostatStatus;
    _mqttLogistics = mqttLogistics;
}

void TemperatureReporter::SendTemperatureReportEveryTimeout()
{
    unsigned long secondsSinceLastReportSent = TemperatureReporter::_currentThermostatStatus->CurrentSeconds - _lastPeripheralOutMessageSentSeconds;

    if (secondsSinceLastReportSent > TemperatureReporter::_currentThermostatStatus->CurrentSettings.TemperatureReportMessageSendIntervalSeconds)
    {
        _lastPeripheralOutMessageSentSeconds = _currentThermostatStatus->CurrentSeconds;                 // Reset time since last message

        std::string outputJson = TemperatureReporter::SerializeReport();

        _mqttLogistics->publish(SECRETS::TOPIC_PERIPHERAL_OUT, outputJson.c_str());

        char temperatureBuffer[7];
        char setpointBuffer[7];

        String currentMode = ConvertersToString::getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);

        dtostrf(_currentThermostatStatus->CurrentTemperatureFahrenheit, 5, 2, temperatureBuffer);
        dtostrf(_currentThermostatStatus->CurrentSetpoint, 5, 2, setpointBuffer);

        _mqttLogistics->publish(SECRETS::TOPIC_JUST_TEMPERATURE_PERIPHERAL_OUT, temperatureBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
        _mqttLogistics->publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, currentMode.c_str());

        if (TemperatureReporter::_currentThermostatStatus->CurrentSettings.DebugModeOn)
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
    temperatureStream << std::fixed << std::setprecision(2) << _currentThermostatStatus->CurrentTemperatureFahrenheit;
    std::string outTemperatureString = temperatureStream.str();

    std::stringstream humidityStream;
    humidityStream << std::fixed << std::setprecision(2) << _currentThermostatStatus->CurrentHumidity;
    std::string outHumidityString = humidityStream.str();

    jsonDocument["TemperatureFahrenheit"] = outTemperatureString;
    jsonDocument["HumidityPercentage"] = outHumidityString;

    jsonDocument["Setpoint"] = _currentThermostatStatus->CurrentSetpoint;

    jsonDocument["ThermostatMode"] = ConvertersToString::getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);
    jsonDocument["FanMode"] = ConvertersToString::getFanModeAsString(_currentThermostatStatus->FanMode);

    jsonDocument["CompressorDelayIfNegative"] = (long)((long)_currentThermostatStatus->CurrentSeconds - (long)_currentThermostatStatus->LastCompressorOffAtSeconds) - (long)_currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;
    jsonDocument["CurrentSeconds"] = _currentThermostatStatus->CurrentSeconds;

    std::string outputJson;

    serializeJson(jsonDocument, outputJson);

    return outputJson;
}
