#ifndef MQTT_THERMOSTAT_WT32_ETH01_PIN_DEFINITIONS_H
#define MQTT_THERMOSTAT_WT32_ETH01_PIN_DEFINITIONS_H

// Don't use 12 for anything, there be dragons. (Wouldn't stay low even with 110k resistor, even if keep low, other issues arise due to not being a safe GPIO)

// Relay output number  // WT32_ETH01 Screen-printed Label
#define PIN_RELAY_01 2  // IO2
#define PIN_RELAY_02 4  // IO4
#define PIN_RELAY_03 33 // 485_EN
#define PIN_RELAY_04 14 // IO14
#define PIN_RELAY_05 15 // IO15
#define PIN_RELAY_06 5  // RXD
// These are left in for modifying this code if we need to use 8ch relay board on another project
//#define PIN_RELAY_07 17  // TXD
//#define PIN_RELAY_08 32  // CFG

#define PIN_SCL 17 // TXD (Blue at 4 pin header)
#define PIN_SDA 32 // CFG (Green at 4 pin header)

#define STATE_ON false
#define STATE_OFF true

// Now assign what relay does what HVAC function
const int PIN_GREEN_FAN_CALL = PIN_RELAY_01;
const int PIN_YELLOW_COMPRESSOR_CALL = PIN_RELAY_02;
const int PIN_ORANGE_REVERSER_VALVE = PIN_RELAY_03;
const int PIN_WHITE_AUX_HEAT = PIN_RELAY_04;

#endif //MQTT_THERMOSTAT_WT32_ETH01_PIN_DEFINITIONS_H
