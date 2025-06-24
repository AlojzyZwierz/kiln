#include "WebServerManager.h"

// server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//   request->send(200, "text/plain", "Hello World");
// });
/*
server.on("/curve.svg", HTTP_GET, [this](AsyncWebServerRequest *request){
  //Curve& cur = curveManager.getAdjustedCurve();
  String svg = generateSVG( curveManager.getAdjustedCurve());
  request->send(200, "image/svg+xml", svg);
});

server.onNotFound([](AsyncWebServerRequest *request){
  request->send(404, "text/plain", "Not Found");
});
*/

void WebServerManager::begin()
{
  // WiFi.mode(WIFI_STA);
  // WiFi.begin(ssid, password);

  StartWebServer();
}

String WebServerManager::generateSVG(const Curve &curIn)
{

  float currentTemp = temperatureSensor.getTemperature();
  // Serial.println(String(curveManager.getSegmentIndex()) + " endtemp: " + String(curveManager.getAdjustedCurve().elems[curveManager.getSegmentIndex()].endTemp) + " endtemp-1 " + String(curveManager.getSegmentIndex() == 0 ? 0 : curveManager.getAdjustedCurve()elems[curveManager.getSegmentIndex() - 1].endTemp) + " endtime " + String(endTime) + " starttime" + String(startTime) + " millis " + String(millis()) + " a " + String(eL.a, 5) + " b " + String(eL.b) + " ");
  unsigned long totalTime = 0;
  for (int i = 0; i < curveElemsNo; i++)
  {

    if (curIn.elems[i].hTime == 0)
      break;
    /*
    if (curveManager.getAdjustedCurve().elems[i].hTime == 60000 && i == curveManager.getSegmentIndex()) {
      unsigned long lastTemp = (i < 1) ? 2000 : curIn.elems[i - 1].endTemp;

      curIn.elems[i].hTime = (((float)(curIn.elems[i].endTemp - lastTemp) / (float)(currentTemp - lastTemp))) * (millis() - ProcessController::get().getSegmentStartTime());
      Serial.println(String(curIn.elems[i].endTemp - lastTemp) + "  " + String(millis() -  ProcessController::get().getSegmentStartTime()) + " " + String(currentTemp - lastTemp) + " " + String(curIn.elems[i].hTime));
    }*/
    totalTime += curIn.elems[i].hTime;
  }

  float timeRatio = 1400.0f / totalTime;
  float tempRatio = 240 / 1300.0f;
  String svg = "<svg viewBox=\"-10 -10 1630 1310\" preserveAspectRatio=\"none\" >";

  // svg += "<rect x=\"0\" y=\"0\" width=\"1000\" height=\"1300\" fill=\"#fee\" style=\"stroke:grey;stroke-width:8\"/>";

  for (int i = 0; i <= 13; i += 1)
  {
    svg += "<line x1=\"0\" y1=\"" + String(1300 - i * 100) + "\" x2=\"1600\" y2=\"" + String(1300 - i * 100) + "\" style=\"stroke:grey;stroke-width:1\" />";
  }
  // Serial.println("totaltime: " + String(totalTime));
  for (int i = 0; i < (totalTime / 3600000) + 4; i++)
  {
    unsigned long hour = i * 3600000 * timeRatio;
    if (hour > 1600)
      break;
    svg += "<line x1=\"" + String(hour) + "\" y1=\"0\" x2=\"" + String(hour) + "\" y2=\"1300\" style=\"stroke:grey;stroke-width:1\" />";
    svg += "<text x=\"" + String(hour + 5) + "\" y=\"" + String(1295) + "\" fill=\"grey\" font-size=\"20\">" + String(i) + "</text>";
    // Serial.println(String(i) + " hour: " + String(hour) + " timeRatio: " + String(timeRatio) + " x: " + String((totalTime / 3600000)));
  }

  int i = 0;
  int lastX = 0;
  // int lastY = SystemState::get().getMode()==SystemMode::Firing ? (programStartTemp ) : 20;
  // int lastY = SystemState::get().getMode() == SystemMode::Firing ? (ProcessController::get().getProgramStartTemperature()) : 20;
  int lastY = 20;
  totalTime = 0;
  while (i < curveElemsNo)
  {
    if (curIn.elems[i].hTime == 0)
      break;
    int X = (totalTime += curIn.elems[i].hTime) * timeRatio;
    int Y = curIn.elems[i].endTemp;
    if ((curveManager.isSkip(i)))
    {
      svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300 - lastY) + "\" x2=\"" + String(X) + "\" y2=\"" + String(1300 - Y) + "\" style=\"stroke:black;stroke-width:4; stroke-dasharray:10,5\" />";
    }
    else
    {
      svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300 - lastY) + "\" x2=\"" + String(X) + "\" y2=\"" + String(1300 - Y) + "\" style=\"stroke:black;stroke-width:4\" />";
    }
    // svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300 - lastY) + "\" x2=\"" + String(X) + "\" y2=\"" + String(1300 - Y) + "\" stroke:\"black\" stroke-width:\"4\" "+    String((curveManager.getAdjustedCurve()elems[i].hTime == 60000) ? "stroke-dasharray=\"10,5\"" : " ")+"/>";
    if (lastY != Y)
      svg += "<text x=\"" + String(X - 50) + "\" y=\"" + String(1295 - Y) + "\" fill=\"black\" font-size=\"30\">" + String(Y) + "</text>";
    lastX = X;
    lastY = Y;
    i++;
  }
  svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300 - lastY) + "\" x2=\"1600\" y2=\"1280\" style=\"stroke:blue;stroke-width:2; stroke-dasharray:10,5\" />";

  svg += "<text x=\"" + String((SystemState::get().getMode() == SystemMode::Firing) ? ((millis() - ProcessController::get().getProgramStartTime() + ProcessController::get().getStartTimeOffset()) * timeRatio + 5) : 10) + "\" y=\"" + String((1300 - currentTemp) + 20) + "\" fill=\"black\" font-size=\"30\">" + String((float)currentTemp, 1) + "</text>";

  // The HTTP response ends with another blank line
  i = 0;
  Measurement last(ProcessController::get().getStartTimeOffset(), ProcessController::get().getProgramStartTemperature());
  for (const Measurement &m : MeasurementManager::get().getMeasurements())
  {

    if (i != 0)
    {
      svg += "<line x1=\"" + String((last.time + ProcessController::get().getStartTimeOffset() / 1000) * timeRatio * 1000) + "\" y1=\"" + String(1300 - last.temp) + "\" x2=\"" + String((m.time + ProcessController::get().getStartTimeOffset() / 1000) * timeRatio * 1000) + "\" y2=\"" + String(1300 - m.temp) + "\" style=\"stroke:orange;stroke-width:2;\"></line>";
      // svg += "<line x1=\"100\" y1=\"100\" x2=\"" + String(m.time * timeRatio) + "\" y2=\"" + String(1300 - m.temp / 100) + "\" style=\"stroke:\"orange\";stroke-width:2\" stroke-linecap=\"round\"/>";

      svg += "<circle r=\"2\" cx=\"" + String((m.time + ProcessController::get().getStartTimeOffset() / 1000) * timeRatio * 1000) + "\" cy=\"" + String(1300 - m.temp) + "\" fill=\"red\"></circle>";
    }
    last = m;
    i++;
    // svg += "<circle r=\"4\" cx=\""+ String((totalTime + millis() - startTime)/timeRatio)+ "\" cy=\""+ String(1300 - currentTemp/100)+ "\" fill=\"yellow\" />";
  }
  if (SystemState::get().getMode() == SystemMode::Firing)
  {
    svg += "<line x1=\"" + String((last.time + ProcessController::get().getStartTimeOffset() / 1000) * timeRatio * 1000) + "\" y1=\"" + String(1300 - last.temp) + "\" x2=\"" + String((millis() - ProcessController::get().getProgramStartTime() + ProcessController::get().getStartTimeOffset()) * timeRatio) + "\" y2=\"" + String(1300 - currentTemp) + "\" style=\"stroke:orange;stroke-width:2;\"></line>";
    // svg += "<text x=\"100\" y=\"100\" fill=\"black\" font-size=\"30\">" + String(ratio, 2) + "</text>";
  }
  svg += "<circle r=\"5\" cx=\"" + String((SystemState::get().getMode() == SystemMode::Firing && (MeasurementManager::get().getMeasurements().size() != 0)) ? ((millis() - ProcessController::get().getProgramStartTime() + ProcessController::get().getStartTimeOffset()) * timeRatio) : 5) + "\" cy=\"" + String(1300 - currentTemp) + "\" fill=\"red\"></circle>";
  svg += "<text x=\"40\" y=\"80\" fill=\"black\" font-size=\"70\">" + String((float)currentTemp, 1) + "</text>";
  svg += "<text x=\"40\" y=\"125\" fill=\"black\" font-size=\"40\">" + String(curveManager.getcurrentCurveIndex()) + " " + String(curveManager.getSegmentIndex() + 1) + "</text>";
  svg += "</svg></div></body></html>";

  return svg;
}

