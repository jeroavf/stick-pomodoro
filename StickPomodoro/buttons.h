#ifndef BUTTONS_H
#define BUTTONS_H

#include <stdint.h>
#include <stdbool.h>

// =============================================================================
// Buttons Module
// =============================================================================

// Button IDs
#define BTN_A 0
#define BTN_B 1

// Initialize buttons
void buttonsInit();

// Update button states (call every loop)
void buttonsUpdate();

// Check button states
bool buttonPressed(uint8_t btn);
bool buttonLongPressed(uint8_t btn);
bool buttonReleased(uint8_t btn);

// Clear button events
void buttonsClear();

#endif
