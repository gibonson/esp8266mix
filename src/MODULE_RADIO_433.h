#pragma once

#include "RCSwitch.h"

// RF 433 remote control setup
RCSwitch mySwitch = RCSwitch();  // Initialize RF switch
const int RC_TRANSMITER_PIN = 0; // GPIO0 = D3 Transmitter data pin

void render433Gui()
{
    server.sendContent(webGui.pHtml("webForm433"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket1ON"));
    server.sendContent(webGui.formEnd("socket1ON"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket1OFF"));
    server.sendContent(webGui.formEnd("socket1OFF"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket2ON"));
    server.sendContent(webGui.formEnd("socket2ON"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket2OFF"));
    server.sendContent(webGui.formEnd("socket2OFF"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket3ON"));
    server.sendContent(webGui.formEnd("socket3ON"));
    server.sendContent(webGui.formBegin());
    server.sendContent(webGui.formHidden("", "function", "433socket3OFF"));
    server.sendContent(webGui.formEnd("socket3OFF"));
}

void init_433()
{
    String moduleName = "433";
    Serial.println(deviceConfig.disableModuleList);
    if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
    {
        Serial.println("Module " + moduleName + " is disabled.");
    }
    else
    {
        Serial.println("Initializing module: " + moduleName);
        registerModuleGui(render433Gui);

        mySwitch.enableTransmit(RC_TRANSMITER_PIN); // Transmitter is connected to Arduino Pin #0
        mySwitch.setProtocol(1);                    // Optional set protocol (default is 1, will work for most outlets)
        mySwitch.setPulseLength(350);               // Optional set pulse length.
                                                    // mySwitch.setRepeatTransmit(15);  // Optional set number of transmission repetitions.
    }
}

void execute_433(StaticJsonDocument<400> jsonDoc)
{
    String moduleName = "433";
    if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
    {
        responseJson("Module " + moduleName + " is disabled in disableModuleList", 0, "error", jsonDoc["requestID"].as<String>());
    }
    else
    {
        if (String(jsonDoc["function"]).indexOf("socket1ON") >= 0)
        {
            responseJson("S433_1_ON", 1, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(4433, 24);
            sendJson("S433_1_ON", 1, "log", jsonDoc["requestID"].as<String>());
        }
        else if (String(jsonDoc["function"]).indexOf("socket1OFF") >= 0)
        {
            responseJson("S433_1_OFF", 0, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(4436, 24);
            sendJson("S433_1_OFF", 0, "log", jsonDoc["requestID"].as<String>());
        }

        if (String(jsonDoc["function"]).indexOf("socket2ON") >= 0)
        {
            responseJson("S433_2_ON", 1, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(5201, 24);
            sendJson("S433_2_ON", 1, "log", jsonDoc["requestID"].as<String>());
        }
        else if (String(jsonDoc["function"]).indexOf("socket2OFF") >= 0)
        {
            responseJson("S433_2_OFF", 0, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(5204, 24);
            sendJson("S433_2_OFF", 0, "log", jsonDoc["requestID"].as<String>());
        }

        if (String(jsonDoc["function"]).indexOf("socket3ON") >= 0)
        {
            responseJson("S433_3_ON", 1, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(5393, 24);
            sendJson("S433_3_ON", 1, "log", jsonDoc["requestID"].as<String>());
        }
        else if (String(jsonDoc["function"]).indexOf("socket3OFF") >= 0)
        {
            responseJson("S433_2_OFF", 0, "log", jsonDoc["requestID"].as<String>());
            mySwitch.send(5396, 24);
            sendJson("S433_3_OFF", 0, "log", jsonDoc["requestID"].as<String>());
        }
        else
        {
            responseJson("Unknown 433 function", 0, "error", jsonDoc["requestID"].as<String>());
        }
    }
}