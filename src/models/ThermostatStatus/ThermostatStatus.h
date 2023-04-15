#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H

#define SECONDS 1000

#include "ThermostatSettings.h"

class ThermostatStatus
{
public:
    ThermostatSettings Settings = *new ThermostatSettings();

    unsigned long CurrentSeconds = 0;

    double TemperatureCelsius = 0.0;
    double CurrentHumidity = 0.0;
    double Setpoint = 0.0;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
