#include <Arduino.h>
#include <ESP8266Ping.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

ESP8266WebServer server(80);
String appVersion = "100.100";

#include "BOARD_LOGGING.h"
#include "BOARD_CONFIG.h"
#include "BOARD_JSON.h"
#include "BOARD_WEB_GUI.h" // Web GUI management file

#include "MODULE_OLED.h"
#include "MODULE_PIR.h"

#include "BOARD_WIFI.h"
#include "BOARD_OTA.h"
#include "BOARD_PUSHOVER.h"

// WatchDog config --------------------------------------------
#ifdef ESP8266_WATCHDOG
String deviceType = "ESP-WatchDog";
#include "APP_WATCHDOG.h"
#endif
// WatchDog config --------------------------------------------

// Satellite config --------------------------------------------
#ifdef ESP8266_SATELLITE
String deviceType = "ESP-Satellite";
#include "APP_SATELLITE.h"
#endif
// Satellite config --------------------------------------------

// AP config --------------------------------------------
const char *accessPointName = "ESP-Configuration";
const char *accessPointPassword = "12345678";
bool configSaved = false;
// AP config --------------------------------------------

void setup()
{

    Serial.begin(115200);
    initLog(true);
    initLittleFS();
    loadConfiguration();

    initOled();
    updateOled(deviceType + " " + appVersion, "", "  (co tu sie      )", "   \\  odpierdala_/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");
    delay(3000);

    String apIP = initAccessPoint(accessPointName, accessPointPassword);
    initWebGUI();
    updateOled("CONFIG MODE", "AP Started!", "", "Go to IP:", apIP, "Waiting for Config...", "", "");

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
            updateOled("CONFIG MODE", "", "AP: " + String(accessPointName), "PASS: " + String(accessPointPassword), "Go to IP:", WiFi.softAPIP().toString(), "Waitig for Config", "");
        }
        else if (timerActive)
        {
            int secondsLeft = 10 - ((millis() - apStartTime) / 1000);

            if (secondsLeft != lastSecondsLeft)
            {
                updateOled("CONFIG MODE", "", "AP: " + String(accessPointName), "PASS: " + String(accessPointPassword), "", "Waiting for client", "Starting in: " + String(secondsLeft) + "s", "");
                lastSecondsLeft = secondsLeft;
            }
            if (secondsLeft <= 0)
            {
                Serial.println("no clients. Starting normally.");
                configSaved = true;
                break;
            }
        }
        else

            yield();
    }

    server.stop();
    WiFi.softAPdisconnect(true);
    // Config setup -------------------------------------------

    updateOled("SSID: " + String(deviceConfig.ssid), "", "  (Connecting to  )", "   \\  the WIFI  _/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");

    Serial.println("SSID" + deviceConfig.ssid);
    Serial.println("pass" + deviceConfig.password);
    initWifiClient(deviceConfig.ssid, deviceConfig.password);

    updateOled("SSID: " + String(deviceConfig.ssid), "", "  (Okay, I\'m .    )", "   \\  ONLINE!!  _/", "    \\/", "     |\\__/,|   ( \\", "   _.|o o  |_   ) )", " -(((---(((--------");
    initPushover(deviceConfig.pushoverApiToken, deviceConfig.pushoverUserKey, deviceType + " " + appVersion);
    sendPushover("Watchdog has just started its watch.");

    initJson(deviceConfig.serverJson, deviceConfig.deviceName, WiFi.localIP().toString());
    sendJson("String addInfo", 666, "String type", "String requestID");

    updateOled("SSID: " + String(deviceConfig.ssid), "FIRMWARE UPDATE", "Version: " + appVersion, "OTA Server:", deviceConfig.serverOta.substring(0, 20), "Status:", "...", "");
    delay(3000);

    String otaStatus = checkForUpdates(deviceConfig.serverOta, appVersion);
    updateOled("SSID: " + String(deviceConfig.ssid), "FIRMWARE UPDATE", "Version: " + appVersion, "OTA Server:", deviceConfig.serverOta.substring(0, 20), "Status:", otaStatus, "");
    delay(3000);

    app_setup();
}

void loop()
{
    checkPir();
    app_loop();
}