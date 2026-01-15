#include "alerts.h"
#include "config.h"
#include <M5StickCPlus2.h>

// =============================================================================
// Alerts Module Implementation
// =============================================================================

static bool alertActive = false;
static uint8_t beepsDone = 0;
static uint8_t flashesDone = 0;
static uint8_t targetBeeps = 0;
static uint8_t targetFlashes = 0;
static uint32_t lastAlertMs = 0;
static uint8_t currentVolume = VOLUME_MEDIUM;
static uint8_t currentMode = ALERT_NONE;

void alertsInit() {
    alertActive = false;
    beepsDone = 0;
    flashesDone = 0;
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
}

void alertPlay(uint8_t mode, uint8_t volume, uint8_t beepCount, uint8_t flashCount) {
    currentMode = mode;
    currentVolume = volume;
    targetBeeps = beepCount;
    targetFlashes = flashCount;
    beepsDone = 0;
    flashesDone = 0;

    if (mode == ALERT_NONE) {
        return;
    }

    alertActive = true;
    lastAlertMs = millis();

    // Play first alert immediately
    if (mode == ALERT_SOUND || mode == ALERT_BOTH) {
        alertSound(volume);
        beepsDone++;
    }

    if (mode == ALERT_VISUAL || mode == ALERT_BOTH) {
        alertVisual();
        flashesDone++;
    }
}

void alertStop() {
    M5.Speaker.stop();
    alertActive = false;
    beepsDone = 0;
    flashesDone = 0;
}

void alertsUpdate() {
    if (!alertActive) return;

    uint32_t now = millis();

    // Alert interval: 300ms between each beep/flash
    if ((now - lastAlertMs) < 300) return;

    lastAlertMs = now;

    bool soundDone = true;
    bool visualDone = true;

    // Handle sound alerts
    if (currentMode == ALERT_SOUND || currentMode == ALERT_BOTH) {
        if (beepsDone < targetBeeps) {
            alertSound(currentVolume);
            beepsDone++;
            soundDone = false;
        }
    }

    // Handle visual alerts
    if (currentMode == ALERT_VISUAL || currentMode == ALERT_BOTH) {
        if (flashesDone < targetFlashes) {
            alertVisual();
            flashesDone++;
            visualDone = false;
        }
    }

    // Check if all alerts are done
    if (soundDone && visualDone) {
        alertActive = false;
    }
}

bool alertIsPlaying() {
    return alertActive;
}
