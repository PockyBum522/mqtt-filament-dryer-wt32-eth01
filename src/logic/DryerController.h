#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_DRYERCONTROLLER_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_DRYERCONTROLLER_H


#include "models/ThermostatStatus/ThermostatStatus.h"
#include "MqttLogistics.h"

class DryerController
{
public:
    explicit DryerController(ThermostatStatus* thermostatStatus,
                    MqttLogistics* mqttLogistics)
    {
        _thermostatStatus = thermostatStatus;
        _mqttLogistics = mqttLogistics;
    }

    void loop();

    void turnOnFan();
    void turnOffFan();

    void setHeaterDutyCycle(double dutyCycle);
    void turnOnHeater();
    void turnOffHeater();

    long LastLoopRunTime = 0;

private:
    ThermostatStatus *_thermostatStatus;
    MqttLogistics *_mqttLogistics;

    int _dutyCycleSpanMillis = 5000;

    long long _lastHeaterOnAtMillis = 0;
    long long _nextHeaterCanTurnBackOnAtMillis = 0;
    long long _nextHeaterOffAtMillis = 0;

    long long _fanOffAfterMillisTimeout = _thermostatStatus->Settings.FanTimeoutSeconds * 1000; // Five minutes

    void turnOnFanIfTimeoutNotElapsed();

    void outputDebugValues(double dutyCycle);
};


#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_DRYERCONTROLLER_H
