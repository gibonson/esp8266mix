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
        return;
    }
    registerModuleGui(renderDS18B20Gui);
    sensors.begin(); // Initialize the DS18B20 sensor
}

void execute_ds18b20(StaticJsonDocument<400> jsonDoc)
{
    if (!isModuleActive("DS18B20"))
    {
        return;
    }

    sensors.requestTemperatures();
    float newT = sensors.getTempCByIndex(0);

    if (newT == DEVICE_DISCONNECTED_C)
    {
        sysLog("DS18B20 disconnected", 0, "error", jsonDoc["requestID"].as<String>(), true, true, true);
    }
    else
    {
        sysLog("DS18B20 temperature: ", newT, "°C", jsonDoc["requestID"].as<String>(), true, true, true);
    }
}
