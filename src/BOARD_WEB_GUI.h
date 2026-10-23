#pragma once

extern bool configSaved;
extern String appVersion;

typedef void (*ModuleGuiRenderer)(); // Typ: Wskaźnik na funkcję rysującą
ModuleGuiRenderer guiRenderers[15];  // Tablica pomieści maksymalnie 15 modułów (zużywa tylko kilkadziesiąt bajtów RAM!)
int registeredModulesCount = 0;

void registerModuleGui(ModuleGuiRenderer renderer)
{
  if (registeredModulesCount < 15)
  {
    guiRenderers[registeredModulesCount] = renderer;
    registeredModulesCount++;
  }
}

class WebGui
{
public:
  const String SEP_START = "<sep>";
  const String SEP_END = "</sep>";
  const String END_LINE = "</br>\n";
  const String HTML_BEGIN = "<!DOCTYPE html><html><head><link rel='icon' href='data:,'>\n<style>\n"
                            "html { font-family: Helvetica; font-size: 25px; text-align: center; background-color: #f2f2f2;}\n"
                            "input[type=text], input[type=number] { font-size: 20px; width:100%; padding: 12px 20px; display: inline-block; border: 1px solid #ccc; border-radius: 4px; box-sizing: border-box}\n"
                            ".submit, button { font-size: 20px; width: 100%; color: white; padding: 14px 20px; border: none; border-radius: 4px; cursor: pointer}\n"
                            ".submit {background-color: #4CAF50;}\n"
                            ".button {background-color: limegreen;}\n"
                            ".button2 {background-color: tomato;}\n"
                            "textarea { width: 100%; max-width: 800px; font-family: monospace; white-space: pre; }\n"
                            ".help-box { background-color: #e8f5e9; border-left: 4px solid #4CAF50; padding: 10px; margin-bottom: 20px; font-size: 14px; text-align: left; }\n"
                            "table {width: 80%;  margin: auto;}\n"
                            "th, td {width: 50%; padding: 8px; text-align: left; 1px solid #ddd;}\n"
                            ".button-row {display: flex; justify-content: center; gap: 10px; margin-bottom: 20px;}\n"
                            "</style>\n</head>\n<body></br>\n"
                            "<div class='button-group button-row'>"
                            "<a href='/'><button class='button'>Home</button></a>"
                            "<a href='/logs'><button class='button'>Logs</button></a>"
                            "<a href='/status'><button class='button'>Status</button></a>"
                            "<a href='/restart'><button class='button2'>restart</button></a>"
                            "</div>";
  const String HTML_END = "<script>"
                          "document.querySelectorAll('.json-form').forEach(form => {"
                          "form.addEventListener('submit', function(event) {"
                          "  event.preventDefault();"
                          "  const formData = new FormData(form);"
                          "  const jsonData = {};"
                          "  for (const [key, value] of formData.entries()) {"
                          "    jsonData[key] = value;"
                          "  }"
                          "  fetch('/json', {"
                          "    method: 'POST',"
                          "    headers: {"
                          "      'Content-Type': 'application/json'"
                          "    },"
                          "    body: JSON.stringify(jsonData)"
                          "  })"
                          "  .then(response => response.json())"
                          "  .then(data => {"
                          "    console.log('Sukces:', data);"
                          "    alert('JSON sent successfully!');"
                          "    refreshLogs();" // <-- Refresh logs after success
                          "  })"
                          "  .catch(error => {"
                          "    console.error('Błąd:', error);"
                          "    alert('Error sending JSON: ' + error.message);"
                          "    refreshLogs();" // <-- Refresh logs after error
                          "  });"
                          "});"
                          "});"
                          "function refreshLogs() {"
                          "  fetch('/logs')"
                          "    .then(response => response.text())"
                          "    .then(data => {"
                          "      document.getElementById('logs').value = data;"
                          "    })"
                          "    .catch(error => {"
                          "      console.error('Error fetching logs:', error);"
                          "    });"
                          "}"
                          "</script>"
                          "</body>\n</html>\n";
  const String RESULT_LOG_END = "</body>\n<a href='javascript:history.back()'><button class='button'>Go Back</button></a>";
  const String HTML_ERROR = "\n";

  void streamWebPage()
  {
    server.sendContent(HTML_BEGIN);
    server.sendContent("<h1>" + deviceConfig.deviceName + "</h1>");
    server.sendContent(END_LINE);

    server.sendContent("<div class='container'><textarea id='logs' readonly name='logs' rows='11' cols='120'>");

    // --- POCZĄTEK STRUMIENIOWANIA LOGÓW ---
    int start = (logIndex + LOG_SIZE - logCount) % LOG_SIZE;
    for (int i = 0; i < logCount; i++)
    {
      int idx = (start + i) % LOG_SIZE;
      if (logs[idx].length() > 0)
      {
        // Wysyłamy pojedynczy log z tablicy + znak nowej linii HTML (&#13;&#10;)
        server.sendContent(logs[idx] + "&#13;&#10;");
      }
    }
    // --- KONIEC STRUMIENIOWANIA LOGÓW ---

    server.sendContent("</textarea></br>\n</div>");

    for (int i = 0; i < registeredModulesCount; i++)
    {
      guiRenderers[i]();
    }

    server.sendContent("<h6>APP Version: ");
    server.sendContent(appVersion);
    server.sendContent("</h6>");
    server.sendContent(HTML_END);
  }