void WebServerManager::StartWebServer()
{
  wifiCreds = StorageManager::loadWiFiCredentials(); // <-- funkcja zależna od wybranego formatu

  WiFi.begin(wifiCreds.ssid, wifiCreds.password);

  unsigned long startAttemptTime = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000)
  {
    Serial.print(".");
    tft.print(".");
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("\nPołączono z WiFi");
    Serial.print("Adres IP: ");
    Serial.println(WiFi.localIP());
  }
  else
  {
    Serial.println("\nNie udało się połączyć, uruchamiam tryb AP...");
    WiFi.softAP(apSSID, apPassword);
    Serial.print("Adres IP AP: ");
    Serial.println(WiFi.softAPIP());
  }

  // Serwowanie strony HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(200, "text/html", R"rawliteral(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>KILN</title>
  <style>
    body {
      display: flex;
      flex-direction: column;
      height: 100vh;
      margin: 0;
    }
    .svg-container {
      flex-grow: 1;
      width: 100%;
      position: relative;
    }
    svg {
      position: absolute;
      width: 100%;
      height: 100%;
    }
    #settingsBtn {
      position: absolute;
      top: 20px;
      right: 20px;
      z-index: 10;
      padding: 6px 10px;
      font-size: 16px;
      background-color: rgba(255, 255, 255, 0.8);
      border: 1px solid #ccc;
      border-radius: 4px;
      cursor: pointer;
    }
      #measurementsBtn {
      position: absolute;
      bottom: 50px;
      left: 200px;
      z-index: 10;
      padding: 6px 10px;
      font-size: 16px;
      background-color: rgba(255, 255, 255, 0.8);
      border: 1px solid #ccc;
      border-radius: 4px;
      cursor: pointer;
    }
  </style>
  <script>
    function updateChart() {
      fetch('/svg')
        .then(response => response.text())
        .then(data => {
          document.getElementById('chart').innerHTML = data;
        });
    }
    setInterval(updateChart, 3000); // Odświeżanie co 3 sekundy
  </script>
