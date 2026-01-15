#include "timer.h"
#include <Arduino.h>

// =============================================================================
// Timer Module Implementation
// =============================================================================

TimerData timerData;
static uint32_t lastUpdateMs = 0;
static bool justFinished = false;

void timerInit() {
    timerData.state = STATE_IDLE;
    timerData.prevState = STATE_IDLE;
    timerData.remainingSec = 0;
    timerData.totalSec = 0;
    timerData.currentCycle = 1;
    timerData.todayPomodoros = 0;
    timerData.todayFocusSec = 0;
    lastUpdateMs = millis();
}

static void setTimer(uint8_t minutes) {
    timerData.totalSec = minutes * 60;
    timerData.remainingSec = timerData.totalSec;
}

void timerStart(Settings* settings) {
    if (timerData.state == STATE_IDLE) {
        timerData.state = STATE_FOCUS;
        setTimer(settings->focusMin);
        timerData.currentCycle = 1;
        lastUpdateMs = millis();
    }
}

void timerPause() {
    if (timerData.state != STATE_IDLE && timerData.state != STATE_PAUSED) {
        timerData.prevState = timerData.state;
        timerData.state = STATE_PAUSED;
    }
}

void timerResume() {
    if (timerData.state == STATE_PAUSED) {
        timerData.state = timerData.prevState;
        lastUpdateMs = millis();
    }
}

void timerReset() {
    timerData.state = STATE_IDLE;
    timerData.remainingSec = 0;
    timerData.totalSec = 0;
    timerData.currentCycle = 1;
}

void timerSkip(Settings* settings) {
    // Simulate timer finished
    timerData.remainingSec = 0;
    timerUpdate(settings);
}

static void transitionToNextState(Settings* settings) {
    switch (timerData.state) {
        case STATE_FOCUS:
            // Completed a pomodoro
            timerData.todayPomodoros++;
            timerData.todayFocusSec += timerData.totalSec;

            if (timerData.currentCycle >= settings->cyclesForLong) {
                // Long break
                timerData.state = STATE_LONG_BREAK;
                setTimer(settings->longBreakMin);
                timerData.currentCycle = 1;
            } else {
                // Short break
                timerData.state = STATE_SHORT_BREAK;
                setTimer(settings->shortBreakMin);
            }
            break;

        case STATE_SHORT_BREAK:
            timerData.state = STATE_FOCUS;
            setTimer(settings->focusMin);
            timerData.currentCycle++;
            break;

        case STATE_LONG_BREAK:
            timerData.state = STATE_FOCUS;
            setTimer(settings->focusMin);
            break;

        default:
            break;
    }
}

bool timerUpdate(Settings* settings) {
    justFinished = false;

    if (timerData.state == STATE_IDLE || timerData.state == STATE_PAUSED) {
        return false;
    }

    uint32_t now = millis();
    uint32_t elapsed = now - lastUpdateMs;

    if (elapsed >= 1000) {
        lastUpdateMs = now;

        if (timerData.remainingSec > 0) {
            timerData.remainingSec--;

            // Track focus time in real-time
            if (timerData.state == STATE_FOCUS) {
                timerData.todayFocusSec++;
            }
        }

        if (timerData.remainingSec == 0) {
            justFinished = true;
            transitionToNextState(settings);
            return true;
        }
    }

    return false;
}

void timerGetTimeString(char* buf, size_t len) {
    uint8_t min = timerData.remainingSec / 60;
    uint8_t sec = timerData.remainingSec % 60;
    snprintf(buf, len, "%02d:%02d", min, sec);
}

bool timerJustFinished() {
    return justFinished;
}
