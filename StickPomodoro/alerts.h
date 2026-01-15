#ifndef ALERTS_H
#define ALERTS_H

#include <stdint.h>

// =============================================================================
// Alerts Module
// =============================================================================

// Initialize alerts (buzzer, LED)
void alertsInit();

// Play alert based on mode with configurable repetitions
void alertPlay(uint8_t mode, uint8_t volume, uint8_t beepCount, uint8_t flashCount);

// Visual alert (LED flash) - single flash
void alertVisual();

// Sound alert (buzzer beep) - single beep
void alertSound(uint8_t volume);

// Stop any ongoing alert
void alertStop();

// Update alerts (for non-blocking animations)
void alertsUpdate();

// Check if alert is currently playing
bool alertIsPlaying();

#endif
