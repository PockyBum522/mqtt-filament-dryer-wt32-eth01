#ifndef MQTT_THERMOSTAT_WT32_ETH01_STATE_HANDLER_H
#define MQTT_THERMOSTAT_WT32_ETH01_STATE_HANDLER_H


#include <Arduino.h>
#include "models/PinDefinitions.h"
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "HvacController.h"

class MqttLogistics;

class ThermostatStateMachine
{
public:
    explicit ThermostatStateMachine(CurrentThermostatStatus *currentThermostatStatus, HvacController *hvacController, MqttLogistics *mqttLogistics);

    void observedTempAboveSetpointMargin();
    void observedTempBelowSetpointMargin();

    void loop();

private:
    CurrentThermostatStatus *_currentThermostatStatus;
    HvacController *_hvacController;
    MqttLogistics *_mqttLogistics;

    ObservedTemperatureStates _observedTemperatureState = ObservedTempUninitialized;
    ObservedTemperatureStates _lastObservedTemperatureState = ObservedTempUninitialized;

    FanStates _fanState = FanUninitialized;
    FanStates _lastFanState = FanUninitialized;

    ThermostatModeStates _thermostatModeState = ModeUninitialized;
    ThermostatModeStates _lastThermostatModeState = ModeUninitialized;

    void checkTemperature();
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_STATE_HANDLER_H
