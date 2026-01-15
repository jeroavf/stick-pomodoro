#ifndef ALERTS_H
#define ALERTS_H

#include <stdint.h>

// =============================================================================
// Alerts Module
// =============================================================================

// Initialize alerts (buzzer, LED)
void alertsInit();

// Play alert based on mode
void alertPlay(uint8_t mode, uint8_t volume);

// Visual alert (LED flash)
void alertVisual();

// Sound alert (buzzer beeps)
void alertSound(uint8_t volume);

// Stop any ongoing alert
void alertStop();

// Update alerts (for non-blocking animations)
void alertsUpdate();

#endif
