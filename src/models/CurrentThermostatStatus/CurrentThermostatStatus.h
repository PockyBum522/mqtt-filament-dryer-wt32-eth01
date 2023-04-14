#ifndef MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
#define MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H

#define SECONDS 1000

#include "models/StateEnums/FanStates.h"
#include "models/StateEnums/ObservedTemperatureStates.h"
#include "models/StateEnums/ThermostatModeStates.h"

#include "CurrentThermostatSettings.h"

class CurrentThermostatStatus
{
public:
    ThermostatModeStates ThermostatMode = ModeUninitialized;
    FanStates FanMode = FanUninitialized;

    CurrentThermostatSettings CurrentSettings = *new CurrentThermostatSettings();

    unsigned long CurrentSeconds = 0;
    unsigned long LastCompressorOffAtSeconds = 0;                          // This gets set to CurrentSeconds when compressor turns off

    bool CompressorQueuedToTurnOn = false;

    double CurrentTemperatureFahrenheit = 0.0;
    double CurrentHumidity = 0.0;
    double CurrentSetpoint = 0.0; // Default to something comfortable in case of brownout

};


#endif //MQTT_THERMOSTAT_WT32_ETH01_CURRENT_THERMOSTAT_STATUS_H
