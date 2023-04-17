#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_FILAMENT_DRYER_STATUS_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_FILAMENT_DRYER_STATUS_H

#define SECONDS 1000

#include "ThermostatSettings.h"

class ThermostatStatus
{
public:
    ThermostatSettings Settings = *new ThermostatSettings();

   long long CurrentSeconds = 0;

    double TemperatureCelsius = 0.0;
    double SetpointTemperatureCelsius = 0.0;

    double Humidity = 0.0;
    double SetpointHumidity = 0.0;

    double PidValue = 0.0;

    long LoopCounter = 0;
};


#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_FILAMENT_DRYER_STATUS_H
