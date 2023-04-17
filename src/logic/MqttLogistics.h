#ifndef MQTT_FILAMENT_DRYER_WT32_ETH01_MQTT_LOGISTICS_H
#define MQTT_FILAMENT_DRYER_WT32_ETH01_MQTT_LOGISTICS_H

#include <WiFiClient.h>
#include <PubSubClient.h>
#include "models/ThermostatStatus/ThermostatStatus.h"

class MqttLogistics
{
public:
    explicit MqttLogistics(ThermostatStatus *thermostatStatus, WiFiClient *client);

    void onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length);

    void reconnectMqttIfNotConnected();

    void publish(const char *topic, const char *payload);

    void loopClient();

    std::string getLastCommand();

private:
    ThermostatStatus *_thermostatStatus;
    WiFiClient *_ethernetClient;
    PubSubClient *_mqttClient;

    std::string _lastCommand;

    void setThermostatTemperatureCelsiusSetpoint(String commandString);

    static String getIncomingPayloadAsString(uint8_t *string, unsigned int payloadLength);

    static String getIncomingTopicAsString(char *topic);

    boolean isNumeric(String str);

    void updateHomeAssistantWithNewValues();

    String GetTimestamp();

    void setThermostatHumiditySetpoint(String str);
};

#endif //MQTT_FILAMENT_DRYER_WT32_ETH01_MQTT_LOGISTICS_H
