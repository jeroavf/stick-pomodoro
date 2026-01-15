#include "display.h"
#include "config.h"

// =============================================================================
// Display Module Implementation
// =============================================================================

uint16_t grays[13];
M5Canvas canvas(&M5.Lcd);

// Initialize grayscale palette
static void initGrays() {
    for (int i = 0; i < 13; i++) {
        int v = 210 - (i * 18);
        if (v < 0) v = 0;
        grays[i] = M5.Lcd.color565(v, v, v);
    }
}

void displayInit() {
    M5.Lcd.setRotation(SCREEN_ROTATION);
    M5.Lcd.fillScreen(TFT_BLACK);

    initGrays();

    canvas.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    canvas.setSwapBytes(true);
}

void displayUpdate() {
    canvas.pushSprite(0, 0);
}

const char* stateToString(TimerState state) {
    switch (state) {
        case STATE_FOCUS:       return "FOCO";
        case STATE_SHORT_BREAK: return "DESCANSO";
        case STATE_LONG_BREAK:  return "DESCANSO LONGO";
        case STATE_PAUSED:      return "PAUSADO";
        default:                return "PRONTO";
    }
}

uint16_t stateToColor(TimerState state) {
    switch (state) {
        case STATE_FOCUS:       return grays[1];
        case STATE_SHORT_BREAK: return grays[4];
        case STATE_LONG_BREAK:  return grays[6];
        case STATE_PAUSED:      return grays[8];
        default:                return grays[3];
    }
}

void drawHeader(const char* title, uint8_t cycle, uint8_t maxCycles) {
    canvas.fillRect(0, 0, SCREEN_WIDTH, 20, grays[11]);
    canvas.setTextColor(grays[2]);
    canvas.setTextSize(1);
    canvas.drawString(title, 6, 5);

    // Cycle indicator
    drawCycleIndicator(SCREEN_WIDTH - 60, 10, cycle, maxCycles);
}

void drawFooter(const char* hint, uint8_t hour, uint8_t minute) {
    canvas.fillRect(0, SCREEN_HEIGHT - 18, SCREEN_WIDTH, 18, grays[11]);
    canvas.setTextColor(grays[4]);
    canvas.setTextSize(1);
    canvas.drawString(hint, 6, SCREEN_HEIGHT - 14);

    // Clock
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", hour, minute);
    canvas.drawString(timeStr, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 14);
}

void drawProgressBar(int x, int y, int w, int h, float progress) {
    int fillW = (int)(w * progress);

    // Background
    canvas.fillRoundRect(x, y, w, h, 3, grays[10]);

    // Fill
    if (fillW > 6) {
        canvas.fillRoundRect(x, y, fillW, h, 3, grays[2]);
    }
}

void drawCycleIndicator(int x, int y, uint8_t completed, uint8_t total) {
    int spacing = 10;
    for (int i = 0; i < total; i++) {
        int cx = x + (i * spacing);
        if (i < completed) {
            canvas.fillCircle(cx, y, 3, grays[2]);
        } else {
            canvas.drawCircle(cx, y, 3, grays[6]);
        }
    }
}

void drawTimer(uint32_t seconds, TimerState state) {
    uint8_t min = seconds / 60;
    uint8_t sec = seconds % 60;

    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", min, sec);

    canvas.setTextColor(stateToColor(state));
    canvas.setTextSize(4);

    int16_t tw = canvas.textWidth(timeStr);
    int16_t x = (138 - tw) / 2;  // Center in left panel

    canvas.drawString(timeStr, x, 55);
}

