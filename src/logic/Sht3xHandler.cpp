#include "Sht3xHandler.h"
#include "models/PinDefinitions.h"

Sht3xHandler::Sht3xHandler(ThermostatStatus* thermostatStatus)
{
    _thermostatStatus = thermostatStatus;

	Wire.begin(PIN_SDA, PIN_SCL);
}

void Sht3xHandler::UpdateCurrentStatus()
{
    _thermostatStatus->CurrentSeconds = millis() / SECONDS;

    if (!_thermostatStatus->Settings.UseMockTemperatureSensorData)
    {
        int shtResponse = GetDataFromShtBoard();

        SetCurrentStatusOrError(shtResponse);
    }

    SetCurrentStatusMockValuesIfEnabled();
}

void Sht3xHandler::SetCurrentStatusOrError(int shtResponse)
{
    if (shtResponse == 0)
    {
        _thermostatStatus->TemperatureCelsius = TemperatureFahrenheit;
        _thermostatStatus->CurrentHumidity = Humidity;
    }
    else
    {
        //_thermostatStatus-> = ErrorSensorTemperatureProblem;
    }
}

void Sht3xHandler::SetCurrentStatusMockValuesIfEnabled()
{
    if (_thermostatStatus->Settings.UseMockTemperatureSensorData)
    {
        _thermostatStatus->TemperatureCelsius = 1.2;
        _thermostatStatus->CurrentHumidity = 3.4;
    }
}

int Sht3xHandler::GetDataFromShtBoard()
{
    unsigned int data[6];

    // Start I2C Transmission
    Wire.beginTransmission(_address);

    // Send measurement request command
    Wire.write(0x2C);
    Wire.write(0x06);

    // Stop I2C transmission
    if (Wire.endTransmission() != 0)
        return 1;

    delay(500);

    // Request 6 bytes of data
    Wire.requestFrom(_address, 6);

    // Read 6 bytes of data
    // TemperatureCelsius msb, TemperatureCelsius lsb, TemperatureCelsius crc, Humidity msb, Humidity lsb, Humidity crc
    for (unsigned int & i : data)
    {
        i = Wire.read();
    }

    delay(50);

    if (Wire.available() != 0)
        return 2;

    // Convert the data
    TemperatureCelsius = (float)((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
    TemperatureFahrenheit = (float)(TemperatureCelsius * 1.8) + 32;
    Humidity = (float)((((data[3] * 256.0) + data[4]) * 100) / 65535.0);

    return 0;
}