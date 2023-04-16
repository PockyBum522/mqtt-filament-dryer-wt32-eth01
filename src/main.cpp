#include <Arduino.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <PID_v1.h>
#include <sstream>
#include <ETH.h>
#include <AsyncElegantOTA.h>
#include "models/ThermostatStatus/ThermostatStatus.h"
#include "models/PinDefinitions.h"
#include "secrets/SECRETS.h"
#include "logic/MqttLogistics.h"
#include "logic/Sht3xHandler.h"
#include "logic/TemperatureReporter.h"
#include "logic/DebugMessageSender.h"
#include "logic/DryerController.h"


#define DEBUG_ETHERNET_WEBSERVER_PORT Serial

bool initialSettingsSet = false;

AsyncWebServer webServer(80);
WiFiClient ethernetClient;

ThermostatStatus currentStatus = *new ThermostatStatus();

MqttLogistics mqttLogistics = *new MqttLogistics(&currentStatus, &ethernetClient);

auto temperatureReportSender = *new TemperatureReporter(&currentStatus, &mqttLogistics);
auto debugMqttSender = *new DebugMessageSender(&currentStatus, &mqttLogistics);
auto temperatureSensorHandler = *new Sht3xHandler(&currentStatus);

auto dryerController = *new DryerController(&currentStatus, &mqttLogistics);

void setInitialSettingsAfterDelay();

void setHeaterDutyCycleWithPid();

void turnOnFan();

void turnOffFan();

//Define Variables we'll be connecting to
double setpoint = 0.0;
double input = 30.0;
double output = 0.0;

//Define the aggressive tuning parameters
double aggressiveKp = 4;
double aggressiveKi = 0.2;
double aggressiveKd = 1;

//Define the conservative tuning parameters
double conservativeKp = 1;
double conservativeKi = 0.05;
double conservativeKd = 0.25;

//Specify the links and initial tuning parameters
PID myPID(&input, &output, &setpoint, conservativeKp, conservativeKi, conservativeKd, DIRECT);

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

    myPID.SetMode(AUTOMATIC);

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
   long startMillis = millis();

    temperatureSensorHandler.loop();

    mqttLogistics.reconnectMqttIfNotConnected();
    mqttLogistics.loopClient();

    dryerController.loop();

    temperatureReportSender.SendTemperatureReportEveryTimeout();
    //debugMqttSender.SendMqttDebugMessagesEveryTimeout();

    setHeaterDutyCycleWithPid();

    currentStatus.LoopCounter++;

    dryerController.LastLoopRunTime = ((long)millis()) - startMillis;
}

void setHeaterDutyCycleWithPid()
{
    input = currentStatus.TemperatureCelsius;
    setpoint = currentStatus.Setpoint;

    double gap = abs(currentStatus.Setpoint - input); //distance away from setpoint

    if(gap < 5)
    {
        //we're close to setpoint, use conservative tuning parameters
        myPID.SetTunings(conservativeKp, conservativeKi, conservativeKd);
    }
    else
    {
        //we're far from setpoint, use aggressive tuning parameters
        myPID.SetTunings(aggressiveKp, aggressiveKi, aggressiveKd);
    }

    bool newPidAvailable = myPID.Compute();

    if (newPidAvailable)
    {
        currentStatus.PidValue = output;

        dryerController.setHeaterDutyCycle(output);
    }
}
