#include "Sht3xHandler.h"
#include "../models/PinDefinitions.h"

Sht3xHandler::Sht3xHandler(ThermostatStatus* thermostatStatus)
{
    _thermostatStatus = thermostatStatus;

	Wire.begin(PIN_SDA, PIN_SCL);
}

void Sht3xHandler::loop()
{
    _thermostatStatus->CurrentSeconds = millis() / SECONDS;

   long millisSinceLastRequest = millis() - _lastSht30RequestSentMillis;

    if (millisSinceLastRequest > 500)
    {
        _lastSht30RequestSentMillis = millis();                 // Reset time since last message

        if (!_thermostatStatus->Settings.UseMockTemperatureSensorData)
        {
            int shtResponse = GetDataFromShtBoard();

            SetCurrentValues(shtResponse);
        }
        else
        {
            _thermostatStatus->TemperatureCelsius = 1.23;
            _thermostatStatus->Humidity = 4.56;
        }
    }
}

void Sht3xHandler::SetCurrentValues(int shtResponse)
{
    if (shtResponse == 0)
    {
        _thermostatStatus->TemperatureCelsius = TemperatureCelsius;
        _thermostatStatus->Humidity = Humidity;
    }
    else
    {
        //Serial.println("ERROR: Sht30 isn't connected properly");
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

    //delay(10);

    // Request 6 bytes of data
    Wire.requestFrom(_address, 6);

    // Read 6 bytes of data
    // TemperatureCelsius msb, TemperatureCelsius lsb, TemperatureCelsius crc, Humidity msb, Humidity lsb, Humidity crc
    for (unsigned int & i : data)
    {
        i = Wire.read();
    }

    //delay(10);

    if (Wire.available() != 0)
        return 2;

    // Convert the data
    TemperatureCelsius = (float)((((data[0] * 256.0) + data[1]) * 175) / 65535.0) - 45;
    TemperatureFahrenheit = (float)(TemperatureCelsius * 1.8) + 32;
    Humidity = (float)((((data[3] * 256.0) + data[4]) * 100) / 65535.0);

    return 0;
}