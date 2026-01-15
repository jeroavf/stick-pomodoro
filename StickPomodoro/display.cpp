#include "display.h"
#include "config.h"

// =============================================================================
// Display Module Implementation
// =============================================================================

uint16_t grays[13];
TFT_eSprite sprite = TFT_eSprite(&M5.Lcd);

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

    sprite.createSprite(SCREEN_WIDTH, SCREEN_HEIGHT);
    sprite.setSwapBytes(true);
}

void displayUpdate() {
    sprite.pushSprite(0, 0);
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
    sprite.fillRect(0, 0, SCREEN_WIDTH, 20, grays[11]);
    sprite.setTextColor(grays[2]);
    sprite.setTextSize(1);
    sprite.drawString(title, 6, 5);

    // Cycle indicator
    drawCycleIndicator(SCREEN_WIDTH - 60, 10, cycle, maxCycles);
}

void drawFooter(const char* hint, uint8_t hour, uint8_t minute) {
    sprite.fillRect(0, SCREEN_HEIGHT - 18, SCREEN_WIDTH, 18, grays[11]);
    sprite.setTextColor(grays[4]);
    sprite.setTextSize(1);
    sprite.drawString(hint, 6, SCREEN_HEIGHT - 14);

    // Clock
    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", hour, minute);
    sprite.drawString(timeStr, SCREEN_WIDTH - 40, SCREEN_HEIGHT - 14);
}

void drawProgressBar(int x, int y, int w, int h, float progress) {
    int fillW = (int)(w * progress);

    // Background
    sprite.fillRoundRect(x, y, w, h, 3, grays[10]);

    // Fill
    if (fillW > 6) {
        sprite.fillRoundRect(x, y, fillW, h, 3, grays[2]);
    }
}

void drawCycleIndicator(int x, int y, uint8_t completed, uint8_t total) {
    int spacing = 10;
    for (int i = 0; i < total; i++) {
        int cx = x + (i * spacing);
        if (i < completed) {
            sprite.fillCircle(cx, y, 3, grays[2]);
        } else {
            sprite.drawCircle(cx, y, 3, grays[6]);
        }
    }
}

void drawTimer(uint32_t seconds, TimerState state) {
    uint8_t min = seconds / 60;
    uint8_t sec = seconds % 60;

    char timeStr[6];
    sprintf(timeStr, "%02d:%02d", min, sec);

    sprite.setTextColor(stateToColor(state));
    sprite.setTextSize(4);

    int16_t tw = sprite.textWidth(timeStr);
    int16_t x = (138 - tw) / 2;  // Center in left panel

    sprite.drawString(timeStr, x, 55);
}

void drawTimerScreen(TimerData* timer, Settings* settings, uint8_t hour, uint8_t minute) {
    sprite.fillSprite(TFT_BLACK);

    // Header
    drawHeader("POMODORO", timer->currentCycle, settings->cyclesForLong);

    // Divider line
    sprite.drawLine(138, 20, 138, SCREEN_HEIGHT - 18, grays[10]);

    // Left panel - Timer
    sprite.setTextColor(grays[4]);
    sprite.setTextSize(1);
    sprite.drawString(stateToString(timer->state), 35, 30);

    drawTimer(timer->remainingSec, timer->state);

    // Progress bar
    float progress = 0;
    if (timer->totalSec > 0) {
        progress = 1.0f - ((float)timer->remainingSec / timer->totalSec);
    }
    drawProgressBar(10, 95, 118, 8, progress);

    // Right panel - Today stats
    sprite.setTextColor(grays[3]);
    sprite.setTextSize(1);
    sprite.drawString("HOJE", 148, 25);

    sprite.setTextColor(grays[2]);
    char pomStr[16];
    sprintf(pomStr, "%d pomodoros", timer->todayPomodoros);
    sprite.drawString(pomStr, 148, 45);

    uint16_t focusMin = timer->todayFocusSec / 60;
    char focusStr[16];
    if (focusMin >= 60) {
        sprintf(focusStr, "%dh %dm foco", focusMin / 60, focusMin % 60);
    } else {
        sprintf(focusStr, "%dm foco", focusMin);
    }
    sprite.drawString(focusStr, 148, 60);

    // Mini bar chart placeholder (last 7 sessions)
    sprite.drawRect(148, 78, 82, 25, grays[9]);
    sprite.setTextColor(grays[6]);
    sprite.drawString("historico", 160, 85);

    // Footer
    const char* hint = (timer->state == STATE_IDLE) ? "[A] Iniciar" : "[A] Pausar";
    drawFooter(hint, hour, minute);

    displayUpdate();
}

