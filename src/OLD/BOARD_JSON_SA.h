#pragma once

// Supports sending json to server address. Requires deviceConfig from Configuration.h file

void sendJson(String addInfo, int value, String type, String requestID = "")
{

  http.begin(client, deviceConfig.serverAddress);
  http.addHeader("Content-Type", "application/json");
  StaticJsonDocument<400> jsonDoc;

  jsonDoc["deviceIP"] = local_IP.toString();
  jsonDoc["deviceName"] = deviceConfig.deviceName;
  jsonDoc["requestID"] = requestID;
  jsonDoc["addInfo"] = addInfo;
  jsonDoc["type"] = type;
  jsonDoc["value"] = value;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  Serial.println("Json to sent: " + jsonString);

  int httpCode = http.POST(jsonString);

  if (httpCode > 0)
  {
    addLog("Send JSON Success (" + String(httpCode) + "): " + jsonString);
  }
  else
  {
    String errorMsg = http.errorToString(httpCode);
    addLog("Send JSON Error: " + errorMsg);
  }
}

void responseJson(String addInfo, int value, String type, String requestID = "")
{
  StaticJsonDocument<400> jsonDoc;
  jsonDoc["requestID"] = requestID;
  jsonDoc["deviceIP"] = String(local_IP[0]) + "." + String(local_IP[1]) + "." + String(local_IP[2]) + "." + String(local_IP[3]);
  jsonDoc["deviceName"] = deviceConfig.deviceName;
  jsonDoc["addInfo"] = addInfo;
  jsonDoc["type"] = type;
  jsonDoc["value"] = value;

  String jsonString;
  serializeJson(jsonDoc, jsonString);

  server.send(200, "application/json", jsonString);
  addLog("Response JSON sent:" + jsonString);
}