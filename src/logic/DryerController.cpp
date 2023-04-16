#include <esp32-hal-gpio.h>
#include <ArduinoJson.h>
#include "DryerController.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"

void DryerController::loop()
{
    turnOnFanIfTimeoutNotElapsed();

    if (millis() > _lastHeaterOnAtMillis &&
        millis() < _nextHeaterOffAtMillis)
    {
        turnOnHeater();
    }

    if (millis() > _nextHeaterOffAtMillis)
    {
        turnOffHeater();
    }
}

void DryerController::turnOnFanIfTimeoutNotElapsed()
{
    long long currentMillis = millis();

    if (currentMillis < _lastHeaterOnAtMillis + _fanOffAfterMillisTimeout)
    {
        turnOnFan();
    }
    else
    {
        turnOffFan();
    }

}

void DryerController::setHeaterDutyCycle(double dutyCycle)
{
    bool currentlyBeyondHeaterOffMillis = millis() > _nextHeaterCanTurnBackOnAtMillis;

    if (!currentlyBeyondHeaterOffMillis) return;

    // Otherwise, set up the heater on millis to take up some percentage of the full duty cycle span:
    double dutyCycleTimeDivisions = _thermostatStatus->Settings.HeaterDutyCycleTimeMillis / 320.0;

    _nextHeaterOffAtMillis = (long long)(millis() + (long long)(dutyCycle * dutyCycleTimeDivisions)); // See how the max PID value (255) splits into our duty cycle time so we know what 100% (ish) looks like

    _nextHeaterCanTurnBackOnAtMillis = millis() + _dutyCycleSpanMillis;

    outputDebugValues(dutyCycle);
}

void DryerController::turnOffFan()
{
    digitalWrite(PIN_RELAY_02, 0);
}

void DryerController::turnOnFan()
{
    digitalWrite(PIN_RELAY_02, 1);
}

void DryerController::turnOnHeater()
{
    _lastHeaterOnAtMillis = millis();

    digitalWrite(PIN_RELAY_01, 1);
}

void DryerController::turnOffHeater()
{
    digitalWrite(PIN_RELAY_01, 0);
}

void DryerController::outputDebugValues(double dutyCycle)
{
    String currentMillisMessage = "Current millis(): ";
    currentMillisMessage += millis();

    String currentSetpointMessage = "Current setpoint: ";
    currentSetpointMessage += _thermostatStatus->Setpoint;

    String currentTemperatureMessage = "Current temp (c): ";
    currentTemperatureMessage += _thermostatStatus->TemperatureCelsius;

    String rawPidMessage = "Raw PID value: ";
    rawPidMessage += dutyCycle;


    String outputJson = "";

    StaticJsonDocument<656> jsonDocument;

    jsonDocument["currentMillisMessage"] = currentMillisMessage;
    jsonDocument["currentTemperatureMessage"] = currentTemperatureMessage;
    jsonDocument["currentSetpointMessage"] = currentSetpointMessage;
    //jsonDocument["HeaterDutyCycleTimeMillis"] = _thermostatStatus->Settings.HeaterDutyCycleTimeMillis;
    jsonDocument["rawPidMessage"] = rawPidMessage;
    jsonDocument["heaterOnAtMessage"] = _lastHeaterOnAtMillis;
    jsonDocument["heaterNextOffMessage"] = _nextHeaterOffAtMillis;
    jsonDocument["heaterPermittedBackOnMessage"] = _nextHeaterCanTurnBackOnAtMillis;
    //jsonDocument["PIN_RELAY_02"] = !digitalRead(PIN_RELAY_02);
    //jsonDocument["LastLoopRunTime"] = LastLoopRunTime;
    jsonDocument["LoopCounterSinceLastOutput"] = _thermostatStatus->LoopCounter;
    jsonDocument["_lastHeaterOnAtMillis"] = _lastHeaterOnAtMillis;
    jsonDocument["_lastHeaterOnAtMillis + _fanOffAfterMillisTimeout"] = _lastHeaterOnAtMillis + _fanOffAfterMillisTimeout;

    _thermostatStatus->LoopCounter = 0;

    serializeJson(jsonDocument, outputJson);

    _mqttLogistics->publish(SECRETS::TOPIC_CONTROLLER_DEBUG_OUT, outputJson.c_str());
}
