#pragma once

// PIR config -------------------------------------------
const int MOTION_SENSOR = 5;          // GPIO5 = D1 - Motion Sensor
volatile bool motionDetected = false;

IRAM_ATTR void detectsMovement()
{
    motionDetected = true;
}
// PIR config -------------------------------------------

void init_Pir()
{
    if (!isModuleActive("PIR"))
    {
        addLog("MODULE_PIR- Module is disabled in configuration.");
        return;
    }
    addLog("MODULE_PIR - enabled.");
    pinMode(MOTION_SENSOR, INPUT_PULLUP);                                           // PIR Motion Sensor mode INPUT_PULLUP
    attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR), detectsMovement, RISING); // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
}

void checkPir()
{
    if (motionDetected == true)
    {
        motionDetected = false;
        Serial.println("Interrupt!!! - Motion detected");
        sendJson("Motion detected", 1, "Alert");
    }
}
