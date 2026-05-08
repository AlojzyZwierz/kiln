#include "WebServerManager.h"

// ─────────────────────────────────────────────────────────────────────────────
//  WiFi event handling (lambda z capturem — bez statycznego singletona)
// ─────────────────────────────────────────────────────────────────────────────

void WebServerManager::registerWiFiEvents()
{
    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info)
    {
        switch (event)
        {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            _staConnected = true;
            Serial.printf("[WiFi] Połączono. IP: %s\n", WiFi.localIP().toString().c_str());
            // Wyłącz AP jeśli nikt na nim nie siedzi
            if (_apActive && WiFi.softAPgetStationNum() == 0)
            {
                WiFi.softAPdisconnect(true);
                WiFi.mode(WIFI_STA);
                _apActive = false;
                Serial.println("[WiFi] AP wyłączone.");
            }
            break;

        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            _staConnected = false;
            Serial.println("[WiFi] Rozłączono.");
            // Jeśli AP nie jest aktywne — odpal je jako fallback
            if (!_apActive)
                startAP();
            break;

        case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
            Serial.println("[AP] Klient podłączony.");
            break;

        case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
            Serial.println("[AP] Klient odłączony.");
            break;

        default:
            break;
        }
    });
}

// ─────────────────────────────────────────────────────────────────────────────
//  Pomocnicze
// ─────────────────────────────────────────────────────────────────────────────

void WebServerManager::startAP()
{
    // WIFI_AP_STA: AP dla klientów + STA może próbować w tle
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(apSSID, apPassword);
    _apActive = true;
    Serial.printf("[AP] Uruchomione. SSID: %s  IP: %s\n", apSSID, WiFi.softAPIP().toString().c_str());
}

void WebServerManager::tryConnect()
{
    if (strlen(wifiCreds.ssid) == 0)
        return;
    Serial.printf("[WiFi] Próba połączenia z: %s\n", wifiCreds.ssid);
    WiFi.begin(wifiCreds.ssid, wifiCreds.password);
    // wynik przyjdzie przez event — tu nie blokujemy
}

// ─────────────────────────────────────────────────────────────────────────────
//  begin() — wywołać raz w setup()
// ─────────────────────────────────────────────────────────────────────────────

void WebServerManager::begin()
{
    wifiCreds = StorageManager::loadWiFiCredentials();

    registerWiFiEvents();

    if (strlen(wifiCreds.ssid) > 0)
    {
        // Tryb STA + AP jednocześnie od razu — AP startuje jako fallback,
        // a STA próbuje się połączyć. Gdy STA dostanie IP, AP zostanie wyłączone.
        startAP();
        tryConnect();

        // Blokujemy maks. 10s tylko po to żeby móc wyświetlić IP na TFT
        tft.print("Łączenie WiFi");
        unsigned long t = millis();
        while (!_staConnected && millis() - t < 10000)
        {
            delay(200);
            tft.print(".");
        }
        tft.println(_staConnected ? "\nWiFi OK" : "\nAP mode");
    }
    else
    {
        // Brak zapisanych danych — od razu AP
        startAP();
    }

    setupRoutes();
    server.begin();
    Serial.println("[Server] Uruchomiony.");
}

// ─────────────────────────────────────────────────────────────────────────────
//  handleClient() — wywołać z loop(), prawie nic nie robi
// ─────────────────────────────────────────────────────────────────────────────

