#include "webserver.h"
#include "storage.h"
#include "config.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <ArduinoJson.h>

// =============================================================================
// Web Server Module Implementation (Simple HTTP)
// =============================================================================

static WiFiServer server(80);
static bool serverActive = false;
static char apSSID[20] = "";
static char apIP[16] = "192.168.4.1";

static Settings* pSettings = nullptr;
static TimerData* pTimer = nullptr;

// HTML pages
static const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Stick Pomodoro</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:sans-serif;background:#1a1a1a;color:#e0e0e0;padding:20px}
.c{max-width:400px;margin:0 auto}
h1{text-align:center;margin-bottom:20px}
.card{background:#2a2a2a;border-radius:8px;padding:20px;margin-bottom:15px}
.timer{text-align:center;font-size:48px;font-weight:bold}
.state{text-align:center;color:#888;margin-bottom:10px}
.stats{display:flex;justify-content:space-around;text-align:center}
.sv{font-size:24px;font-weight:bold}
.sl{font-size:12px;color:#888}
.btn{display:block;width:100%;padding:15px;border:none;border-radius:8px;font-size:16px;cursor:pointer;margin-top:10px;background:#4a4a4a;color:#fff}
a{color:#888;text-decoration:none;display:block;text-align:center;margin-top:15px}
</style>
</head>
<body>
<div class="c">
<h1>Stick Pomodoro</h1>
<div class="card">
<div class="state" id="st">-</div>
<div class="timer" id="tm">--:--</div>
</div>
<div class="card">
<div class="stats">
<div><div class="sv" id="pm">0</div><div class="sl">Pomodoros</div></div>
<div><div class="sv" id="fc">0m</div><div class="sl">Foco</div></div>
<div><div class="sv" id="cy">1/4</div><div class="sl">Ciclo</div></div>
</div>
</div>
<a href="/config">Configuracoes</a>
</div>
<script>
function u(){fetch('/api/status').then(r=>r.json()).then(d=>{
document.getElementById('st').textContent=d.s;
document.getElementById('tm').textContent=d.t;
document.getElementById('pm').textContent=d.p;
document.getElementById('fc').textContent=d.f;
document.getElementById('cy').textContent=d.c+'/'+d.m;
});}
setInterval(u,1000);u();
</script>
</body>
</html>
)rawliteral";

static const char CONFIG_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Config</title>
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{font-family:sans-serif;background:#1a1a1a;color:#e0e0e0;padding:20px}
.c{max-width:400px;margin:0 auto}
h1{text-align:center;margin-bottom:20px}
.card{background:#2a2a2a;border-radius:8px;padding:20px;margin-bottom:15px}
label{display:block;margin-bottom:5px;color:#888}
input,select{width:100%;padding:10px;border:1px solid #444;border-radius:4px;background:#333;color:#fff;margin-bottom:15px}
.btn{display:block;width:100%;padding:15px;border:none;border-radius:8px;font-size:16px;cursor:pointer;background:#4a4a4a;color:#fff}
a{color:#888;text-decoration:none;display:block;text-align:center;margin-top:15px}
</style>
</head>
<body>
<div class="c">
<h1>Configuracoes</h1>
<form id="f">
<div class="card">
<label>Foco (min)</label>
<input type="number" id="fo" min="1" max="60" value="25">
<label>Descanso Curto (min)</label>
<input type="number" id="sh" min="1" max="30" value="5">
<label>Descanso Longo (min)</label>
<input type="number" id="lo" min="1" max="60" value="15">
<label>Ciclos</label>
<input type="number" id="cy" min="1" max="10" value="4">
</div>
<div class="card">
<label>Alerta</label>
<select id="al">
<option value="0">Nenhum</option>
<option value="1">Som</option>
<option value="2">Visual</option>
<option value="3" selected>Ambos</option>
</select>
<label>Volume</label>
<select id="vo">
<option value="0">Baixo</option>
<option value="1" selected>Medio</option>
<option value="2">Alto</option>
</select>
<label>Num. Beeps</label>
<input type="number" id="bp" min="1" max="10" value="3">
<label>Num. Flashes</label>
<input type="number" id="fl" min="1" max="10" value="3">
</div>
<button type="submit" class="btn">Salvar</button>
</form>
<a href="/">Voltar</a>
</div>
<script>
fetch('/api/config').then(r=>r.json()).then(d=>{
document.getElementById('fo').value=d.fo;
document.getElementById('sh').value=d.sh;
document.getElementById('lo').value=d.lo;
document.getElementById('cy').value=d.cy;
document.getElementById('al').value=d.al;
document.getElementById('vo').value=d.vo;
document.getElementById('bp').value=d.bp;
document.getElementById('fl').value=d.fl;
});
document.getElementById('f').onsubmit=function(e){
e.preventDefault();
var p='fo='+document.getElementById('fo').value;
p+='&sh='+document.getElementById('sh').value;
p+='&lo='+document.getElementById('lo').value;
p+='&cy='+document.getElementById('cy').value;
p+='&al='+document.getElementById('al').value;
p+='&vo='+document.getElementById('vo').value;
p+='&bp='+document.getElementById('bp').value;
p+='&fl='+document.getElementById('fl').value;
fetch('/api/save?'+p).then(()=>alert('Salvo!'));
};
</script>
</body>
</html>
)rawliteral";

static const char* stateNames[] = {"Pronto", "Foco", "Descanso", "Descanso Longo", "Pausado"};

static void sendResponse(WiFiClient& client, int code, const char* contentType, const char* body) {
    client.println("HTTP/1.1 " + String(code) + " OK");
    client.println("Content-Type: " + String(contentType));
    client.println("Connection: close");
    client.println();
    client.print(body);
}

static void sendHtml(WiFiClient& client, const char* html) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html; charset=utf-8");
    client.println("Connection: close");
    client.println();

    // ESP32 has unified memory, can read PROGMEM directly
    client.print(html);
}

static void handleRequest(WiFiClient& client, String& request) {
    if (request.indexOf("GET / ") >= 0) {
        sendHtml(client, INDEX_HTML);
    }
    else if (request.indexOf("GET /config") >= 0) {
        sendHtml(client, CONFIG_HTML);
    }
    else if (request.indexOf("GET /api/status") >= 0) {
        char json[200];
        uint8_t min = pTimer->remainingSec / 60;
        uint8_t sec = pTimer->remainingSec % 60;
        uint32_t focusMin = pTimer->todayFocusSec / 60;

        char timeStr[8], focusStr[16];
        sprintf(timeStr, "%02d:%02d", min, sec);
        if (focusMin >= 60) {
            sprintf(focusStr, "%ldh%ldm", focusMin / 60, focusMin % 60);
        } else {
            sprintf(focusStr, "%ldm", focusMin);
        }

        sprintf(json, "{\"s\":\"%s\",\"t\":\"%s\",\"p\":%d,\"f\":\"%s\",\"c\":%d,\"m\":%d}",
            stateNames[pTimer->state], timeStr, pTimer->todayPomodoros,
            focusStr, pTimer->currentCycle, pSettings->cyclesForLong);

        sendResponse(client, 200, "application/json", json);
    }
    else if (request.indexOf("GET /api/config") >= 0) {
        char json[150];
        sprintf(json, "{\"fo\":%d,\"sh\":%d,\"lo\":%d,\"cy\":%d,\"al\":%d,\"vo\":%d,\"bp\":%d,\"fl\":%d}",
            pSettings->focusMin, pSettings->shortBreakMin, pSettings->longBreakMin,
            pSettings->cyclesForLong, pSettings->alertMode, pSettings->volume,
            pSettings->beepCount, pSettings->flashCount);

        sendResponse(client, 200, "application/json", json);
    }
    else if (request.indexOf("GET /api/save") >= 0) {
        // Parse parameters from URL
        int idx;

        idx = request.indexOf("fo=");
        if (idx > 0) pSettings->focusMin = request.substring(idx + 3).toInt();

        idx = request.indexOf("sh=");
        if (idx > 0) pSettings->shortBreakMin = request.substring(idx + 3).toInt();

        idx = request.indexOf("lo=");
        if (idx > 0) pSettings->longBreakMin = request.substring(idx + 3).toInt();

        idx = request.indexOf("cy=");
        if (idx > 0) pSettings->cyclesForLong = request.substring(idx + 3).toInt();

        idx = request.indexOf("al=");
        if (idx > 0) pSettings->alertMode = request.substring(idx + 3).toInt();

        idx = request.indexOf("vo=");
        if (idx > 0) pSettings->volume = request.substring(idx + 3).toInt();

        idx = request.indexOf("bp=");
        if (idx > 0) pSettings->beepCount = request.substring(idx + 3).toInt();

        idx = request.indexOf("fl=");
        if (idx > 0) pSettings->flashCount = request.substring(idx + 3).toInt();

        // Persist settings to NVS
        saveSettings(pSettings);

        sendResponse(client, 200, "application/json", "{\"ok\":true}");
    }
    else {
        sendResponse(client, 404, "text/plain", "Not Found");
    }
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
    if (!serverActive) return;

    WiFiClient client = server.available();
    if (!client) return;

    // Wait for data
    unsigned long timeout = millis() + 1000;
    while (!client.available() && millis() < timeout) {
        delay(1);
    }

    if (!client.available()) {
        client.stop();
        return;
    }

    // Read request
    String request = "";
    while (client.available()) {
        request += (char)client.read();
    }

    handleRequest(client, request);
    client.stop();
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
