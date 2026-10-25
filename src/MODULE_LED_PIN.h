#pragma once

const int LED_PIN_1 = 2;  // GPIO2 = D4 = ESP LED - LED_PIN_1
const int LED_PIN_2 = 13; // GPIO13= D7 - Relay 1 control pin
const int LED_PIN_3 = 15; // GPIO15= D8 - Relay 2 control pin
const int LED_PIN_4 = 3;  // GPIO3= RX - Relay 3 control pin
const int LED_PIN_5 = 1;  // GPIO1 = TX - Relay 4 control pin

const int sequencePins[5] = {LED_PIN_1, LED_PIN_2, LED_PIN_3, LED_PIN_4, LED_PIN_5};
unsigned long sequenceDelays[5];
int currentLedStep = -1; // -1 oznacza, że sekwencja nie jest aktywna
unsigned long ledStepStartTime = 0;
String sequenceReqID = "";

void renderLEDGui()
{
  server.sendContent(webGui.pHtml("webFormLed"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "builtinLed"));
  server.sendContent(webGui.formHidden("", "ledState", "on"));
  server.sendContent(webGui.formEnd("Led ON"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "builtinLed"));
  server.sendContent(webGui.formHidden("", "ledState", "off"));
  server.sendContent(webGui.formEnd("Led OFF"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_2"));
  server.sendContent(webGui.formHidden("", "ledState", "on"));
  server.sendContent(webGui.formEnd("Led 2 ON"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_2"));
  server.sendContent(webGui.formHidden("", "ledState", "off"));
  server.sendContent(webGui.formEnd("Led 2 OFF"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_3"));
  server.sendContent(webGui.formHidden("", "ledState", "on"));
  server.sendContent(webGui.formEnd("Led 3 ON"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_3"));
  server.sendContent(webGui.formHidden("", "ledState", "off"));
  server.sendContent(webGui.formEnd("Led 3 OFF"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_4"));
  server.sendContent(webGui.formHidden("", "ledState", "on"));
  server.sendContent(webGui.formEnd("Led 4 ON"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_4"));
  server.sendContent(webGui.formHidden("", "ledState", "off"));
  server.sendContent(webGui.formEnd("Led 4 OFF"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_5"));
  server.sendContent(webGui.formHidden("", "ledState", "on"));
  server.sendContent(webGui.formEnd("Led 5 ON"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_pin_5"));
  server.sendContent(webGui.formHidden("", "ledState", "off"));
  server.sendContent(webGui.formEnd("Led 5 OFF"));
  server.sendContent(webGui.formBegin());
  server.sendContent(webGui.formHidden("", "function", "led_timer"));
  server.sendContent(webGui.formText("builtinLed", "value1", "1"));
  server.sendContent(webGui.formText("led_pin_2", "value2", "1"));
  server.sendContent(webGui.formText("led_pin_3", "value3", "1"));
  server.sendContent(webGui.formText("led_pin_4", "value4", "1"));
  server.sendContent(webGui.formText("led_pin_5", "value5", "1"));
  server.sendContent(webGui.formEnd("Start timer"));
}

void init_led_pins()
{
  registerModuleGui(renderLEDGui);
  pinMode(LED_PIN_1, OUTPUT);
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
}

void execute_single_led(StaticJsonDocument<400> &jsonDoc)
{
  String func = jsonDoc["function"].as<String>();
  String state = jsonDoc["ledState"].as<String>();
  String reqID = jsonDoc["requestID"].as<String>();

  int targetPin = -1;
  if (func == "builtinLed")
    targetPin = LED_PIN_1;
  if (func == "led_pin_2")
    targetPin = LED_PIN_2;
  if (func == "led_pin_3")
    targetPin = LED_PIN_3;
  if (func == "led_pin_4")
    targetPin = LED_PIN_4;
  if (func == "led_pin_5")
    targetPin = LED_PIN_5;

  if (targetPin == -1)
  {
    sysLog("Unknown LED: " + func, 0, "error", reqID, true, true, true);
    return;
  }

  if (state == "on")
  {
    digitalWrite(targetPin, HIGH);
    sysLog(func + " ON", 1, "log", reqID, true, true, true);
  }
  else if (state == "off")
  {
    digitalWrite(targetPin, LOW);
    sysLog(func + " OFF", 0, "log", reqID, true, true, true);
  }
  else
  {
    sysLog("Unknown state: " + state, 0, "error", reqID, true, true, true);
  }
}

void execute_led_timer(StaticJsonDocument<400> &jsonDoc)
{
  sequenceDelays[0] = jsonDoc["value1"].as<int>() * 1000;
  sequenceDelays[1] = jsonDoc["value2"].as<int>() * 1000;
  sequenceDelays[2] = jsonDoc["value3"].as<int>() * 1000;
  sequenceDelays[3] = jsonDoc["value4"].as<int>() * 1000;
  sequenceDelays[4] = jsonDoc["value5"].as<int>() * 1000;
  sequenceReqID = jsonDoc["requestID"].as<String>();

  // 2. Startujemy pierwszy krok sekwencji
  currentLedStep = 0;
  digitalWrite(sequencePins[0], HIGH);
  ledStepStartTime = millis(); // włączamy stoper

  sysLog("led_pin_timer started", 1, "log", sequenceReqID, true, true, true);
}

// Ta funkcja będzie ciągle "nasłuchiwać" w głównej pętli
void handleLedSequence()
{
  // Jeśli sekwencja jest aktywna (kroki od 0 do 4)
  if (currentLedStep >= 0 && currentLedStep < 5)
  {
    // Jeśli upłynął czas dla obecnego pinu
    if (millis() - ledStepStartTime >= sequenceDelays[currentLedStep])
    {
      // Wyłącz obecny pin
      digitalWrite(sequencePins[currentLedStep], LOW);
      addLog("led_pin_" + String(currentLedStep + 1) + " - OFF");

      currentLedStep++; // Przejdź do następnego kroku

      // Jeśli to nie koniec, włącz następny pin
      if (currentLedStep < 5)
      {
        digitalWrite(sequencePins[currentLedStep], HIGH);
        addLog("led_pin_" + String(currentLedStep + 1) + " - ON");
        ledStepStartTime = millis(); // reset stopera
      }
      else
      {
        currentLedStep = -1; // Zakończ sekwencję
      }
    }
  }
}