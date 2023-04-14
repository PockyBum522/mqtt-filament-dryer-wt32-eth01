#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H

#define SECONDS 1000

#include "models/StateEnums/FanStates.h"
#include "models/StateEnums/ObservedTemperatureStates.h"
#include "models/StateEnums/ThermostatModeStates.h"

#include "CurrentSettings.h"

class CurrentThermostatStatus
{
public:
    ThermostatModeStates ThermostatMode = ModeUninitialized;
    FanStates FanMode = FanUninitialized;

    CurrentSettings Settings = *new CurrentSettings();

    unsigned long CurrentSeconds = 0;

    double TemperatureCelsius = 0.0;
    double CurrentHumidity = 0.0;
    double Setpoint = 0.0;

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
