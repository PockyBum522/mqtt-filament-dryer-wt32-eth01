#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_PIN_DEFINITIONS_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_PIN_DEFINITIONS_H

// Don't use 12 for anything, there be dragons. (Wouldn't stay low even with 110k resistor, even if keep low, other issues arise due to not being a safe GPIO)

// Relay output number  // WT32_ETH01 Screen-printed Label
#define PIN_OUTPUT_01 2  // IO2
#define PIN_OUTPUT_02 4  // IO4
#define PIN_OUTPUT_03 33 // 485_EN
#define PIN_OUTPUT_04 14 // IO14
#define PIN_OUTPUT_05 15 // IO15
#define PIN_OUTPUT_06 5  // RXD
// These are left in for modifying this code if we need to use 8ch relay board on another project
//#define PIN_OUTPUT_07 17  // TXD
//#define PIN_OUTPUT_08 32  // CFG

#define PIN_SCL 17 // TXD (Blue at 4 pin header)
#define PIN_SDA 32 // CFG (Green at 4 pin header)

// For some ESP boards these are backwards. If that is the case, switch it easily here
#define STATE_ON 1
#define STATE_OFF 0

const int PIN_HEATER_CONTROL = PIN_OUTPUT_01;
const int PIN_FAN_CONTROL = PIN_OUTPUT_04;
const int PIN_BEEPER_CONTROL = PIN_OUTPUT_02;



#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_PIN_DEFINITIONS_H
