#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_SETTINGS_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_SETTINGS_H

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

    //Define the aggressive tuning parameters, which are used when we're not close to the setpoint
    double AggressiveKp = 0.0;
    double AggressiveKi = 3.0;
    double AggressiveKd = 0.2;

    //Define the conservative tuning parameters for when we get close to the setpoint
    double ConservativeKp = 0.0;      //
    double ConservativeKi = 3.0;      //
    double ConservativeKd = 0.2;        // Ki / 6

    double SwitchToConservativeMargin = 2.0;         // Value in process variable units (Temperature) to consider being
    // "Close to the setpoint"      Example: If setpoint is 50, and this is 5.0 PID will switch to conservative values
    // when temperature passes 45.0 when rising until 55.0, and the inverse when falling (If PV happens to get outside of this range on rise.)
};


#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_CURRENT_SETTINGS_H