void drawTimerScreen(TimerData* timer, Settings* settings, uint8_t hour, uint8_t minute) {
    canvas.fillSprite(TFT_BLACK);

    // Header
    drawHeader("POMODORO", timer->currentCycle, settings->cyclesForLong);

    // Divider line
    canvas.drawLine(138, 20, 138, SCREEN_HEIGHT - 18, grays[10]);

    // Left panel - Timer
    canvas.setTextColor(grays[4]);
    canvas.setTextSize(1);
    canvas.drawString(stateToString(timer->state), 35, 30);

    drawTimer(timer->remainingSec, timer->state);

    // Progress bar
    float progress = 0;
    if (timer->totalSec > 0) {
        progress = 1.0f - ((float)timer->remainingSec / timer->totalSec);
    }
    drawProgressBar(10, 95, 118, 8, progress);

    // Right panel - Today stats
    canvas.setTextColor(grays[3]);
    canvas.setTextSize(1);
    canvas.drawString("HOJE", 148, 25);

    canvas.setTextColor(grays[2]);
    char pomStr[16];
    sprintf(pomStr, "%d pomodoros", timer->todayPomodoros);
    canvas.drawString(pomStr, 148, 45);

    uint16_t focusMin = timer->todayFocusSec / 60;
    char focusStr[16];
    if (focusMin >= 60) {
        sprintf(focusStr, "%dh %dm foco", focusMin / 60, focusMin % 60);
    } else {
        sprintf(focusStr, "%dm foco", focusMin);
    }
    canvas.drawString(focusStr, 148, 60);

    // Mini bar chart placeholder (last 7 sessions)
    canvas.drawRect(148, 78, 82, 25, grays[9]);
    canvas.setTextColor(grays[6]);
    canvas.drawString("historico", 160, 85);

    // Footer
    const char* hint = (timer->state == STATE_IDLE) ? "[A] Iniciar" : "[A] Pausar";
    drawFooter(hint, hour, minute);

    displayUpdate();
}

void drawStatsScreen(DayStats* today, uint16_t weekPomodoros, uint16_t monthPomodoros) {
    canvas.fillSprite(TFT_BLACK);

    drawHeader("ESTATISTICAS", 0, 0);

    // Today box
    canvas.drawRoundRect(10, 28, 70, 50, 4, grays[8]);
    canvas.setTextColor(grays[4]);
    canvas.drawString("HOJE", 25, 32);
    canvas.setTextColor(grays[2]);
    canvas.setTextSize(2);
    char buf[8];
    sprintf(buf, "%d", today->pomodoros);
    canvas.drawString(buf, 30, 48);
    canvas.setTextSize(1);
    canvas.drawString("pom", 50, 55);

    // Week box
    canvas.drawRoundRect(85, 28, 70, 50, 4, grays[8]);
    canvas.setTextColor(grays[4]);
    canvas.setTextSize(1);
    canvas.drawString("SEMANA", 97, 32);
    canvas.setTextColor(grays[2]);
    canvas.setTextSize(2);
    sprintf(buf, "%d", weekPomodoros);
    canvas.drawString(buf, 105, 48);
    canvas.setTextSize(1);
    canvas.drawString("pom", 125, 55);

    // Month box
    canvas.drawRoundRect(160, 28, 70, 50, 4, grays[8]);
    canvas.setTextColor(grays[4]);
    canvas.setTextSize(1);
    canvas.drawString("MES", 182, 32);
    canvas.setTextColor(grays[2]);
    canvas.setTextSize(2);
    sprintf(buf, "%d", monthPomodoros);
    canvas.drawString(buf, 180, 48);
    canvas.setTextSize(1);
    canvas.drawString("pom", 200, 55);

    // Focus time today
    canvas.setTextColor(grays[3]);
    canvas.setTextSize(1);
    uint32_t focusMin = today->focusSeconds / 60;
    char focusStr[32];
    if (focusMin >= 60) {
        sprintf(focusStr, "Tempo foco hoje: %ldh %ldm", focusMin / 60, focusMin % 60);
    } else {
        sprintf(focusStr, "Tempo foco hoje: %ldm", focusMin);
    }
    canvas.drawString(focusStr, 10, 90);

    drawFooter("[A] Voltar  [B] WiFi", 0, 0);

    displayUpdate();
}

void drawWifiScreen(const char* ssid, const char* ip) {
    canvas.fillSprite(TFT_BLACK);

    drawHeader("CONFIGURACAO WiFi", 0, 0);

    canvas.setTextColor(grays[3]);
    canvas.setTextSize(1);
    canvas.drawString("Conecte-se a rede:", 10, 30);

    canvas.drawRoundRect(10, 45, 220, 45, 4, grays[9]);

    canvas.setTextColor(grays[2]);
    char ssidStr[32];
    sprintf(ssidStr, "SSID: %s", ssid);
    canvas.drawString(ssidStr, 20, 55);

    char passStr[32];
    sprintf(passStr, "Senha: %s", WIFI_AP_PASSWORD);
    canvas.drawString(passStr, 20, 70);

    canvas.setTextColor(grays[3]);
    char urlStr[32];
    sprintf(urlStr, "Acesse: http://%s", ip);
    canvas.drawString(urlStr, 10, 100);

    drawFooter("[A] Voltar", 0, 0);

    displayUpdate();
}
