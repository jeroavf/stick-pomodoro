/*
 * Stick Pomodoro
 * Pomodoro Timer for M5Stack StickC Plus 2
 *
 * MIT License
 * https://github.com/jeronimo/stick-pomodoro
 */

#include <M5StickCPlus2.h>
#include <ESP32Time.h>

#include "config.h"
#include "types.h"
#include "display.h"
#include "timer.h"
#include "storage.h"
#include "alerts.h"
#include "buttons.h"
#include "webserver.h"

// =============================================================================
// Global State
// =============================================================================

Settings settings;
DayStats todayStats;
ScreenType currentScreen = SCREEN_TIMER;
ESP32Time rtc;

bool wifiEnabled = false;
uint32_t lastDisplayUpdate = 0;

// =============================================================================
// Setup
// =============================================================================

void setup() {
    // Initialize M5StickC Plus 2
    auto cfg = M5.config();
    M5.begin(cfg);

    Serial.begin(115200);
    Serial.println("Stick Pomodoro starting...");

    // Initialize modules
    displayInit();
    storageInit();
    loadSettings(&settings);
    timerInit();
    alertsInit();
    buttonsInit();

    // Get today's date
    char today[11];
    sprintf(today, "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay());
    loadTodayStats(&todayStats, today);

    // Load previous stats into timer
    timerData.todayPomodoros = todayStats.pomodoros;
    timerData.todayFocusSec = todayStats.focusSeconds;

    Serial.println("Ready!");
}

// =============================================================================
// Main Loop
// =============================================================================

void loop() {
    M5.update();
    buttonsUpdate();

    // Check for new day
    char today[11];
    sprintf(today, "%04d-%02d-%02d", rtc.getYear(), rtc.getMonth() + 1, rtc.getDay());
    checkNewDay(today, &timerData);

    // Handle button inputs
    handleButtons();

    // Update timer
    bool stateChanged = timerUpdate(&settings);
    if (stateChanged && timerJustFinished()) {
        alertPlay(settings.alertMode, settings.volume);

        // Update today stats
        todayStats.pomodoros = timerData.todayPomodoros;
        todayStats.focusSeconds = timerData.todayFocusSec;
        saveTodayStats(&todayStats);
    }

    // Update alerts
    alertsUpdate();

    // Handle web server
    if (wifiEnabled) {
        webServerHandle();
    }

    // Update display
    uint32_t now = millis();
    if (now - lastDisplayUpdate >= DISPLAY_UPDATE_MS) {
        lastDisplayUpdate = now;
        updateDisplay();
    }

    delay(10);
}

// =============================================================================
// Button Handling
// =============================================================================

void handleButtons() {
    // Button A - Timer control
    if (buttonPressed(BTN_A)) {
        switch (currentScreen) {
            case SCREEN_TIMER:
                if (timerData.state == STATE_IDLE) {
                    timerStart(&settings);
                } else if (timerData.state == STATE_PAUSED) {
                    timerResume();
                } else {
                    timerPause();
                }
                break;

            case SCREEN_STATS:
            case SCREEN_WIFI:
                currentScreen = SCREEN_TIMER;
                break;
        }
    }

    // Button A long press - Reset
    if (buttonLongPressed(BTN_A)) {
        if (currentScreen == SCREEN_TIMER) {
            timerReset();
            alertStop();
        }
    }

    // Button B - Change screen
    if (buttonPressed(BTN_B)) {
        if (currentScreen == SCREEN_TIMER) {
            currentScreen = SCREEN_STATS;
        } else if (currentScreen == SCREEN_STATS) {
            currentScreen = SCREEN_TIMER;
        } else if (currentScreen == SCREEN_WIFI) {
            currentScreen = SCREEN_TIMER;
        }
    }

    // Button B long press - Toggle WiFi
    if (buttonLongPressed(BTN_B)) {
        if (!wifiEnabled) {
            wifiEnabled = true;
            webServerStart(&settings, &timerData);
            currentScreen = SCREEN_WIFI;
        } else {
            wifiEnabled = false;
            webServerStop();
            currentScreen = SCREEN_TIMER;
        }
    }
}

// =============================================================================
// Display Update
// =============================================================================

void updateDisplay() {
    uint8_t hour = rtc.getHour(true);
    uint8_t minute = rtc.getMinute();

    switch (currentScreen) {
        case SCREEN_TIMER:
            drawTimerScreen(&timerData, &settings, hour, minute);
            break;

        case SCREEN_STATS:
            drawStatsScreen(&todayStats, getWeekPomodoros(), getMonthPomodoros());
            break;

        case SCREEN_WIFI:
            drawWifiScreen(webServerGetSSID(), webServerGetIP());
            break;
    }
}
