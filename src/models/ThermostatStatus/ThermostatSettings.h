//
// Created by David on 4/12/2023.
//

#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H


class ThermostatSettings
{
public:
    const int DebugMessageSendIntervalSeconds = 10;
    const int TemperatureReportMessageSendIntervalSeconds = 10;

    const bool DebugModeOn = false;
    const bool UseMockTemperatureSensorData = false;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_SETTINGS_H
