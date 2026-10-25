#pragma once

int pingFailCounter[6] = {0, 0, 0, 0, 0, 0};
String oledBuffer[7] = {"", "", "", "", "", "", ""};

unsigned long previousMillis = 0;
int currentHostIndex = 0;
int currentPhase = 0;

void app_setup()
{
        addLog("APP_WATCHDOG");
}

void app_loop()
{

    unsigned long currentMillis = millis();

    if (currentPhase == 0)
    {
        String wifiStatus = wifiConnectionStatus();
        if (serviceCheckConfig[currentHostIndex].ips == "")
        {
            oledBuffer[currentHostIndex] = "-  | ---";
            updateOled("SSID: " + String(deviceConfig.ssid), wifiStatus + " dBm=" + String(WiFi.RSSI()),
                       oledBuffer[0], oledBuffer[1], oledBuffer[2], oledBuffer[3], oledBuffer[4], oledBuffer[5]);
        }
        else
        {
            bool ret = false;
            String IPstatus = "  ";

            if (serviceCheckConfig[currentHostIndex].ipsPort == "")
            {
                oledBuffer[currentHostIndex] = "\x10  |" + serviceCheckConfig[currentHostIndex].ips;
                updateOled("SSID: " + String(deviceConfig.ssid), wifiStatus + " dBm=" + String(WiFi.RSSI()),
                           oledBuffer[0], oledBuffer[1], oledBuffer[2], oledBuffer[3], oledBuffer[4], oledBuffer[5]);
                ret = Ping.ping(serviceCheckConfig[currentHostIndex].ips.c_str());
            }
            else
            {
                oledBuffer[currentHostIndex] = ("\x10  |" + serviceCheckConfig[currentHostIndex].ips + ":" + serviceCheckConfig[currentHostIndex].ipsPort).substring(0, 22);
                updateOled("SSID: " + String(deviceConfig.ssid), wifiStatus + " dBm=" + String(WiFi.RSSI()),
                           oledBuffer[0], oledBuffer[1], oledBuffer[2], oledBuffer[3], oledBuffer[4], oledBuffer[5]);
                WiFiClient client;
                ret = client.connect(serviceCheckConfig[currentHostIndex].ips, serviceCheckConfig[currentHostIndex].ipsPort.toInt());
                client.stop();
            }

            if (ret == false)
            {
                if (pingFailCounter[currentHostIndex] <= 99)
                {
                    pingFailCounter[currentHostIndex]++;
                    IPstatus = String(pingFailCounter[currentHostIndex]);
                    while (IPstatus.length() < 3)
                    {
                        IPstatus = IPstatus + " ";
                    }
                }
                else
                {
                    IPstatus = "OFF";
                }
                if (pingFailCounter[currentHostIndex] == 5 || pingFailCounter[currentHostIndex] == 50 || pingFailCounter[currentHostIndex] == 99)
                {
                    sendPushover(serviceCheckConfig[currentHostIndex].ipsName + " - " + serviceCheckConfig[currentHostIndex].ips + " - connection error! Attempt: " + String(pingFailCounter[currentHostIndex]));
                }
            }
            else
            {
                IPstatus = "OK ";
                if (pingFailCounter[currentHostIndex] >= 5)
                {
                    sendPushover(serviceCheckConfig[currentHostIndex].ipsName + " - " + serviceCheckConfig[currentHostIndex].ips + " - back online!");
                }
                pingFailCounter[currentHostIndex] = 0;
            }
            oledBuffer[currentHostIndex] = IPstatus + "|" + serviceCheckConfig[currentHostIndex].ipsName;
        }
        currentPhase = 1;
    }

    else if (currentPhase == 1)
    {
        if (currentMillis - previousMillis >= 4000)
        {
            String wifiStatus = wifiConnectionStatus();
            updateOled("SSID: " + String(deviceConfig.ssid), wifiStatus + " dBm=" + String(WiFi.RSSI()),
                       oledBuffer[0], oledBuffer[1], oledBuffer[2], oledBuffer[3], oledBuffer[4], oledBuffer[5]);
            previousMillis = currentMillis;
            currentPhase = 2;
        }
    }

    else if (currentPhase == 2)
    {
        if (currentMillis - previousMillis >= 4000)
        {
            currentHostIndex++;
            if (currentHostIndex >= 6)
                currentHostIndex = 0;
            previousMillis = millis();
            currentPhase = 0;
        }
    }
}