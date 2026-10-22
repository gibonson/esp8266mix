#pragma once

#include "MODULE_DS18B20.h"   // DS18B20 configuration file
#include "MODULE_DHT22.h"     // DHT22 configuration file
#include "MODULE_LED_PIN.h"   // LED pin configuration file
#include "MODULE_RADIO_433.h" // RF remote control configuration file

String moduleList = "DS18B20,DHT22,OLED,BuiltinLed,RADIO_433"; // List of modules available in the system
void setupRouting()
{

    server.on("/", HTTP_GET, []()
              {
                  server.setContentLength(CONTENT_LENGTH_UNKNOWN); // Tryb strumieniowania
                  server.send(200, "text/html", "");               // Wysyłamy same nagłówki
                  webGui.streamWebPage();                          // Nasza funkcja (bez argumentu server.client!)
                  server.sendContent("");                          // ZAMYKA strumień HTML!
              });

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

    if (func == "lcd") executeOled(jsonDoc);
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

    server.begin(); 
    addLog("APP_SATELLITE - Web GUI Server started!");
}

void app_setup()
{
    addLog("APP_SATELLITE - Uruchamianie logiki satelity...");
    
    init_led_pins(); // Initialize LED pins
    init_ds18b20();  // Initialize DS18B20 sensor configuration
    init_dht22();    // Initialize DHT22 sensor configuration
    init_433();      // Initialize RF 433 module

    setupRouting();
}

void app_loop()
{
    Serial.println("test");
    handleLedSequence();   // Checking LEDs without blocking
    server.handleClient(); // <--- Ta JEDNA komenda obsługuje teraz całą sieć WWW i zapytania!
}