void drawStatsScreen(DayStats* today, uint16_t weekPomodoros, uint16_t monthPomodoros) {
    sprite.fillSprite(TFT_BLACK);

    drawHeader("ESTATISTICAS", 0, 0);

    // Today box
    sprite.drawRoundRect(10, 28, 70, 50, 4, grays[8]);
    sprite.setTextColor(grays[4]);
    sprite.drawString("HOJE", 25, 32);
    sprite.setTextColor(grays[2]);
    sprite.setTextSize(2);
    char buf[8];
    sprintf(buf, "%d", today->pomodoros);
    sprite.drawString(buf, 30, 48);
    sprite.setTextSize(1);
    sprite.drawString("pom", 50, 55);

    // Week box
    sprite.drawRoundRect(85, 28, 70, 50, 4, grays[8]);
    sprite.setTextColor(grays[4]);
    sprite.setTextSize(1);
    sprite.drawString("SEMANA", 97, 32);
    sprite.setTextColor(grays[2]);
    sprite.setTextSize(2);
    sprintf(buf, "%d", weekPomodoros);
    sprite.drawString(buf, 105, 48);
    sprite.setTextSize(1);
    sprite.drawString("pom", 125, 55);

    // Month box
    sprite.drawRoundRect(160, 28, 70, 50, 4, grays[8]);
    sprite.setTextColor(grays[4]);
    sprite.setTextSize(1);
    sprite.drawString("MES", 182, 32);
    sprite.setTextColor(grays[2]);
    sprite.setTextSize(2);
    sprintf(buf, "%d", monthPomodoros);
    sprite.drawString(buf, 180, 48);
    sprite.setTextSize(1);
    sprite.drawString("pom", 200, 55);

    // Focus time today
    sprite.setTextColor(grays[3]);
    sprite.setTextSize(1);
    uint32_t focusMin = today->focusSeconds / 60;
    char focusStr[32];
    if (focusMin >= 60) {
        sprintf(focusStr, "Tempo foco hoje: %ldh %ldm", focusMin / 60, focusMin % 60);
    } else {
        sprintf(focusStr, "Tempo foco hoje: %ldm", focusMin);
    }
    sprite.drawString(focusStr, 10, 90);

    drawFooter("[A] Voltar  [B] WiFi", 0, 0);

    displayUpdate();
}

void drawWifiScreen(const char* ssid, const char* ip) {
    sprite.fillSprite(TFT_BLACK);

    drawHeader("CONFIGURACAO WiFi", 0, 0);

    sprite.setTextColor(grays[3]);
    sprite.setTextSize(1);
    sprite.drawString("Conecte-se a rede:", 10, 30);

    sprite.drawRoundRect(10, 45, 220, 45, 4, grays[9]);

    sprite.setTextColor(grays[2]);
    char ssidStr[32];
    sprintf(ssidStr, "SSID: %s", ssid);
    sprite.drawString(ssidStr, 20, 55);

    char passStr[32];
    sprintf(passStr, "Senha: %s", WIFI_AP_PASSWORD);
    sprite.drawString(passStr, 20, 70);

    sprite.setTextColor(grays[3]);
    char urlStr[32];
    sprintf(urlStr, "Acesse: http://%s", ip);
    sprite.drawString(urlStr, 10, 100);

    drawFooter("[A] Voltar", 0, 0);

    displayUpdate();
}
