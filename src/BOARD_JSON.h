#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

static String _jsonServerUrl = "";
static String _jsonDeviceIP = "";
static String _jsonDeviceName = "UnknownDevice";

void initJson(String serverUrl, String deviceName, String deviceIP)
{
  _jsonServerUrl = serverUrl;
  _jsonDeviceName = deviceName;
  _jsonDeviceIP = deviceIP;
  addLog("BOARD_JSON - Module initialized.");
  addLog("BOARD_JSON - Server URL: " + _jsonServerUrl);
  addLog("BOARD_JSON - Device Name: " + _jsonDeviceName);
}

static String buildJsonString(String addInfo, int value, String type, String requestID)
{
  StaticJsonDocument<400> jsonDoc;
  jsonDoc["deviceIP"] = _jsonDeviceIP;
  jsonDoc["deviceName"] = _jsonDeviceName;
  jsonDoc["requestID"] = requestID;
  jsonDoc["addInfo"] = addInfo;
  jsonDoc["type"] = type;
  jsonDoc["value"] = value;

  String jsonString;
  serializeJson(jsonDoc, jsonString);
  return jsonString;
}

void sendJson(String addInfo, int value, String type, String requestID = "")
{
  if (_jsonServerUrl == "")
  {
    addLog("BOARD_JSON - Send JSON Error: No endpoint address configured.");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  String jsonString = buildJsonString(addInfo, value, type, requestID);
  addLog("BOARD_JSON - Json to sent: " + jsonString);

  http.begin(client, _jsonServerUrl);
  http.addHeader("Content-Type", "application/json");
  int httpCode = http.POST(jsonString);

  if (httpCode > 0)
  {
    addLog("BOARD_JSON - " + requestID + " Send JSON Success (" + String(httpCode) + ")");
  }
  else
  {
    addLog("BOARD_JSON - " + requestID + " Send JSON Error: " + http.errorToString(httpCode));
  }
  http.end();
}

void responseJson(String addInfo, int value, String type, String requestID = "")
{
  String jsonString = buildJsonString(addInfo, value, type, requestID);
  server.send(200, "application/json", jsonString);
  addLog("BOARD_JSON - " + requestID + " HttpResponse JSON sent.");
}