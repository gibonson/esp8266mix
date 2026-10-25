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
    if (!isModuleActive("433"))
    {
        return;
    }
    registerModuleGui(render433Gui);
    mySwitch.enableTransmit(RC_TRANSMITER_PIN); // Transmitter is connected to Arduino Pin #0
    mySwitch.setProtocol(1);                    // Optional set protocol (default is 1, will work for most outlets)
    mySwitch.setPulseLength(350);               // Optional set pulse length.
                                                // mySwitch.setRepeatTransmit(15);  // Optional set number of transmission repetitions.
}

void execute_433(StaticJsonDocument<400> &jsonDoc)
{
    if (!isModuleActive("433"))
    {
        return;
    }

    String func = jsonDoc["function"].as<String>();
    String reqID = jsonDoc["requestID"].as<String>();

    int codesON[] = {4433, 5201, 5393};
    int codesOFF[] = {4436, 5204, 5396};

    for (int i = 1; i <= 3; i++)
    {
        if (func.indexOf("socket" + String(i) + "ON") >= 0)
        {
            mySwitch.send(codesON[i - 1], 24);
            sysLog("S433_" + String(i) + "_ON", 1, "log", reqID, true, true, true);
            return;
        }
        if (func.indexOf("socket" + String(i) + "OFF") >= 0)
        {
            mySwitch.send(codesOFF[i - 1], 24);
            sysLog("S433_" + String(i) + "_OFF", 0, "log", reqID, true, true, true);
            return;
        }
    }

    sysLog("Unknown 433: " + func, 0, "error", reqID, true, true, true);
}