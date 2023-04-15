# mqtt-filament-dryer-wt32-eth01
Replacing the brains of a filament dryer to make it network enabled

Parts:
* WT32-ETH01 ESP32/Ethernet combination board
* SHT30 I2c Temperature Sensor
* https://www.amazon.com/gp/product/B09686Z5R3/

# Status/Features

* Basic control and reporting to and from home assistant over MQTT: Not yet

# Wiring

## SHT30

| SHT30 PIN | ESP32 Pin | WT32_ETH01 Pin Silkscreen Label |
|-----------|-----------|---------------------------------|
| Ground    | Ground    | GND                             |
| VIN       | 3.3v      | 3v3                             |
| SCL       | IO 17     | TXD                             |
| SDA       | IO 32     | CFG                             |

## Relays

| Relay Preprocessor Directive Name | ESP32 GPIO | WT32_ETH01 Pin Silkscreen Label |
|-----------------------------------|------------|---------------------------------|
| PIN_FAN                           | GPIO 02    | IO2                             |
| PIN_HEATER                        | GPIO 04    | IO4                             |
| PIN_NOT_CONNECTED                 | GPIO 33    | 485_EN                          |
| PIN_NOT_CONNECTED                 | GPIO 14    | IO14                            |
| PIN_NOT_CONNECTED                 | GPIO 15    | IO15                            |
| PIN_NOT_CONNECTED                 | GPIO 05    | RXD                             |
