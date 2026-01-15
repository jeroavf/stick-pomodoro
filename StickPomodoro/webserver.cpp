#include "webserver.h"
#include "config.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

// =============================================================================
// Web Server Module Implementation
// =============================================================================

static WebServer server(80);
static bool serverActive = false;
static char apSSID[20] = "";
static char apIP[16] = "192.168.4.1";

static Settings* pSettings = nullptr;
static TimerData* pTimer = nullptr;

// HTML pages stored in PROGMEM
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Stick Pomodoro</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: -apple-system, sans-serif; background: #1a1a1a; color: #e0e0e0; padding: 20px; }
        .container { max-width: 400px; margin: 0 auto; }
        h1 { text-align: center; margin-bottom: 20px; color: #fff; }
        .card { background: #2a2a2a; border-radius: 8px; padding: 20px; margin-bottom: 15px; }
        .status { text-align: center; font-size: 48px; font-weight: bold; }
        .state { text-align: center; color: #888; margin-bottom: 10px; }
        .stats { display: flex; justify-content: space-around; text-align: center; }
        .stat-value { font-size: 24px; font-weight: bold; }
        .stat-label { font-size: 12px; color: #888; }
        .btn { display: block; width: 100%; padding: 15px; border: none; border-radius: 8px;
               font-size: 16px; cursor: pointer; margin-top: 10px; }
        .btn-primary { background: #4a4a4a; color: #fff; }
        .btn-primary:hover { background: #5a5a5a; }
        a { color: #888; text-decoration: none; display: block; text-align: center; margin-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Stick Pomodoro</h1>
        <div class="card">
            <div class="state" id="state">-</div>
            <div class="status" id="timer">--:--</div>
        </div>
        <div class="card">
            <div class="stats">
                <div><div class="stat-value" id="pomodoros">0</div><div class="stat-label">Pomodoros</div></div>
                <div><div class="stat-value" id="focus">0m</div><div class="stat-label">Foco</div></div>
                <div><div class="stat-value" id="cycle">1/4</div><div class="stat-label">Ciclo</div></div>
            </div>
        </div>
        <button class="btn btn-primary" onclick="control('toggle')">Iniciar / Pausar</button>
        <button class="btn btn-primary" onclick="control('reset')">Resetar</button>
        <a href="/config">Configuracoes</a>
    </div>
    <script>
        function update() {
            fetch('/api/status').then(r => r.json()).then(d => {
                document.getElementById('state').textContent = d.state;
                document.getElementById('timer').textContent = d.time;
                document.getElementById('pomodoros').textContent = d.pomodoros;
                document.getElementById('focus').textContent = d.focus;
                document.getElementById('cycle').textContent = d.cycle + '/' + d.maxCycle;
            });
        }
        function control(action) {
            fetch('/api/control', {method:'POST', headers:{'Content-Type':'application/json'},
                body: JSON.stringify({action: action})}).then(() => update());
        }
        setInterval(update, 1000);
        update();
    </script>
</body>
</html>
)rawliteral";

static const char CONFIG_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Configuracoes</title>
    <style>
        * { box-sizing: border-box; margin: 0; padding: 0; }
        body { font-family: -apple-system, sans-serif; background: #1a1a1a; color: #e0e0e0; padding: 20px; }
        .container { max-width: 400px; margin: 0 auto; }
        h1 { text-align: center; margin-bottom: 20px; color: #fff; }
        .card { background: #2a2a2a; border-radius: 8px; padding: 20px; margin-bottom: 15px; }
        label { display: block; margin-bottom: 5px; color: #888; }
        input, select { width: 100%; padding: 10px; border: 1px solid #444; border-radius: 4px;
                        background: #333; color: #fff; margin-bottom: 15px; }
        .btn { display: block; width: 100%; padding: 15px; border: none; border-radius: 8px;
               font-size: 16px; cursor: pointer; }
        .btn-primary { background: #4a4a4a; color: #fff; }
        a { color: #888; text-decoration: none; display: block; text-align: center; margin-top: 15px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>Configuracoes</h1>
        <form id="configForm">
            <div class="card">
                <label>Tempo de Foco (min)</label>
                <input type="number" id="focus" min="1" max="60" value="25">
                <label>Descanso Curto (min)</label>
                <input type="number" id="short" min="1" max="30" value="5">
                <label>Descanso Longo (min)</label>
                <input type="number" id="long" min="1" max="60" value="15">
                <label>Ciclos para Descanso Longo</label>
                <input type="number" id="cycles" min="1" max="10" value="4">
            </div>
            <div class="card">
                <label>Modo de Alerta</label>
                <select id="alert">
                    <option value="0">Nenhum</option>
                    <option value="1">Som</option>
                    <option value="2">Visual</option>
                    <option value="3" selected>Ambos</option>
                </select>
                <label>Volume</label>
                <select id="volume">
                    <option value="0">Baixo</option>
                    <option value="1" selected>Medio</option>
                    <option value="2">Alto</option>
                </select>
            </div>
            <button type="submit" class="btn btn-primary">Salvar</button>
        </form>
        <a href="/">Voltar</a>
    </div>
    <script>
        fetch('/api/config').then(r => r.json()).then(d => {
            document.getElementById('focus').value = d.focus;
            document.getElementById('short').value = d.short;
            document.getElementById('long').value = d.long;
            document.getElementById('cycles').value = d.cycles;
            document.getElementById('alert').value = d.alert;
            document.getElementById('volume').value = d.volume;
        });
        document.getElementById('configForm').onsubmit = function(e) {
            e.preventDefault();
            fetch('/api/config', {
                method: 'POST',
                headers: {'Content-Type': 'application/json'},
                body: JSON.stringify({
                    focus: parseInt(document.getElementById('focus').value),
                    short: parseInt(document.getElementById('short').value),
                    long: parseInt(document.getElementById('long').value),
                    cycles: parseInt(document.getElementById('cycles').value),
                    alert: parseInt(document.getElementById('alert').value),
                    volume: parseInt(document.getElementById('volume').value)
                })
            }).then(() => alert('Salvo!'));
        };
    </script>
</body>
</html>
)rawliteral";

static const char* stateNames[] = {"Pronto", "Foco", "Descanso", "Descanso Longo", "Pausado"};

static void handleRoot() {
    server.send(200, "text/html", INDEX_HTML);
}

static void handleConfig() {
    server.send(200, "text/html", CONFIG_HTML);
}

static void handleApiStatus() {
    JsonDocument doc;

    doc["state"] = stateNames[pTimer->state];

    uint8_t min = pTimer->remainingSec / 60;
    uint8_t sec = pTimer->remainingSec % 60;
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", min, sec);
    doc["time"] = timeStr;

    doc["pomodoros"] = pTimer->todayPomodoros;

    uint32_t focusMin = pTimer->todayFocusSec / 60;
    char focusStr[10];
    if (focusMin >= 60) {
        sprintf(focusStr, "%ldh%ldm", focusMin / 60, focusMin % 60);
    } else {
        sprintf(focusStr, "%ldm", focusMin);
    }
    doc["focus"] = focusStr;
    doc["cycle"] = pTimer->currentCycle;
    doc["maxCycle"] = pSettings->cyclesForLong;

    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

static void handleApiConfig() {
    if (server.method() == HTTP_GET) {
        JsonDocument doc;
        doc["focus"] = pSettings->focusMin;
        doc["short"] = pSettings->shortBreakMin;
        doc["long"] = pSettings->longBreakMin;
        doc["cycles"] = pSettings->cyclesForLong;
        doc["alert"] = pSettings->alertMode;
        doc["volume"] = pSettings->volume;

        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    } else if (server.method() == HTTP_POST) {
        JsonDocument doc;
        deserializeJson(doc, server.arg("plain"));

        pSettings->focusMin = doc["focus"] | pSettings->focusMin;
        pSettings->shortBreakMin = doc["short"] | pSettings->shortBreakMin;
        pSettings->longBreakMin = doc["long"] | pSettings->longBreakMin;
        pSettings->cyclesForLong = doc["cycles"] | pSettings->cyclesForLong;
        pSettings->alertMode = doc["alert"] | pSettings->alertMode;
        pSettings->volume = doc["volume"] | pSettings->volume;

        server.send(200, "application/json", "{\"ok\":true}");
    }
}

static void handleApiControl() {
    // Control is handled in main loop
    server.send(200, "application/json", "{\"ok\":true}");
}

void webServerStart(Settings* settings, TimerData* timer) {
    pSettings = settings;
    pTimer = timer;

    // Generate SSID from MAC
    uint8_t mac[6];
    WiFi.macAddress(mac);
    sprintf(apSSID, "Pomodoro-%02X%02X", mac[4], mac[5]);

    WiFi.mode(WIFI_AP);
    WiFi.softAP(apSSID, WIFI_AP_PASSWORD, WIFI_AP_CHANNEL);

    IPAddress ip = WiFi.softAPIP();
    sprintf(apIP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

    server.on("/", handleRoot);
    server.on("/config", handleConfig);
    server.on("/api/status", handleApiStatus);
    server.on("/api/config", HTTP_GET, handleApiConfig);
    server.on("/api/config", HTTP_POST, handleApiConfig);
    server.on("/api/control", HTTP_POST, handleApiControl);

    server.begin();
    serverActive = true;

    Serial.printf("WiFi AP: %s\n", apSSID);
    Serial.printf("IP: %s\n", apIP);
}

void webServerStop() {
    server.stop();
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_OFF);
    serverActive = false;
}

void webServerHandle() {
    if (serverActive) {
        server.handleClient();
    }
}

bool webServerIsActive() {
    return serverActive;
}

const char* webServerGetSSID() {
    return apSSID;
}

const char* webServerGetIP() {
    return apIP;
}
