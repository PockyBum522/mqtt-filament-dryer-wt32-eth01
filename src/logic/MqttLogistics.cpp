#include <Arduino.h>
#include <ETH.h>
#include <cstdlib>
#include "secrets/SECRETS.h"
#include "MqttLogistics.h"
#include "ConvertersToString.h"
#include "../models/ThermostatStatus/ThermostatStatus.h"
#include "../secrets/SECRETS.h"
#include "../models/PinDefinitions.h"
#include "time.h"

MqttLogistics::MqttLogistics(ThermostatStatus *thermostatStatus,
                             WiFiClient *client)
{
    _ethernetClient = client;
    _thermostatStatus = thermostatStatus;

    _mqttClient = new PubSubClient();

    _mqttClient->setClient(*_ethernetClient);
    _mqttClient->setServer(SECRETS::MQTT_SERVER, 1883);
    _mqttClient->setBufferSize(680);

    _mqttClient->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onMqttMessageReceived(topic, payload, length); });
}

void MqttLogistics::onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    // A lot of the arduino string functions are really useful, so we'll use it here
    String topicStr = getIncomingTopicAsString(topic);
    String payloadStr = getIncomingPayloadAsString(payload, length);

    if (_thermostatStatus->Settings.DebugModeOn)
    {
        Serial.println();
        Serial.println();

        Serial.println("New MQTT Message:");
        Serial.println();
        Serial.print("Topic is [");
        Serial.print(topicStr);
        Serial.print("] ");
        Serial.println();

        Serial.print("Payload is [");
        Serial.print(payloadStr);
        Serial.print("] ");
        Serial.println();

        Serial.println();
        Serial.println();

        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, topicStr.c_str(), false);
        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, payloadStr.c_str(), false);
    }

    // Handle getInfoAllNodes request
    if (payloadStr.equalsIgnoreCase(F("getInfoAllNodes")))
    {
        _lastCommand = payloadStr.c_str();

        String messageToSend = String(SECRETS::MQTT_ID) + " is up at IPv4: " + ETH.localIP().toString();
        MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, messageToSend.c_str());
    }

    // SET TEMPERATURE HANDLER

    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_SET_TEMPERATURE_INCOMING))
    {
        // Change thermostat setpoint
        if (isNumeric(payloadStr))
        {
            Serial.println("Saw command to set thermostat setpoint! Value: " + payloadStr);

            setThermostatTemperatureCelsiusSetpoint(payloadStr);

            updateHomeAssistantWithNewValues();
        }
    }

    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_SET_HUMIDITY_INCOMING))
    {
        // Change thermostat setpoint
        if (isNumeric(payloadStr))
        {
            Serial.println("Saw command to set humidity setpoint! Value: " + payloadStr);

            setThermostatHumiditySetpoint(payloadStr);

            updateHomeAssistantWithNewValues();
        }
    }

    // RELAYS ON/OFF HANDLER

    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_SET_RELAY_INCOMING))
    {
        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, "Message came in on SET_RELAY topic");

        char relayNumber = payloadStr[0];

        if (payloadStr[2] == 'N')
        {
            _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, "Message was to turn relay ON");

            if (relayNumber == '1')
                digitalWrite(PIN_OUTPUT_01, STATE_ON);

            if (relayNumber == '2')
                digitalWrite(PIN_OUTPUT_02, STATE_ON);

            if (relayNumber == '3')
                digitalWrite(PIN_OUTPUT_03, STATE_ON);

            if (relayNumber == '4')
                digitalWrite(PIN_OUTPUT_04, STATE_ON);
        }

        if (payloadStr[2] == 'F')
        {
            _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, "Message was to turn relay OFF");

            if (relayNumber == '1')
                digitalWrite(PIN_OUTPUT_01, STATE_OFF);

            if (relayNumber == '2')
                digitalWrite(PIN_OUTPUT_02, STATE_OFF);

            if (relayNumber == '3')
                digitalWrite(PIN_OUTPUT_03, STATE_OFF);

            if (relayNumber == '4')
                digitalWrite(PIN_OUTPUT_04, STATE_OFF);
        }
    }
}

