#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H

#include <Arduino.h>

class ThermostatSettings
{
public:
    const String DeviceName = "Filament_Dryer_01";

    const int MaximumPermittedTemperatureCelsius = 200;

    const int DebugMessageSendIntervalSeconds = 20;
    const int TemperatureReportMessageSendIntervalSeconds = 20;

    const long HeaterDutyCycleTimeMillis = 7000;

    const bool DebugModeOn = false;
    const bool UseMockTemperatureSensorData = false;

    const int FanTimeoutSeconds = 300;
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
