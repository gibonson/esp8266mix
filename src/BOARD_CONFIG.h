#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

struct DeviceConfig
{
    // --- WI-FI Configuration---
    String ssid;
    String password;
    IPAddress deviceIp;
    // --- PUSHOVER Configuration---
    String pushoverApiToken = "";
    String pushoverUserKey = "";
    // --- Name Configuration---
    String deviceName;
    // --- JSON Configuration---
    String serverJson;
    // --- OTA Configuration---
    String serverOta;
};

DeviceConfig deviceConfig;


struct ModuleConfig
{
    String moduleName;
    String moduleActive;
};

ModuleConfig moduleConfig[10];


bool isModuleActive(String name)
{
    for (int i = 0; i < 10; i++)
    {
        if (moduleConfig[i].moduleName == name)
        {
            return (moduleConfig[i].moduleActive == "yes");
        }
    }
    return false; // Jeśli modułu nie ma na liście, domyślnie jest wyłączony
}


#ifdef ESP8266_WATCHDOG
struct ServiceCheckConfig
{
    String ipsName;
    String ips;
    String ipsPort;
};

ServiceCheckConfig serviceCheckConfig[6];
#endif

// --- Config template ---

#ifdef ESP8266_WATCHDOG
const String defaultJsonTemplate = "{\n"
                                   "  \"wifi\": {\n"
                                   "    \"ssid\": \"\",\n"
                                   "    \"password\": \"\",\n"
                                   "    \"localIp\": \"192.168.0.199\"\n"
                                   "  },\n"
                                   "  \"pushover\": {\n"
                                   "    \"userKey\": \"\",\n"
                                   "    \"apiToken\": \"\"\n"
                                   "  },\n"
                                   "  \"serverJson\": \"http://192.168.0.242:5000/api/addEvent\",\n"
                                   "  \"serverOta\": \"\",\n"
                                   "  \"deviceList\": [\n"
                                   "    {\n"
                                   "      \"name\": \"Router\",\n"
                                   "      \"ip\": \"192.168.0.1\"\n"
                                   "    },\n"
                                   "    {\n"
                                   "      \"name\": \"Google DNS (Test)\",\n"
                                   "      \"ip\": \"8.8.8.8\"\n"
                                   "    }\n"
                                   "  ],\n"
                                   "  \"moduleList\": [\n"
                                   "    {\n"
                                   "      \"name\": \"OLED\",\n"
                                   "      \"active\": \"yes\"\n"
                                   "    }\n"
                                   "  ]\n"
                                   "}";
#endif

#ifdef ESP8266_SATELLITE
const String defaultJsonTemplate = "{\n"
                                   "  \"wifi\": {\n"
                                   "    \"ssid\": \"\",\n"
                                   "    \"password\": \"\",\n"
                                   "    \"localIp\": \"192.168.0.199\"\n"
                                   "  },\n"
                                   "  \"pushover\": {\n"
                                   "    \"userKey\": \"\",\n"
                                   "    \"apiToken\": \"\"\n"
                                   "  },\n"
                                   "  \"serverJson\": \"http://192.168.0.242:5000/api/addEvent\",\n"
                                   "  \"serverOta\": \"\",\n"
                                   "  \"moduleList\": [\n"
                                   "    {\n"
                                   "      \"name\": \"OLED\",\n"
                                   "      \"active\": \"yes\"\n"
                                   "    },\n"
                                   "    {\n"
                                   "      \"name\": \"DHT22\",\n"
                                   "      \"active\": \"no\"\n"
                                   "    }\n"
                                   "  ]\n"
                                   "}";
#endif

bool stringToIP(const String &str, IPAddress &ip)
{
    return ip.fromString(str);
}

void initLittleFS()
{
    if (!LittleFS.begin())
    {
        addLog("BOARD_CONFIG - failed mounting of LittleFS!");
    }
}

bool saveRawConfig(const String &rawData)
{
    File f = LittleFS.open("/config.json", "w");
    if (!f)
        return false;

    f.print(rawData);
    f.close();
    return true;
}

String readRawConfig()
{
    if (!LittleFS.exists("/config.json"))
    {
        addLog("BOARD_CONFIG - no /config.json - create config.json and save template");
        saveRawConfig(defaultJsonTemplate);
        return defaultJsonTemplate;
    }

    String content = "";
    File f = LittleFS.open("/config.json", "r");
    if (f)
    {
        content = f.readString();
        f.close();
    }

    DynamicJsonDocument testDoc(1024);
    DeserializationError error = deserializeJson(testDoc, content);

    if (error || content.length() == 0)
    {
        addLog("BOARD_CONFIG -config.json - empty or format error, load default template.");
        saveRawConfig(defaultJsonTemplate);
        return defaultJsonTemplate;
    }
    return content;
}

void loadConfiguration()
{
    String rawConfig = readRawConfig();

    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, rawConfig);

    if (error)
    {
        addLog("BOARD_CONFIG - JSON format error: " + String(error.c_str()));
        return;
    }

    deviceConfig.ssid = doc["wifi"]["ssid"] | "";
    deviceConfig.password = doc["wifi"]["password"] | "";
    String ipStr = doc["wifi"]["localIp"] | "";
    if (ipStr != "")
        stringToIP(ipStr, deviceConfig.deviceIp);

    deviceConfig.pushoverUserKey = doc["pushover"]["userKey"] | "";
    deviceConfig.pushoverApiToken = doc["pushover"]["apiToken"] | "";

    deviceConfig.serverJson = doc["serverJson"] | "";
    deviceConfig.serverOta = doc["serverOta"] | "";

#ifdef ESP8266_WATCHDOG

    JsonArray deviceList = doc["deviceList"].as<JsonArray>();

    int i = 0;
    for (JsonObject device : deviceList)
    {
        if (i >= 6)
            break;

        serviceCheckConfig[i].ipsName = device["name"] | "-";
        serviceCheckConfig[i].ips = device["ip"] | "";
        serviceCheckConfig[i].ipsPort = device["port"] | "";

        i++;
    }

    for (int j = i; j < 6; j++)
    {
        serviceCheckConfig[j].ips = "";
        serviceCheckConfig[j].ipsName = "-";
        serviceCheckConfig[j].ipsPort = "";
    }
#endif

#ifdef ESP8266_SATELLITE
    JsonArray moduleList = doc["moduleList"].as<JsonArray>();

    int i = 0;
    for (JsonObject module : moduleList)
    {
        if (i >= 10)
            break;

        moduleConfig[i].moduleName = module["name"] | "-";
        moduleConfig[i].moduleActive = module["active"] | "";

        i++;
    }

    for (int j = i; j < 10; j++)
    {
        moduleConfig[j].moduleName = "";
        moduleConfig[j].moduleActive = "-";
    }

#endif
    addLog("BOARD_CONFIG - Configuration loaded successfully.");
}