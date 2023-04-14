
#include "ThermostatStateMachine.h"
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "secrets/SECRETS.h"

ThermostatStateMachine::ThermostatStateMachine(CurrentThermostatStatus *currentThermostatStatus,
                                               HvacController *hvacController,
                                               MqttLogistics *mqttLogistics)
{
    _currentThermostatStatus = currentThermostatStatus;
    _hvacController = hvacController;
    _mqttLogistics = mqttLogistics;
}

void ThermostatStateMachine::loop()
{
    _fanState = _currentThermostatStatus->FanMode;
    _thermostatModeState = _currentThermostatStatus->ThermostatMode;

    checkTemperature();

    switch (_observedTemperatureState)
    {
        case ObservedTempAboveSetpointMargin:
            observedTempAboveSetpointMargin();
            break;

        case ObservedTempBelowSetpointMargin:
            observedTempBelowSetpointMargin();
            break;

        // This is basically just here to get a warning if we ever add more states
        case ObservedTempUninitialized:
            break;
    }
}

void ThermostatStateMachine::observedTempAboveSetpointMargin()
{
    if (_observedTemperatureState != _lastObservedTemperatureState ||
        _fanState != _lastFanState ||
        _thermostatModeState != _lastThermostatModeState)
    {
        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_STATE_MACHINE,
    "State machine transition firing: Inside Setpoint Margin -> Above Setpoint Margin");

        switch (_currentThermostatStatus->ThermostatMode)
        {
            case ModeOff:
            case ModeHeating:
                _hvacController->allOffFanModeRespected();
                break;

            case ModeCooling:
                _hvacController->startCooling();
                break;

            // Just so we get a warning if we add more states
            case ModeMaintainingRange:
            case ModeUninitialized:
                break;
        }

        _lastObservedTemperatureState = _observedTemperatureState;
        _lastFanState = _fanState;
        _lastThermostatModeState = _thermostatModeState;
    }

    // Perform state tasks here



    if (_observedTemperatureState != _lastObservedTemperatureState)
    {
        // Transitioning: Above Setpoint Margin -> Inside Setpoint Margin

        _lastObservedTemperatureState = _observedTemperatureState;
    }
}

void ThermostatStateMachine::observedTempBelowSetpointMargin()
{
    if (_observedTemperatureState != _lastObservedTemperatureState ||
        _fanState != _lastFanState ||
        _thermostatModeState != _lastThermostatModeState)
    {
        _mqttLogistics->publish(SECRETS::TOPIC_DEBUG_STATE_MACHINE,
    "State machine transition firing: Inside Setpoint Margin -> Below Setpoint Margin");

        switch (_currentThermostatStatus->ThermostatMode)
        {
            case ModeOff:
            case ModeCooling:
                _hvacController->allOffFanModeRespected();
                break;

            case ModeHeating:
                _hvacController->startHeating();
                break;

            // These are just so we get a warning if we add more states
            case ModeMaintainingRange:
            case ModeUninitialized:
                break;
        }

        _lastObservedTemperatureState = _observedTemperatureState;
        _lastFanState = _fanState;
        _lastThermostatModeState = _thermostatModeState;
    }

    // Perform state tasks here



    if (_observedTemperatureState != _lastObservedTemperatureState)
    {
        // Transitioning: Below Setpoint Margin -> Inside Setpoint Margin

        _lastObservedTemperatureState = _observedTemperatureState;
    }
}

void ThermostatStateMachine::checkTemperature()
{
    float currentTemperature = _currentThermostatStatus->CurrentTemperatureFahrenheit;
    float setPoint = _currentThermostatStatus->CurrentSetpoint;
    float margin = _currentThermostatStatus->CurrentSettings.MarginFahrenheit;

    float setpointLimitTop = setPoint + margin;
    float setpointLimitBottom = setPoint - margin;

    if (currentTemperature >= setpointLimitTop)
    {
        // At or above high limit
        _observedTemperatureState = ObservedTempAboveSetpointMargin;
    }
    else if (currentTemperature <= setpointLimitBottom)
    {
        // At or below low limit
        _observedTemperatureState = ObservedTempBelowSetpointMargin;
    }
}
