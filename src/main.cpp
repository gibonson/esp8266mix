#include <Arduino.h>
#include <ESP8266Ping.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>      // JSON library for Arduino, used to create and parse JSON objects
#include <ESP8266WebServer.h> // Dedykowany serwer HTTP
ESP8266WebServer server(80);  // Set web server port number to 80

#include "MODULE_OLED_WD.h"
#include "MODULE_OLED.h"

#include "BOARD_LOGGING.h"
#include "BOARD_CONFIG.h"
#include "BOARD_WIFI.h"
#include "BOARD_JSON.h"
#include "BOARD_OTA.h"
#include "BOARD_PUSHOVER.h"
#include "BOARD_WEB_GUI.h" // Web GUI management file

WebGui webGui;

// WatchDog config --------------------------------------------
#ifdef ESP8266_WATCHDOG
String deviceType = "ESP-WatchDog";
String appVersion = "0.00";
#include "APP_WATCHDOG.h"
#endif
// WatchDog config --------------------------------------------

// Satellite config --------------------------------------------
#ifdef ESP8266_SATELLITE
String deviceType = "ESP-Satellite";
String appVersion = "0.00";
#include "APP_SATELLITE.h"
#endif
// Satellite config --------------------------------------------

// AP config --------------------------------------------
const char *accessPointName = "ESP-Configuration";
const char *accessPointPassword = "12345678";
bool configSaved = false;
// AP config --------------------------------------------

// PIR config -------------------------------------------
const int MOTION_SENSOR = 5;          // GPIO5 = D1 - Motion Sensor
volatile bool motionDetected = false; // Data to send over HTTP

ICACHE_RAM_ATTR void detectsMovement()
{
    motionDetected = true;
}
// PIR config -------------------------------------------

void setup()
{

    Serial.begin(115200);
    initOLED();
    updateOLED(deviceType + " " + appVersion, "", "  (co tu sie      )", "   \\  odpierdala_/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");
    delay(3000);

    // Config setup -------------------------------------------

    initLittleFS();
    loadConfiguration();

    String apIP = initAccessPoint(accessPointName, accessPointPassword);
    initWebGUI();
    updateOLED("CONFIG MODE", "AP Started!", "", "Go to IP:", apIP, "Waiting for Config...", "", "");

    // Configuration Mode
    unsigned long apStartTime = millis();
    int lastSecondsLeft = -1;
    bool timerActive = true;

    while (!configSaved)
    {
        server.handleClient();

        int clients = WiFi.softAPgetStationNum();

        if (clients > 0) // if client deactivate timer
        {
            timerActive = false;
            updateOLED("CONFIG MODE", "", "AP: " + String(accessPointName), "PASS: " + String(accessPointPassword), "Go to IP:", WiFi.softAPIP().toString(), "Waitig for Config", "");
        }
        else if (timerActive)
        {
            int secondsLeft = 10 - ((millis() - apStartTime) / 1000);

            if (secondsLeft != lastSecondsLeft)
            {
                updateOLED("CONFIG MODE", "", "AP: " + String(accessPointName), "PASS: " + String(accessPointPassword), "", "Waiting for client", "Starting in: " + String(secondsLeft) + "s", "");
                lastSecondsLeft = secondsLeft;
            }
            if (secondsLeft <= 0)
            {
                Serial.println("no clients. Starting normally.");
                break;
            }
        }
        else

            yield();
    }

    server.stop();
    WiFi.softAPdisconnect(true);
    // Config setup -------------------------------------------

    updateOLED("SSID: " + String(deviceConfig.ssid), "", "  (Connecting to  )", "   \\  the WIFI  _/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");

    initWifiClient(deviceConfig.ssid, deviceConfig.password);

    updateOLED("SSID: " + String(deviceConfig.ssid), "", "  (Okay, I\'m .    )", "   \\  ONLINE!!  _/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");
    initPushover(deviceConfig.pushoverApiToken, deviceConfig.pushoverUserKey, deviceType + " " + appVersion);
    sendPushover("Watchdog has just started its watch.");

    initJson(deviceConfig.serverJson, deviceType, deviceConfig.deviceName);
    sendJson("String addInfo", 666, "String type", "String requestID");

    updateOLED("SSID: " + String(deviceConfig.ssid), "FIRMWARE UPDATE", "Version: " + appVersion, "OTA Server:", deviceConfig.serverOta.substring(0, 20), "Status:", "...", "");
    delay(3000);

    String otaStatus = checkForUpdates(deviceConfig.serverOta, appVersion);
    updateOLED("SSID: " + String(deviceConfig.ssid), "FIRMWARE UPDATE", "Version: " + appVersion, "OTA Server:", deviceConfig.serverOta.substring(0, 20), "Status:", otaStatus, "");
    delay(3000);

    app_setup();
    // PIR setup -------------------------------------------
    pinMode(MOTION_SENSOR, INPUT_PULLUP);                                           // PIR Motion Sensor mode INPUT_PULLUP
    attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR), detectsMovement, RISING); // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
    // PIR setup -------------------------------------------
}

void loop()
{
    app_loop();
}