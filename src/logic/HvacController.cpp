#include "Arduino.h"
#include "HvacController.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"

HvacController::HvacController(CurrentThermostatStatus* currentThermostatStatus, MqttLogistics *mqttLogistics)
{
    _currentThermostatStatus = currentThermostatStatus;
    _mqttLogistics = mqttLogistics;
}

void HvacController::loopForQueuedCompressorHandling()
{
    if(!_currentThermostatStatus->CompressorQueuedToTurnOn)
        return;

    long secondsUntilCompressorOn = ((long)_currentThermostatStatus->CurrentSeconds - (long)_currentThermostatStatus->LastCompressorOffAtSeconds) - (long)_currentThermostatStatus->CurrentSettings.CompressorTimeoutSeconds;

    if (secondsUntilCompressorOn > 0)
    {
         _mqttLogistics->publish(SECRETS::TOPIC_JUST_ACTIONS_PERIPHERAL_OUT, "cooling");

        _currentThermostatStatus->CompressorQueuedToTurnOn = false;
        digitalWrite(PIN_YELLOW_COMPRESSOR_CALL, STATE_ON);
    }
}

void HvacController::allOff()
{
    _mqttLogistics->publish(SECRETS::TOPIC_JUST_ACTIONS_PERIPHERAL_OUT, "off");

    setCompressorState(STATE_OFF);
    setFanState(STATE_OFF);
    setReverserValveState(STATE_OFF);
}

void HvacController::allOffFanModeRespected()
{
    setCompressorState(STATE_OFF);
    setReverserValveState(STATE_OFF);

    switch (_currentThermostatStatus->FanMode)
    {
        case FanAlwaysOn:
            turnOffAllButFan();
            break;

        case FanAlwaysOff:
        case FanOnAutomatically:
            allOff();
            break;

        case FanUninitialized:
            break;
    }
}

void HvacController::startCooling()
{
    setReverserValveState(STATE_ON);
    setFanState(STATE_ON);
    setCompressorState(STATE_ON);
}

void HvacController::startHeating()
{
    _mqttLogistics->publish(SECRETS::TOPIC_JUST_ACTIONS_PERIPHERAL_OUT, "heating");

    setReverserValveState(STATE_OFF);
    setFanState(STATE_ON);
    setCompressorState(STATE_ON);
}

void HvacController::turnOffAllButFan()
{
    _mqttLogistics->publish(SECRETS::TOPIC_JUST_ACTIONS_PERIPHERAL_OUT, "idle");

    setReverserValveState(STATE_OFF);
    setCompressorState(STATE_OFF);

    setFanState(STATE_ON);
}

void HvacController::setCompressorState(bool newState)
{
    if (!newState)
    {
        _currentThermostatStatus->CompressorQueuedToTurnOn = true;
        return;
    }

    // Otherwise:
    _currentThermostatStatus->CompressorQueuedToTurnOn = false;

    // If we were on (ESP32 is inverted) and are now turning off, update LastCompressorOffAtSeconds
    if (!digitalRead(PIN_YELLOW_COMPRESSOR_CALL))
        _currentThermostatStatus->LastCompressorOffAtSeconds = _currentThermostatStatus->CurrentSeconds;

    _mqttLogistics->publish(SECRETS::TOPIC_JUST_ACTIONS_PERIPHERAL_OUT, "idle");
    digitalWrite(PIN_YELLOW_COMPRESSOR_CALL, STATE_OFF);
}

void HvacController::setReverserValveState(bool newState)
{
    if (!newState)
        digitalWrite(PIN_ORANGE_REVERSER_VALVE, STATE_ON);
    else
        digitalWrite(PIN_ORANGE_REVERSER_VALVE, STATE_OFF);

}

void HvacController::setFanState(bool newState)
{
    if (!newState)
    {
        digitalWrite(PIN_GREEN_FAN_CALL, STATE_ON);
    }
    else
    {
        digitalWrite(PIN_GREEN_FAN_CALL, STATE_OFF);
    }
}
