#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <LittleFS.h>
#include <ESP8266WiFi.h>
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

struct ServiceCheckConfig
{
    String ipsName;
    String ips;
    String ipsPort;
};

ServiceCheckConfig serviceCheckConfig[6];

// // --- WI-FI Configuration---
// String ssid = "";
// String password = "";
// IPAddress deviceIp(192, 168, 0, 199); // Adres ESP

// // --- PUSHOVER Configuration---
// String pushoverApiToken = "";
// String pushoverUserKey = "";

// --- JSON Configuration---
// String serverJson = "";

// // --- OTA Configuration---
// String serverOta = "";

// // --- Addresses to check ---
// String ips[6] = {"", "", "", "", "", ""};
// String ipsPort[6] = {"", "", "", "", "", ""};
// String ipsName[6] = {"", "", "", "", "", ""};

// --- Config template ---
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
                                   "  ]\n"
                                   "}";

bool stringToIP(const String &str, IPAddress &ip)
{
    return ip.fromString(str);
}

void initLittleFS()
{
    if (!LittleFS.begin())
    {
        Serial.println(F("Blad montowania LittleFS"));
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
        Serial.println(F("no /config.json - create config.json and save template"));
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
        Serial.println(F("config.json - empty or format error, load default template."));
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
        Serial.print(F("JSON format error: "));
        Serial.println(error.c_str());
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
        serviceCheckConfig[i].ips = "";
        serviceCheckConfig[i].ipsName = "-";
        serviceCheckConfig[i].ipsPort = "";
    }
    Serial.println(F("Configuration loaded successfully."));
}

#endif // BOARD_CONFIG_H