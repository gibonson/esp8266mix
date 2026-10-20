#pragma once

const int LOG_SIZE = 10;
String logs[LOG_SIZE];
int logIndex = 0;
int logCount = 0;


void addLog(const String &entry) // Używamy referencji (&), żeby nie kopiować niepotrzebnie JSONów w pamięci
{
  logs[logIndex] = entry;
  logIndex = (logIndex + 1) % LOG_SIZE;
  if (logCount < LOG_SIZE)
    logCount++;
}