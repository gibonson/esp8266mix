// Wi-Fi and HTTP libraries
#include <ESP8266WiFi.h>       // Load Wi-Fi library
#include <ESP8266HTTPClient.h> // HTTP client for ESP8266
#include <ESP8266WebServer.h>  // Dedykowany serwer HTTP

// Wi-Fi and HTTP configuration
#define CONFIG_TIMEOUT 5000           // Data waiting time
String header;                        // Variable to store the HTTP request
HTTPClient http;                      // HTTP client instance
WiFiClient client;                    // Wi-Fi client instance
unsigned long currentTime = millis(); // Current time
unsigned long previousTime = 0;       // Previous time
const long timeoutTime = 500;         // Timeout time in milliseconds

// Server configuration
ESP8266WebServer server(80);       // Set web server port number to 80
IPAddress gateway(192, 168, 1, 1); // Set your Gateway IP address
IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
IPAddress local_IP;

void init_wifi()
{
  if (local_IP.fromString(deviceConfig.deviceIP))
  {
    Serial.println("Skonwertowany IP: " + local_IP.toString());
  }
  else
  {
    Serial.println("Błąd konwersji IP.");
  }

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  else
  {
    Serial.println("Wifi configuration - OK");
  }

  Serial.println("Connecting to " + String(deviceConfig.ssid));
  WiFi.begin(deviceConfig.ssid, deviceConfig.password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nWiFi connected. IP address: "); // Print local IP address and start web server
  Serial.println(WiFi.localIP());                 // Print local IP address and start web server
  Serial.println("");
  server.begin();
}