void WebServerManager::handleClient()
{
    if (_staConnected)
        return; // wszystko gra
    if (strlen(wifiCreds.ssid) == 0)
        return; // nie ma czego próbować

    // Próbuj reconnect tylko gdy nikt nie jest podłączony do AP
    if (_apActive && WiFi.softAPgetStationNum() > 0)
        return;

    unsigned long now = millis();
    if (now - _lastReconnectAt >= RECONNECT_INTERVAL)
    {
        _lastReconnectAt = now;
        WiFi.disconnect(false);   // rozłącz bez kasowania danych
        tryConnect();
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Trasy HTTP — wydzielone żeby begin() był czytelny
// ─────────────────────────────────────────────────────────────────────────────

void WebServerManager::setupRoutes()
{
    // Strona główna
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/html", R"rawliteral(
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>KILN</title>
  <style>
    body { display:flex; flex-direction:column; height:100vh; margin:0; }
    .svg-container { flex-grow:1; width:100%; position:relative; }
    svg { position:absolute; width:100%; height:100%; }
    #settingsBtn, #measurementsBtn {
      position:absolute; right:20px; z-index:10;
      padding:6px 10px; font-size:16px;
      background-color:rgba(255,255,255,0.8);
      border:1px solid #ccc; border-radius:4px; cursor:pointer;
    }
    #settingsBtn    { bottom:20px; }
    #measurementsBtn{ bottom:55px; }
  </style>
  <script>
    function updateChart() {
      fetch('/svg')
        .then(r => r.text())
        .then(d => document.getElementById('chart').innerHTML = d);
    }
    setInterval(updateChart, 3000);
  </script>
</head>
<body>
  <div class="svg-container">
    <div id="chart"></div>
    <a id="settingsBtn"     href="/settings">wifi</a>
    <a id="measurementsBtn" href="/getMeasurements">measurements</a>
  </div>
  <script>updateChart();</script>
</body>
</html>)rawliteral");
    });

    // Dynamiczny SVG
    server.on("/svg", HTTP_GET, [this](AsyncWebServerRequest *request)
    {
        request->send(200, "image/svg+xml", generateSVG(curveManager.getAdjustedCurve()));
    });

    // Pomiary jako tekst
    server.on("/getMeasurements", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        String out = "Time[s]\tTemp[°C]\n--------\n";
        for (const auto &m : MeasurementManager::get().getMeasurements())
            out += String(m.time) + "\t" + String(m.temp) + "\n";
        out += "\ncount: " + String(MeasurementManager::get().getMeasurements().size());
        request->send(200, "text/plain", out);
    });

    // Formularz WiFi
    server.on("/settings", HTTP_GET, [](AsyncWebServerRequest *request)
    {
        request->send(200, "text/html", R"rawliteral(
<html><head><title>WiFi</title></head>
<body>
  <h1>WiFi</h1>
  <form action="/save" method="POST">
    SSID: <input type="text" name="ssid"><br>
    Hasło: <input type="password" name="password"><br>
    <input type="submit" value="Zapisz">
  </form>
  <a href="/">Wróć</a>
</body></html>)rawliteral");
    });

    // Zapis credentials i reconnect
    server.on("/save", HTTP_POST, [this](AsyncWebServerRequest *request)
    {
        if (!request->hasParam("ssid", true) || !request->hasParam("password", true))
        {
            request->send(400, "text/plain", "Brak parametrów");
            return;
        }

        String ssid     = request->getParam("ssid", true)->value();
        String password = request->getParam("password", true)->value();

        ssid.toCharArray(wifiCreds.ssid,     sizeof(wifiCreds.ssid));
        password.toCharArray(wifiCreds.password, sizeof(wifiCreds.password));
        StorageManager::saveWiFiCredentials(wifiCreds);

        request->send(200, "text/html", "<p>Zapisano.</p><a href='/'>Wróć</a>");

        // Próba połączenia z nowym AP — nieblokująca
        _staConnected = false;
        _lastReconnectAt = 0;   // wymuś próbę przy najbliższym handleClient()
        WiFi.disconnect(false);
        tryConnect();
    });
}

// ─────────────────────────────────────────────────────────────────────────────
//  SVG — bez zmian merytorycznych, tylko przeniesione
// ─────────────────────────────────────────────────────────────────────────────

