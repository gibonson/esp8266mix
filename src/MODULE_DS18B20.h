#pragma once

#include <OneWire.h>           // OneWire library
#include <DallasTemperature.h> // DS18B20 Dallas Temperature library

// DS18B20 sensor setup
const int ONE_WIRE_BUS = 16;         // GPIO16 = D0 pin connected to the  sensor
OneWire oneWire(ONE_WIRE_BUS);       // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature sensor

void renderDS18B20Gui()
{
    server.sendContent(webGui.pHtml("webFormDS18B20"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "getDS18B20"));
    server.sendContent(webGui.formEnd("Get sensor value DS18B20"));
}

void init_ds18b20()
{
    if (!isModuleActive("DS18B20"))
    {
        addLog("MODULE_DS18B20 - Module is disabled in configuration.");
        return;
    }
    addLog("MODULE_DS18B20 - enabled.");
    registerModuleGui(renderDS18B20Gui);
    sensors.begin(); // Initialize the DS18B20 sensor
}

void execute_ds18b20(StaticJsonDocument<400> jsonDoc)
{

    if (!isModuleActive("DS18B20"))
    {
        addLog("MODULE_DS18B20 - Module is disabled in configuration.");
        return;
    }

    sensors.requestTemperatures();
    float newT = sensors.getTempCByIndex(0);
    Serial.print("DS18B20 Temperature: ");
    Serial.println(newT);
    if (newT == DEVICE_DISCONNECTED_C)
    {
        responseJson("DS18B20 sensor disconnected", 0, "error", jsonDoc["requestID"].as<String>());
        sendJson("DS18B20 sensor disconnected: ", 0, "error", jsonDoc["requestID"].as<String>());
    }
    else
    {
        responseJson("DS18B20 data", 1, "log", jsonDoc["requestID"].as<String>());
        sendJson("DS18B20 temperature: ", newT, "°C", jsonDoc["requestID"].as<String>());
    }
}
