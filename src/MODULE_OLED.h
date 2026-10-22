#pragma once

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// --- LCD Configuration ---
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // If not work please try 0x3D
#define OLED_SDA D5         // Stock firmware shows wrong pins
#define OLED_SCL D6         // They swap SDA with SCL ;)

static Adafruit_SSD1306 *display = nullptr;
static bool isOledConnected = false;

#ifdef ESP8266_SATELLITE
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
#endif

void initOled()
{

  if (!isModuleActive("OLED"))
  {
    addLog("MODULE_OLED - Module is disabled in configuration.");
    return;
  }
  #ifdef ESP8266_SATELLITE
    registerModuleGui(renderOledGui);
  #endif
  
  display = new Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    addLog("MODULE_OLED - Allocation or connection failed! Hardware missing?");
    isOledConnected = false;
    delete display;
    display = nullptr;
    return;
  }

  isOledConnected = true;
  display->clearDisplay();
  display->display();
  addLog("MODULE_OLED - Initialized successfully.");
}

void updateOled(String line1, String line2, String line3, String line4, String line5, String line6, String line7, String line8)
{
  // Zabezpieczenie przed zawieszeniem, jeśli brak sprzętu lub pamięci
  if (!isOledConnected || display == nullptr)
    return;

  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(SSD1306_WHITE);
  display->setCursor(0, 0);

  display->println(line1);
  display->println(line2);
  display->println(line3);
  display->println(line4);
  display->println(line5);
  display->println(line6);
  display->println(line7);
  display->println(line8);

  display->display();
}

void executeOled(StaticJsonDocument<400> jsonDoc)
{
  if (!isModuleActive("OLED"))
  {
    addLog("MODULE_OLED - Module is disabled in configuration.");
    return;
  }

  if (!isOledConnected)
  {
    responseJson("OLED hardware error", 0, "error", jsonDoc["requestID"].as<String>());
    return;
  }

  String v1 = jsonDoc["value1"].as<String>();
  String v2 = jsonDoc["value2"].as<String>();
  String v3 = jsonDoc["value3"].as<String>();
  String v4 = jsonDoc["value4"].as<String>();
  String v5 = jsonDoc["value5"].as<String>();
  String v6 = jsonDoc["value6"].as<String>();
  String v7 = jsonDoc["value7"].as<String>();
  String v8 = jsonDoc["value8"].as<String>();

  updateOled(v1, v2, v3, v4, v5, v6, v7, v8);
  responseJson("OLED updated", 1, "log", jsonDoc["requestID"].as<String>());
}