String WebServerManager::generateSVG(const Curve &curIn)
{
    float currentTemp = temperatureSensor.getTemperature();

    unsigned long totalTime = 0;
    for (int i = 0; i < curveElemsNo; i++)
    {
        if (curIn.elems[i].hTime == 0) break;
        totalTime += curIn.elems[i].hTime;
    }

    float timeRatio = 1400.0f / totalTime;
    float tempRatio = 240 / 1300.0f;

    String svg = "<svg viewBox=\"-10 -10 1630 1310\" preserveAspectRatio=\"none\">";

    // Linie poziome (co 100°C)
    for (int i = 0; i <= 13; i++)
        svg += "<line x1=\"0\" y1=\"" + String(1300 - i*100) + "\" x2=\"1600\" y2=\"" + String(1300 - i*100) + "\" style=\"stroke:grey;stroke-width:1\"/>";

    // Linie godzinowe
    for (int i = 0; i < (int)(totalTime / 3600000) + 4; i++)
    {
        unsigned long hour = (unsigned long)(i * 3600000UL * timeRatio);
        if (hour > 1600) break;
        svg += "<line x1=\"" + String(hour) + "\" y1=\"0\" x2=\"" + String(hour) + "\" y2=\"1300\" style=\"stroke:grey;stroke-width:1\"/>";
        svg += "<text x=\"" + String(hour+5) + "\" y=\"1295\" fill=\"grey\" font-size=\"20\">" + String(i) + "</text>";
    }

    // Krzywa programowa
    int   lastX    = 0;
    int   lastY    = 20;
    totalTime      = 0;
    for (int i = 0; i < curveElemsNo; i++)
    {
        if (curIn.elems[i].hTime == 0) break;
        int  X              = (int)((totalTime += curIn.elems[i].hTime) * timeRatio);
        int  Y              = curIn.elems[i].endTemp;
        bool prevDescending = i > 0 && curIn.elems[i-1].endTemp > Y;
        String dash         = curveManager.isSkip(i) ? "stroke-dasharray:10,5;" : "";

        svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300-lastY) +
               "\" x2=\""   + String(X)      + "\" y2=\"" + String(1300-Y)     +
               "\" style=\"stroke:black;stroke-width:4;" + dash + "\"/>";

        if (lastY != Y)
            svg += "<text x=\"" + String(X) + "\" y=\"" + String(1298-Y) +
                   "\" fill=\"black\" font-size=\"25\" " +
                   String(prevDescending
                       ? "text-anchor=\"start\" dominant-baseline=\"text-after-edge\""
                       : "text-anchor=\"end\"   dominant-baseline=\"text-after-edge\"") +
                   ">" + String(Y) + "</text>";
        lastX = X;
        lastY = Y;
    }

    // Linia do końca (przewidywanie)
    svg += "<line x1=\"" + String(lastX) + "\" y1=\"" + String(1300-lastY) +
           "\" x2=\"1600\" y2=\"1280\" style=\"stroke:blue;stroke-width:2;stroke-dasharray:10,5\"/>";

    // Aktualna temperatura — etykieta
    bool  firing    = SystemState::get().getMode() == SystemMode::Firing;
    float elapsed   = firing ? (millis() - ProcessController::get().getProgramStartTime()
                                + ProcessController::get().getStartTimeOffset()) : 0;
    svg += "<text x=\"" + String(firing ? (int)(elapsed * timeRatio + 5) : 10) +
           "\" y=\"" + String((int)(1300 - currentTemp) + 20) +
           "\" fill=\"black\" font-size=\"30\">" + String(currentTemp, 1) + "</text>";

    // Pomiary (linia pomarańczowa)
    const auto &measurements = MeasurementManager::get().getMeasurements();
    if (measurements.size() > 0)
    {
        int         mi   = 0;
        float       tRat = timeRatio * 1000.0f;
        long        off  = ProcessController::get().getStartTimeOffset();
        Measurement last(off, ProcessController::get().getProgramStartTemperature());

        for (const Measurement &m : measurements)
        {
            if (mi != 0)
            {
                int lx = (int)((last.time + off/1000) * tRat);
                int ly = 1300 - (int)last.temp;
                int mx = (int)((m.time    + off/1000) * tRat);
                int my = 1300 - (int)m.temp;
                svg += "<line x1=\"" + String(lx) + "\" y1=\"" + String(ly) +
                       "\" x2=\""   + String(mx) + "\" y2=\"" + String(my) +
                       "\" style=\"stroke:orange;stroke-width:2\"/>";
                svg += "<circle r=\"2\" cx=\"" + String(mx) + "\" cy=\"" + String(my) + "\" fill=\"red\"/>";
            }
            last = m;
            mi++;
        }

        if (firing)
        {
            int lx = (int)((last.time + off/1000) * tRat);
            int ly = 1300 - (int)last.temp;
            int cx = (int)(elapsed * timeRatio);
            int cy = 1300 - (int)currentTemp;
            svg += "<line x1=\"" + String(lx) + "\" y1=\"" + String(ly) +
                   "\" x2=\""   + String(cx) + "\" y2=\"" + String(cy) +
                   "\" style=\"stroke:orange;stroke-width:2\"/>";
            svg += "<circle r=\"15\" cx=\"25\" cy=\"35\" fill=\"red\"/>"; // wskaźnik grzania
        }

        int dotX = (firing && measurements.size() > 0) ? (int)(elapsed * timeRatio) : 5;
        svg += "<circle r=\"5\" cx=\"" + String(dotX) + "\" cy=\"" + String(1300-(int)currentTemp) + "\" fill=\"red\"/>";
    }

    // HUD
    svg += "<text x=\"40\" y=\"80\"  fill=\"black\" font-size=\"70\">" + String(currentTemp, 1) + "</text>";
    svg += "<text x=\"40\" y=\"125\" fill=\"black\" font-size=\"40\">"
         + String(curveManager.getcurrentCurveIndex()) + "."
         + String(curveManager.getSegmentIndex() + 1) + "</text>";
    svg += "</svg></div></body></html>";

    return svg;
}