</head>
<body>
  <div class="svg-container">
    <div id="chart"></div>
    <a id="settingsBtn" href="/settings">wifi password</a>
    <a id="measurementsBtn" href="/getMeasurements">measurements</a>
  </div>
  <script> updateChart(); </script>
</body>
</html>
)rawliteral"); });
  // Endpoint zwracający dynamiczny SVG
  server.on("/svg", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
              request->send(200, "image/svg+xml", generateSVG(curveManager.getAdjustedCurve()));
              // request->send(200, "image/svg+xml", generateSVG((ON || (cooling && measurements.size()!=0))? graphCurve : cur));
            });

  server.on("/getMeasurements", HTTP_GET, [this](AsyncWebServerRequest *request)
            {
  String output = "Time[s]  Temp[°C]\n";
  output += "--------\n";
  
  for (const auto& m : MeasurementManager::get().getMeasurements() ) {
    output += String(m.time) + "\t" + String(m.temp) + "\n";
  }
  
  output += "\nmeasurements count: " + String(MeasurementManager::get().getMeasurements().size());
  request->send(200, "text/html", output); });

  server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  String html = R"rawliteral(
    <html>
    <head><title>WiFi</title></head>
    <body>
      <h1>WiFi</h1>
      <form action="/save" method="POST">
        SSID: <input type="text" name="ssid"><br>
        passWD: <input type="password" name="password"><br>
        <input type="submit" value="Save">
      </form>
      <a href="/">Back</a>
    </body>
    </html>
  )rawliteral";
  request->send(200, "text/html", html); });
  server.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request)
            {
  if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();

    ssid.toCharArray(this->wifiCreds.ssid, sizeof(wifiCreds.ssid));
    password.toCharArray(this->wifiCreds.password, sizeof(wifiCreds.password));

    StorageManager::saveWiFiCredentials(this->wifiCreds);  // <-- funkcja zależna od wybranego formatu

    request->send(200, "text/html", "<p>data saved</p><a href='/'>Back</a>");


    
    // Opcjonalnie: restart WiFi po zapisaniu
    WiFi.disconnect(true);
    delay(1000);
    WiFi.begin(wifiCreds.ssid, wifiCreds.password);
  } else {
    request->send(400, "text/plain", "insufficient parameters");
  } });
  server.begin();
}
