#pragma once

const int LOG_SIZE = 20;
static String logs[LOG_SIZE];
static int logIndex = 0;
static int logCount = 0;
static bool _guiLogsEnabled = false;

void initLog(bool enableGuiLogs)
{
  _guiLogsEnabled = enableGuiLogs;

  if (_guiLogsEnabled)
  {
    Serial.println("BOARD_LOGGING - GUI Logs ENABLED (Size: 20)");
  }
  else
  {
    Serial.println("BOARD_LOGGING - GUI Logs DISABLED (Only Serial)");
  }
}

void addLog(const String &entry)
{
  Serial.println(entry);
  if (_guiLogsEnabled)
  {
    logs[logIndex] = entry;
    logIndex = (logIndex + 1) % LOG_SIZE;
    if (logCount < LOG_SIZE)
    {
      logCount++;
    }
  }
}