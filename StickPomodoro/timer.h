#ifndef TIMER_H
#define TIMER_H

#include "types.h"
#include "config.h"

// =============================================================================
// Timer Module
// =============================================================================

extern TimerData timerData;

// Initialize timer
void timerInit();

// Start timer (from IDLE)
void timerStart(Settings* settings);

// Pause/Resume
void timerPause();
void timerResume();

// Reset to IDLE
void timerReset();

// Skip current phase
void timerSkip(Settings* settings);

// Update timer (call every loop)
// Returns true if state changed
bool timerUpdate(Settings* settings);

// Get remaining time formatted
void timerGetTimeString(char* buf, size_t len);

// Check if timer just finished
bool timerJustFinished();

#endif
