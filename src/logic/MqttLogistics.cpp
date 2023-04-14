#include <ETH.h>
#include "secrets/SECRETS.h"
#include "MqttLogistics.h"
#include "ConvertersToString.h"

MqttLogistics::MqttLogistics(CurrentThermostatStatus *currentThermostatStatus,
                             WiFiClient *client)
{
    _ethernetClient = client;
    _currentThermostatStatus = currentThermostatStatus;

    _mqttClient = new PubSubClient();

    _mqttClient->setClient(*_ethernetClient);
    _mqttClient->setServer(SECRETS::MQTT_SERVER, 1883);
    _mqttClient->setBufferSize(512);

    _mqttClient->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->onMqttMessageReceived(topic, payload, length); });
}

void MqttLogistics::onMqttMessageReceived(char* topic, uint8_t* payload, unsigned int length)
{
    // A lot of the arduino string functions are really useful, so we'll use it here
    String topicStr = getIncomingTopicAsString(topic);
    String payloadStr = getIncomingPayloadAsString(payload, length);

    if (_currentThermostatStatus->CurrentSettings.DebugModeOn)
    {
        String topicMessage = String("Topic was: ") + topicStr;
        String payloadMessage = String("Payload was: ") + payloadStr;

        Serial.println(topicMessage);
        Serial.println(payloadMessage);

        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, topicMessage.c_str(), false);
        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, payloadMessage.c_str(), false);
    }

    // Handle getInfoAllNodes request
    if (payloadStr.equalsIgnoreCase(F("getInfoAllNodes")))
    {
        _lastCommand = payloadStr.c_str();

        String messageToSend = String(SECRETS::MQTT_ID) + " is up at IPv4: " + ETH.localIP().toString();
        MqttLogistics::_mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, messageToSend.c_str());
    }

    // If message isn't coming in on the commands topic, we don't care
    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_SET_MODE_INCOMING))
    {
        String modeUpdateConfirmation = "Updating mode: " + payloadStr;
        _mqttClient->publish(SECRETS::TOPIC_DEBUG_OUT, modeUpdateConfirmation.c_str(), false);
        setThermostatMode(payloadStr);
        updateHomeAssistantWithNewValues();
    }

    if (topicStr.equalsIgnoreCase(SECRETS::TOPIC_SET_TEMPERATURE_INCOMING))
    {
        // Change thermostat setpoint
        if (isNumeric(payloadStr))
        {
            Serial.println("Saw command to set thermostat setpoint! Value: " + payloadStr);

            setThermostatSetpoint(payloadStr);

            updateHomeAssistantWithNewValues();
        }
    }
}

void MqttLogistics::setThermostatSetpoint(String commandString)
{
    double setpointFloat = std::stod(commandString.c_str());

    _currentThermostatStatus->CurrentSetpoint = setpointFloat;
}

void MqttLogistics::setThermostatMode(const String &payloadStr)
{
    _lastCommand = payloadStr.c_str();

    if (payloadStr.equalsIgnoreCase("cool"))
    {
        _currentThermostatStatus->ThermostatMode = ModeCooling;
        _currentThermostatStatus->FanMode = FanOnAutomatically;
    }
    else if (payloadStr.equalsIgnoreCase("MAINTAIN"))
    {
        _currentThermostatStatus->ThermostatMode = ModeMaintainingRange;
    }
    else if (payloadStr.equalsIgnoreCase("heat"))
    {
        _currentThermostatStatus->ThermostatMode = ModeHeating;
        _currentThermostatStatus->FanMode = FanOnAutomatically;
    }
    else if (payloadStr.equalsIgnoreCase("fan_only"))
    {
        _currentThermostatStatus->FanMode = FanAlwaysOn;
    }
    else if (payloadStr.equalsIgnoreCase("OFF"))
    {
        _currentThermostatStatus->ThermostatMode = ModeOff;
        _currentThermostatStatus->FanMode = FanOnAutomatically;
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
        String macTrimmed = ETH.macAddress().substring(9, 11) + ETH.macAddress().substring(12, 14) + ETH.macAddress().substring(15, 17);

        Serial.print("Last 6 of mac: ");
        Serial.println(macTrimmed);

        Serial.println();

        String mqttId = "WT32_ETH01_HVAC_" + macTrimmed;

        // Attempt to connect
        if (_mqttClient->connect(mqttId.c_str(), SECRETS::MQTT_USER, SECRETS::MQTT_PASSWORD))
        {
            Serial.println("...connected");

            // Once connected, publish an announcement...
            String data = "Hello from " + mqttId + " at IPv4: " + ETH.localIP().toString() + " ETH Mac addr: " + ETH.macAddress();
            _mqttClient->publish(SECRETS::TOPIC_GET_INFO_ALL, data.c_str());

            Serial.print("Device is up: ");
            Serial.println(data);

            // ... and resubscribe
            _mqttClient->subscribe(SECRETS::TOPIC_SET_TEMPERATURE_INCOMING);
            _mqttClient->subscribe(SECRETS::TOPIC_SET_MODE_INCOMING);

            _mqttClient->subscribe(SECRETS::TOPIC_GET_INFO_ALL);

            _mqttClient->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, "71.00");
            _mqttClient->publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, "off");
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

    Serial.print("Payload is [");
    Serial.print(payloadStr);
    Serial.print("] ");
    Serial.println();

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

    Serial.print("Topic is [");
    Serial.print(topicStr);
    Serial.print("] ");
    Serial.println();

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
    char setpointBuffer[7];

    String currentMode =  ConvertersToString::getThermostatModeAsString(_currentThermostatStatus->ThermostatMode);

    dtostrf(_currentThermostatStatus->CurrentSetpoint, 5, 2, setpointBuffer);

    _mqttClient->publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
    _mqttClient->publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, currentMode.c_str());
}