  void streamConfigPage(String configContent)
  {
    server.sendContent(HTML_BEGIN);
    server.sendContent("<h2>ESP Configuration (Raw)</h2>\n");
    server.sendContent(
        "<div class='help-box'>\n"
        "  <strong>File structure example:</strong><br>\n"
        "{\n<br>"
        "  \"wifi\": {\n<br>"
        "    \"ssid\": \"\",\n<br>"
        "    \"password\": \"\",\n<br>"
        "    \"localIp\": \"192.168.0.199\"\n<br>"
        "  },\n<br>"
        "  \"pushover\": {\n<br>"
        "    \"userKey\": \"\",\n<br>"
        "    \"apiToken\": \"\"\n<br>"
        "  },\n<br>"
        "  \"serverJson\": \"http://192.168.0.242:5000/api/addEvent\",\n<br>"
        "  \"serverOta\": \"\",\n<br>"
        "  \"deviceList\": [\n<br>"
        "    {\n<br>"
        "      \"name\": \"Router\",\n<br>"
        "      \"ip\": \"192.168.0.1\"\n<br>"
        "    },\n<br>"
        "    {\n<br>"
        "      \"name\": \"Google DNS (Test)\",\n<br>"
        "      \"ip\": \"8.8.8.8\"\n<br>"
        "    }\n<br>"
        "  ]<br>\n"
        "}\n"
        "</div>\n");

    server.sendContent("<form action='/save' method='POST'>\n");
    server.sendContent("<textarea name='configRaw' rows='25'>\n");
    server.sendContent(configContent);
    server.sendContent("\n</textarea><br><br>\n");
    server.sendContent("<input type='submit' class='submit' value='Save and Reboot'>\n");
    server.sendContent("</form>\n");

    server.sendContent(HTML_END);
  }

  String hHtml(String text)
  {
    return "<h1>" + text + "</h1>\n";
  }

  String pHtml(String text)
  {
    return "<h3>" + text + "</h3>\n";
  }

  String formBegin()
  {
    return "<form class='json-form' action = json >\n<table>\n";
  }

  String formText(String label, String name, String value)
  {
    return "<tr><td>" + label + "</td><td><input type='text' name='" + name + "' value='" + value + "'></td></tr>\n";
  }

  String formNumber(String label, String name, String value)
  {
    return "<tr><td>" + label + "</td><td><input type='number' name='" + name + "' value='" + value + "'></td></tr>\n";
  }

  String formHidden(String label, String name, String value)
  {
    return "<input type='hidden' name='" + name + "' value='" + value + "'>\n";
  }

  String formEnd(String value)
  {
    return "<tr><td colspan='2'><input type = 'submit' class='submit' value = '" + value + "'></td></tr>\n</table>\n</form>\n";
  }

  String htmlButton(String label, String name, String value)
  {
    return "<table><tr><td>" + label + "</td><td><a href='" + name + "'><button class='button'>" + value + "</button></a></table>\n";
  }

  String htmlButton2(String label, String name, String value)
  {
    return "<table><tr><td>" + label + "</td><td><a href='" + name + "'><button class='button2'>" + value + "</button></a></table>\n";
  }

  String escapeHtml(String text)
  {
    text.replace("&", "&amp;");
    text.replace("<", "&lt;");
    text.replace(">", "&gt;");
    text.replace("\"", "&quot;");
    return text;
  }
};

static WebGui webGui;

void handleRoot()
{
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  server.send(200, "text/html", "");

  if (configSaved)
  {
    webGui.streamWebPage();
  }
  else
  {
    String configContent = readRawConfig();
    webGui.streamConfigPage(configContent);
  }

  server.sendContent("");
}

void handleSave()
{
  if (!server.hasArg("configRaw"))
  {
    server.send(400, "text/plain", "No configuration data!");
    return;
  }

  if (!saveRawConfig(server.arg("configRaw")))
  {
    server.send(500, "text/plain", "Error saving to flash memory");
    return;
  }

  server.send(200, "text/html", "<h2>Configuration saved! ESP will reboot now.</h2>");

  delay(1000);
  ESP.restart();
}

void initWebGUI()
{
  server.on("/", handleRoot);
  server.on("/config", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.begin();
  Serial.println(F("Web GUI Started"));
}