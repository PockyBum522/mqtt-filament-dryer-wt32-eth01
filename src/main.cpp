#include <Arduino.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <sstream>
#include <ETH.h>
#include <AsyncElegantOTA.h>
#include "models/CurrentThermostatStatus/CurrentThermostatStatus.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"
#include "logic/MqttLogistics.h"
#include "logic/Sht3xHandler.h"
#include "logic/TemperatureReporter.h"
#include "logic/DebugMessageSender.h"
#include "logic/ThermostatStateMachine.h"
#include "logic/ConvertersToString.h"

#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

bool initialSettingsSet = false;

AsyncWebServer webServer(80);
WiFiClient ethernetClient;

CurrentThermostatStatus currentStatus = *new CurrentThermostatStatus();

MqttLogistics mqttLogistics = *new MqttLogistics(&currentStatus, &ethernetClient);

auto temperatureReportSender = *new TemperatureReporter(&currentStatus, &mqttLogistics);
auto debugMqttSender = *new DebugMessageSender(&currentStatus, &mqttLogistics);
auto temperatureSensorHandler = *new Sht3xHandler(&currentStatus);
auto hvacController = *new HvacController(&currentStatus, &mqttLogistics);
auto thermostatStateMachine = *new ThermostatStateMachine(&currentStatus, &hvacController, &mqttLogistics);

void setInitialSettingsAfterDelay();

void setup()
{
    pinMode(PIN_RELAY_01, OUTPUT);
    pinMode(PIN_RELAY_02, OUTPUT);
    pinMode(PIN_RELAY_03, OUTPUT);
    pinMode(PIN_RELAY_04, OUTPUT);
    pinMode(PIN_RELAY_05, OUTPUT);
    pinMode(PIN_RELAY_06, OUTPUT);

    // Turn all pins off
    digitalWrite(PIN_RELAY_01, HIGH);
    digitalWrite(PIN_RELAY_02, HIGH);
    digitalWrite(PIN_RELAY_03, HIGH);
    digitalWrite(PIN_RELAY_04, HIGH);
    digitalWrite(PIN_RELAY_05, HIGH);
    digitalWrite(PIN_RELAY_06, HIGH);

    Serial.begin(115200);

    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER);

    // Static IP, leave without this line to get IP via DHCP
    // ETH.config(myIP, myGW, mySN, myDNS);

    webServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/plain", "Hi! I am WT32_ETH01_Thermostat.");
    });

    AsyncElegantOTA.begin(&webServer);    // Start ElegantOTA
    webServer.begin();

    Serial.println();
    Serial.println("V03_HTTP server started with MAC: " + ETH.macAddress() + ", at IPv4: " + ETH.localIP().toString());
    Serial.println();
}

void loop()
{
    temperatureSensorHandler.UpdateCurrentStatus();

    mqttLogistics.reconnectMqttIfNotConnected();
    mqttLogistics.loopClient();

    thermostatStateMachine.loop();
    hvacController.loopForQueuedCompressorHandling();

    temperatureReportSender.SendTemperatureReportEveryTimeout();
    debugMqttSender.SendMqttDebugMessagesEveryTimeout();

    setInitialSettingsAfterDelay(); // Fires once after 30 seconds to set sane settings after brownouts
}

void setInitialSettingsAfterDelay()
{
    if (!(millis() > 30000 && !initialSettingsSet))
        return;

    initialSettingsSet = true;

    currentStatus.ThermostatMode = ModeCooling;
    currentStatus.FanMode = FanOnAutomatically;
    currentStatus.CurrentSetpoint = 71.0;

    // Update HA with new values
    char setpointBuffer[7];

    String currentMode =  ConvertersToString::getThermostatModeAsString(currentStatus.ThermostatMode);

    dtostrf(currentStatus.CurrentSetpoint, 5, 2, setpointBuffer);

    mqttLogistics.publish(SECRETS::TOPIC_JUST_SETPOINT_PERIPHERAL_OUT, setpointBuffer);
    mqttLogistics.publish(SECRETS::TOPIC_JUST_MODE_PERIPHERAL_OUT, currentMode.c_str());
}