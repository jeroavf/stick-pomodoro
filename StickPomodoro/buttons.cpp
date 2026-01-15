#include "buttons.h"
#include "config.h"
#include <M5StickCPlus2.h>

// =============================================================================
// Buttons Module Implementation
// =============================================================================

static bool btnPressed[2] = {false, false};
static bool btnLongPressed[2] = {false, false};
static bool btnReleased[2] = {false, false};
static uint32_t btnPressTime[2] = {0, 0};
static bool btnWasPressed[2] = {false, false};

void buttonsInit() {
    for (int i = 0; i < 2; i++) {
        btnPressed[i] = false;
        btnLongPressed[i] = false;
        btnReleased[i] = false;
        btnPressTime[i] = 0;
        btnWasPressed[i] = false;
    }
}

void buttonsUpdate() {
    // Clear previous frame events
    for (int i = 0; i < 2; i++) {
        btnPressed[i] = false;
        btnLongPressed[i] = false;
        btnReleased[i] = false;
    }

    // Button A
    if (M5.BtnA.wasPressed()) {
        btnPressed[BTN_A] = true;
        btnPressTime[BTN_A] = millis();
        btnWasPressed[BTN_A] = true;
    }

    if (M5.BtnA.wasReleased()) {
        btnReleased[BTN_A] = true;
        btnWasPressed[BTN_A] = false;
    }

    if (btnWasPressed[BTN_A] && M5.BtnA.isPressed()) {
        if ((millis() - btnPressTime[BTN_A]) >= BUTTON_LONG_PRESS_MS) {
            btnLongPressed[BTN_A] = true;
            btnWasPressed[BTN_A] = false;  // Prevent repeat
        }
    }

    // Button B
    if (M5.BtnB.wasPressed()) {
        btnPressed[BTN_B] = true;
        btnPressTime[BTN_B] = millis();
        btnWasPressed[BTN_B] = true;
    }

    if (M5.BtnB.wasReleased()) {
        btnReleased[BTN_B] = true;
        btnWasPressed[BTN_B] = false;
    }

    if (btnWasPressed[BTN_B] && M5.BtnB.isPressed()) {
        if ((millis() - btnPressTime[BTN_B]) >= BUTTON_LONG_PRESS_MS) {
            btnLongPressed[BTN_B] = true;
            btnWasPressed[BTN_B] = false;
        }
    }
}

bool buttonPressed(uint8_t btn) {
    if (btn > 1) return false;
    return btnPressed[btn];
}

bool buttonLongPressed(uint8_t btn) {
    if (btn > 1) return false;
    return btnLongPressed[btn];
}

bool buttonReleased(uint8_t btn) {
    if (btn > 1) return false;
    return btnReleased[btn];
}

void buttonsClear() {
    for (int i = 0; i < 2; i++) {
        btnPressed[i] = false;
        btnLongPressed[i] = false;
        btnReleased[i] = false;
    }
}