void MqttLogistics::reconnectMqttIfNotConnected()
{
    // Loop until we're reconnected
    while (!_mqttClient->connected())
    {
        Serial.print("Attempting MQTT connection to ");
        Serial.print(SECRETS::MQTT_SERVER);

        // Get last 6 of mac address without ':'
        String macTrimmed = String("-") + ETH.macAddress().substring(9, 11) + ETH.macAddress().substring(12, 14) + ETH.macAddress().substring(15, 17);

        Serial.print("Last 6 of mac: ");
        Serial.println(macTrimmed);

        Serial.println();

        String mqttId = _thermostatStatus->Settings.DeviceName + macTrimmed;

        // Attempt to connect
        if (_mqttClient->connect(mqttId.c_str(), SECRETS::MQTT_USER, SECRETS::MQTT_PASSWORD))
        {
            Serial.println("...connected");

            String dateTimestamp = GetTimestamp();

            // Once connected, publish an announcement...
            String data = "AT: " + dateTimestamp+ ", " + mqttId + " @ IP: " + ETH.localIP().toString();
            data += " ETH Mac: " + ETH.macAddress();

            _mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, data.c_str());

            Serial.print("Device is up: ");
            Serial.println(data);

            // ... and resubscribe
            _mqttClient->subscribe(SECRETS::TOPIC_GET_INFO_ALL);
            _mqttClient->subscribe(SECRETS::TOPIC_SET_TEMPERATURE_INCOMING);
            _mqttClient->subscribe(SECRETS::TOPIC_SET_RELAY_INCOMING);

            _mqttClient->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, "0.00");
        }
        else
        {
            Serial.print("Attempting MQTT connection to ");
            Serial.print(SECRETS::MQTT_SERVER);

            Serial.print("...failed, rc=");
            Serial.print(MqttLogistics::_mqttClient->state());
            Serial.println(" try again in 5 seconds");

            // Wait 5 seconds before retrying
            delay(5000);
        }
    }
}

void MqttLogistics::publish(const char* topic, const char* payload)
{
    MqttLogistics::_mqttClient->publish(topic, payload);
}

void MqttLogistics::loopClient()
{
    MqttLogistics::_mqttClient->loop();
}

std::string MqttLogistics::getLastCommand()
{
    return _lastCommand;
}

String MqttLogistics::getIncomingPayloadAsString(uint8_t *payload, unsigned int payloadLength)
{
    String payloadStr = "";

    // Convert payload to string
    for (int i = 0; i < payloadLength; i++)
    {
        payloadStr += ((char)payload[i]);
    }

    return payloadStr;
}

String MqttLogistics::getIncomingTopicAsString(char *topic)
{
    String topicStr = "";

    // Convert topic to string
    for (int i = 0; i < strlen(topic); i++)
    {
        topicStr += ((char)topic[i]);
    }

    return topicStr;
}

boolean MqttLogistics::isNumeric(String str) {
    unsigned int stringLength = str.length();

    if (stringLength == 0) {
        return false;
    }

    boolean seenDecimal = false;

    for(unsigned int i = 0; i < stringLength; ++i) {
        if (isDigit(str.charAt(i))) {
            continue;
        }

        if (str.charAt(i) == '.') {
            if (seenDecimal) {
                return false;
            }
            seenDecimal = true;
            continue;
        }
        return false;
    }
    return true;
}

void MqttLogistics::updateHomeAssistantWithNewValues()
{
//    char setpointBuffer[7];
//
//    String currentMode =  ConvertersToString::getThermostatModeAsString(_thermostatStatus->ThermostatMode);
//
//    dtostrf(_thermostatStatus->SetpointTemperatureCelsius, 5, 2, setpointBuffer);
//
//    _mqttClient->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
//    _mqttClient->publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, currentMode.c_str());
}

String MqttLogistics::GetTimestamp()
{
    const char* ntpServer = "pool.ntp.org";
    const long  gmtOffset_sec = 0;
    const int   daylightOffset_sec = 3600;

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;

    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time");
        return "";
    }

    // Fix GMT offset cause I have no idea how using what's above
    timeinfo.tm_hour -= 5;

    if (timeinfo.tm_hour < 0) timeinfo.tm_hour += 24;

    static char timeBuffer[50];

    strftime(timeBuffer, 50, "%Y-%b-%d @ %H:%M:%S", &timeinfo);

    return {timeBuffer};
}

void MqttLogistics::setThermostatTemperatureCelsiusSetpoint(String commandString)
{
    double setpointFloat = std::stod(commandString.c_str());

    if (setpointFloat < _thermostatStatus->Settings.MaximumPermittedTemperatureCelsius &&
        setpointFloat > 0)
    {
        _thermostatStatus->SetpointTemperatureCelsius = setpointFloat;
    }
    else
    {
        _thermostatStatus->SetpointTemperatureCelsius = 0.0;
    }
}

void MqttLogistics::setThermostatHumiditySetpoint(String mqttPayload)
{
    double humiditySetpointDouble = std::stod(mqttPayload.c_str());

    if (humiditySetpointDouble < 100 &&
        humiditySetpointDouble > 0)
    {
        _thermostatStatus->SetpointHumidity = humiditySetpointDouble;
    }
    else
    {
        _thermostatStatus->SetpointHumidity = 50.0;
    }
}
