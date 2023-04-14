#include <WString.h>

#ifndef MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H
#define MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H


class SECRETS
{
public:
    static const char *MQTT_SERVER;                         // Broker address

    static const char *MQTT_USER;                                  // Username to connect to your MQTT broker
    static const char *MQTT_PASSWORD;                              // Password to connect to your MQTT broker
    static const char *MQTT_ID;                                    // Name of our device, must be unique

    static const char *TOPIC_PERIPHERAL_OUT;
    static const char *TOPIC_CONTROLLER_COMMANDS;

    static const char *TOPIC_GET_INFO_ALL;
    static const char *TOPIC_DEBUG_OUT;
    static const char *TOPIC_DEBUG_STATE_MACHINE;

    // HVAC Topics
    static const char *TOPIC_SET_TEMPERATURE_INCOMING;

    static const char *WIFI_SSID;
    static const char *WIFI_PSK;
    static const char *TOPIC_JUST_TEMPERATURE_PERIPHERAL_OUT;
    static const char *TOPIC_JUST_SETPOINT_PERIPHERAL_OUT;
    static const char *TOPIC_JUST_ACTIONS_PERIPHERAL_OUT;
    static const char *TOPIC_SET_MODE_INCOMING;
    static const char *TOPIC_JUST_MODE_PERIPHERAL_OUT;
};


#endif //MQTT_THERMOSTAT_WT32_ETH01_SECRETS_H
