#pragma once

void sysLog(String addInfo, int value, String type, String requestID = "ESP_EVENT", bool saveToLog = true, bool sendToServer = false, bool httpResponse = false)
{
    String message = "SYS_LOG - " + addInfo + " | " + String(value) + " | " + type + " | REQ:" + requestID;
    if (saveToLog)
    {
        addLog(message);
    }

    if (sendToServer)
    {
        sendJson(addInfo, value, type, requestID);
    }

    if (httpResponse)
    {
        responseJson(addInfo, value, type, requestID);
    }

    // if (sendToPushover)
    // {
    //     sendPushover(message);
    // }
}