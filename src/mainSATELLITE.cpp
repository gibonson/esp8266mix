#include <Arduino.h>
#include <ArduinoJson.h>      // JSON library for Arduino, used to create and parse JSON objects
#include <ESP8266WebServer.h> // Dedykowany serwer HTTP
ESP8266WebServer server(80);  // Set web server port number to 80

// app version
String deviceType = "ESP-Saelite";
String appVersion = "0.00";

// Project files list
String moduleList = "DS18B20,DHT22,OLED,BuiltinLed,RADIO_433"; // List of modules available in the system

#include "BOARD_LOGGING.h"       // Log management file
#include "BOARD_CONFIG.h" // Board configuration file
#include "BOARD_WIFI.h"          // Wi-Fi management file
#include "BOARD_JSON.h"
#include "BOARD_WEB_GUI_SA.h" // Web GUI management file

WebGui webGui;

#include "MODULE_OLED.h"
#include "MODULE_DS18B20.h"   // DS18B20 configuration file
#include "MODULE_DHT22.h"     // DHT22 configuration file
#include "MODULE_LED_PIN.h"   // LED pin configuration file
#include "MODULE_RADIO_433.h" // RF remote control configuration file

const int MOTION_SENSOR = 5;          // GPIO5 = D1 - Motion Sensor
volatile bool motionDetected = false; // Data to send over HTTP

// Checks if motion was detected
ICACHE_RAM_ATTR void detectsMovement()
{
  motionDetected = true;
}

void setupRouting()
{

  server.on("/", HTTP_GET, []()
            {
              server.setContentLength(CONTENT_LENGTH_UNKNOWN); // Tryb strumieniowania
              server.send(200, "text/html", "");               // Wysyłamy same nagłówki
              webGui.streamWebPage();                          // Nasza funkcja (bez argumentu server.client!)
              server.sendContent("");                          // ZAMYKA strumień HTML!
            });

  server.on("/newConfig", HTTP_GET, []()
            {
              server.setContentLength(CONTENT_LENGTH_UNKNOWN);
              server.send(200, "text/html", "");
              webGui.streamNewConfigPage(deviceConfig.ssid, deviceConfig.password, deviceConfig.deviceIP, deviceConfig.deviceName, deviceConfig.serverAddress, deviceConfig.disableModuleList);
              server.sendContent(""); // ZAMYKA strumień HTML!
            });

  server.on("/saveNewConfig", HTTP_POST, []()
            {
    if (server.hasArg("ssid")) deviceConfig.ssid = server.arg("ssid");
    if (server.hasArg("password")) deviceConfig.password = server.arg("password");
    if (server.hasArg("deviceIP")) deviceConfig.deviceIP = server.arg("deviceIP");
    if (server.hasArg("deviceName")) deviceConfig.deviceName = server.arg("deviceName");
    if (server.hasArg("serverAddress")) deviceConfig.serverAddress = server.arg("serverAddress");
    if (server.hasArg("disableList")) deviceConfig.disableModuleList = server.arg("disableList");
    
    saveConfig();
    server.send(200, "text/html", "<html><body><h1>Configuration updated</h1><p>Your settings were saved.</p><a href='/newConfig'><button class='button'>Back</button></a><a href='/'><button class='button'>Home</button></a></body></html>"); });

  server.on("/logs", HTTP_GET, []()
            {
              server.setContentLength(CONTENT_LENGTH_UNKNOWN);
              server.send(200, "text/plain", "");

              int start = (logIndex + LOG_SIZE - logCount) % LOG_SIZE;
              for (int i = 0; i < logCount; i++)
              {
                int idx = (start + i) % LOG_SIZE;
                if (logs[idx].length() > 0)
                {
                  server.sendContent(logs[idx] + "\n"); // Wysyłamy log po logu
                }
              }
              server.sendContent(""); // Koniec strumienia
            });

  server.on("/status", HTTP_GET, []()
            { responseJson("Connection ok", 1, "log", "Device Status"); });

  server.on("/readConfig", HTTP_GET, []()
            {
          String configData = "SSID: " + deviceConfig.ssid + "\nPassword: " + deviceConfig.password + "\nDevice IP: " + deviceConfig.deviceIP + "\nDevice Name: " + deviceConfig.deviceName + "\nServer Address: " + deviceConfig.serverAddress + "\nDisable List: " + deviceConfig.disableModuleList;
          responseJson(configData, 1, "log", "Device Config"); });

  server.on("/restart", HTTP_GET, []()
            {
    responseJson("Reset", 1, "log", "Device Status");
    delay(500);
    ESP.restart(); });

  server.on("/json", HTTP_POST, []()
            {
    String jsonString = server.arg("plain"); // Automatyczne pobranie ciała (body) JSON
    StaticJsonDocument<400> jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error) {
      server.send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
      return;
    }
    if (jsonDoc["requestID"].isNull()) jsonDoc["requestID"] = "Device request";
    
    String func = jsonDoc["function"].as<String>();

    if (func == "lcd") execute_oled(jsonDoc);
    else if (func == "builtinLed") execute_builtinLed(jsonDoc);
    else if (func == "led_pin_2") execute_led_pin_2(jsonDoc);
    else if (func == "led_pin_3") execute_led_pin_3(jsonDoc);
    else if (func == "led_pin_4") execute_led_pin_4(jsonDoc);
    else if (func == "led_pin_5") execute_led_pin_5(jsonDoc);
    else if (func == "led_pin_all") execute_led_pin_all(jsonDoc);
    else if (func == "getDHT22") execute_dht22(jsonDoc);
    else if (func == "getDS18B20") execute_ds18b20(jsonDoc);
    else if (func.indexOf("433") >= 0) execute_433(jsonDoc);
    else responseJson("Unknown function: " + func, 0, "error", jsonDoc["requestID"].as<String>()); });
}

void setup()
{
  Serial.begin(115200);
  delay(1000);

  init_baord_file_system(); // Initialize the file system
  init_configuration();     // Initialize configuration settings
  readSettings();

  initWifiClient(deviceConfig.ssid, deviceConfig.password);
  delay(500);

  // here add all initialization functions
  init_oled();     // Initialize OLED display
  init_led_pins(); // Initialize LED pins
  init_ds18b20();  // Initialize DS18B20 sensor configuration
  init_dht22();    // Initialize DHT22 sensor configuration
  init_433();      // Initialize RF 433 module

  sendJson("Device started", 1, "log");

  pinMode(MOTION_SENSOR, INPUT_PULLUP);                                           // PIR Motion Sensor mode INPUT_PULLUP
  attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR), detectsMovement, RISING); // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
  setupRouting();
}

void loop()
{

  if (motionDetected == true)
  {
    motionDetected = false;
    Serial.println("Interrupt!!! - Motion detected");
    sendJson("Motion detected", 1, "Alert");
  }

  handleLedSequence();   // Checking LEDs without blocking
  server.handleClient(); // <--- Ta JEDNA komenda obsługuje teraz całą sieć WWW i zapytania!
}
