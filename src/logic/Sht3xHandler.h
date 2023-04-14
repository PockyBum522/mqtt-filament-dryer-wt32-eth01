#ifndef MQTT_THERMOSTAT_WT32_ETH01_SHT_3X_HANDLER_H
#define MQTT_THERMOSTAT_WT32_ETH01_SHT_3X_HANDLER_H


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include "Wire.h"
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"

class Sht3xHandler
{
public:
	explicit Sht3xHandler(CurrentThermostatStatus *currentThermostatStatus);

	float TemperatureCelsius = 0;
	float TemperatureFahrenheit = 0;
    float Humidity = 0;

    void UpdateCurrentStatus();

private:
	int _address = 0x45;

    CurrentThermostatStatus *_currentThermostatStatus;

    int GetDataFromShtBoard();

    void SetCurrentStatusOrError(int shtResponse);

    void SetCurrentStatusMockValuesIfEnabled();
};

#endif
