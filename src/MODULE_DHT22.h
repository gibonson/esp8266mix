#pragma once

#include <DHT.h> // DHT sensor library

// DHT sensor setup
#define DHTTYPE DHT22      // DHT22 - DHT 22(AM2302)
const int DHT_PIN = 4;     // GPIO4 = D2 Digital pin connected to the DHT sensor
DHT dht(DHT_PIN, DHTTYPE); // Initialize DHT sensor

void renderDHT22Gui()
{
    server.sendContent(webGui.pHtml("webFormDHT22"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "getDHT22"));
    server.sendContent(webGui.formEnd("Get sensor value DHT22"));
}

void init_dht22()
{
    String moduleName = "DHT22";
    Serial.println(deviceConfig.disableModuleList);
    if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
    {
        Serial.println("Module " + moduleName + " is disabled.");
    }
    else
    {
        Serial.println("Initializing module: " + moduleName);
        registerModuleGui(renderDHT22Gui);
        dht.begin(); // Initialize the DHT sensor
    }
}

void execute_dht22(StaticJsonDocument<400> jsonDoc)
{
    String moduleName = "DHT22";
    if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
    {
        responseJson("Module " + moduleName + " is disabled in disableModuleList", 0, "error", jsonDoc["requestID"].as<String>());
    }
    else
    {
        // float newT = random(20, 30); // Simulated temperature value
        // float newH = random(40, 60); // Simulated humidity value
        float newT = dht.readTemperature();
        float newH = dht.readHumidity();
        responseJson("DHT22 data", 1, "log", jsonDoc["requestID"].as<String>());
        sendJson("DHT22 temperature: ", newT, "°C", jsonDoc["requestID"].as<String>());
        sendJson("DHT22 humidity: ", newH, "%", jsonDoc["requestID"].as<String>());
    }
}