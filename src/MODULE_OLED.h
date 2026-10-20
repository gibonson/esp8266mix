#pragma once

// LCD libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// LCD Configuration
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // If not work please try 0x3D
#define OLED_SDA D5         // Stock firmware shows wrong pins
#define OLED_SCL D6         // They swap SDA with SCL ;)
Adafruit_SSD1306 *display;

void renderOledGui()
{
  server.sendContent(webGui.pHtml("webFormOLED"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "lcd"));
  server.sendContent(webGui.formText("Text to send:", "value1", "1"));
  server.sendContent(webGui.formText("Text to send:", "value2", "1"));
  server.sendContent(webGui.formText("Text to send:", "value3", "1"));
  server.sendContent(webGui.formText("Text to send:", "value4", "1"));
  server.sendContent(webGui.formText("Text to send:", "value5", "1"));
  server.sendContent(webGui.formText("Text to send:", "value6", "1"));
  server.sendContent(webGui.formText("Text to send:", "value7", "1"));
  server.sendContent(webGui.formText("Text to send:", "value8", "1"));
  server.sendContent(webGui.formEnd("Send to LCD"));
}

// to void setup
void init_oled()
{
  String moduleName = "OLED";
  Serial.println(deviceConfig.disableModuleList);

  if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
  {
    Serial.println("Module " + moduleName + " is disabled.");
  }
  else
  {
    registerModuleGui(renderOledGui);
    display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    // OLED used nonstandard SDA and SCL pins
    Wire.begin(D5, D6);
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
    {
      Serial.println(F("SSD1306 allocation failed"));
      return;
    }
  }
}

void handle_oled(String value1, String value2, String value3, String value4, String value5, String value6, String value7, String value8)
{
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);

  display->println(value1);
  display->println(value2);
  display->println(value3);
  display->println(value4);
  display->println(value5);
  display->println(value6);
  display->println(value7);
  display->println(value8);
  display->println(value8);

  display->display();
}

void execute_oled(StaticJsonDocument<400> jsonDoc)
{
  String moduleName = "OLED";
  if (deviceConfig.disableModuleList.indexOf(moduleName) != -1)
  {
    responseJson("Module " + moduleName + " is disabled in disableModuleList", 0, "error", jsonDoc["requestID"].as<String>());
  }
  else
  {
    String value1 = jsonDoc["value1"].as<String>();
    String value2 = jsonDoc["value2"].as<String>();
    String value3 = jsonDoc["value3"].as<String>();
    String value4 = jsonDoc["value4"].as<String>();
    String value5 = jsonDoc["value5"].as<String>();
    String value6 = jsonDoc["value6"].as<String>();
    String value7 = jsonDoc["value7"].as<String>();
    String value8 = jsonDoc["value8"].as<String>();

    responseJson("OLED updated", 1, "log", jsonDoc["requestID"].as<String>());
    handle_oled(value1, value2, value3, value4, value5, value6, value7, value8);
  }
}
