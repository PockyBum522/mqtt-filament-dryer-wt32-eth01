
#ifndef MQTT_THERMOSTAT_WT32_ETH01_HVAC_CONTROLLER_H
#define MQTT_THERMOSTAT_WT32_ETH01_HVAC_CONTROLLER_H


#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "MqttLogistics.h"

class MqttLogistics;

class HvacController
{
public:
    explicit HvacController(CurrentThermostatStatus *currentThermostatStatus, MqttLogistics *mqttLogistics);

    void loopForQueuedCompressorHandling();

    void allOff();

    void startCooling();
    void startHeating();

    void turnOffAllButFan();
    void allOffFanModeRespected();

private:
    CurrentThermostatStatus *_currentThermostatStatus;

    void setCompressorState(bool newState);
    void setReverserValveState(bool newState);
    void setFanState(bool newState);

    MqttLogistics *_mqttLogistics;
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_HVAC_CONTROLLER_H
