#include "alerts.h"
#include "config.h"
#include <M5StickCPlus2.h>

// =============================================================================
// Alerts Module Implementation
// =============================================================================

static bool alertActive = false;
static uint8_t beepCount = 0;
static uint32_t lastBeepMs = 0;
static uint8_t currentVolume = VOLUME_MEDIUM;

void alertsInit() {
    // Buzzer is handled by M5StickCPlus2
    alertActive = false;
    beepCount = 0;
}

void alertSound(uint8_t volume) {
    int freq;
    int duration;

    switch (volume) {
        case VOLUME_LOW:
            freq = BUZZER_FREQ_LOW;
            duration = 100;
            break;
        case VOLUME_HIGH:
            freq = BUZZER_FREQ_HIGH;
            duration = 200;
            break;
        default:
            freq = BUZZER_FREQ_MID;
            duration = 150;
            break;
    }

    M5.Speaker.tone(freq, duration);
}

void alertVisual() {
    // Flash display by inverting colors briefly
    M5.Lcd.invertDisplay(true);
    delay(100);
    M5.Lcd.invertDisplay(false);
    delay(100);
    M5.Lcd.invertDisplay(true);
    delay(100);
    M5.Lcd.invertDisplay(false);
}

void alertPlay(uint8_t mode, uint8_t volume) {
    currentVolume = volume;

    if (mode == ALERT_NONE) {
        return;
    }

    if (mode == ALERT_SOUND || mode == ALERT_BOTH) {
        alertActive = true;
        beepCount = 0;
        lastBeepMs = millis();
        alertSound(volume);
        beepCount++;
    }

    if (mode == ALERT_VISUAL || mode == ALERT_BOTH) {
        alertVisual();
    }
}

void alertStop() {
    M5.Speaker.stop();
    alertActive = false;
    beepCount = 0;
}

void alertsUpdate() {
    if (!alertActive) return;

    uint32_t now = millis();

    // Play 3 beeps with 300ms interval
    if (beepCount < 3 && (now - lastBeepMs) >= 300) {
        alertSound(currentVolume);
        beepCount++;
        lastBeepMs = now;
    }

    if (beepCount >= 3) {
        alertActive = false;
    }
}
