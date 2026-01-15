#ifndef DISPLAY_H
#define DISPLAY_H

#include <M5StickCPlus2.h>
#include "types.h"

// =============================================================================
// Display Module
// =============================================================================

// Grayscale palette (13 levels)
extern uint16_t grays[13];

// Canvas for double-buffering
extern M5Canvas canvas;

// Initialize display and sprite
void displayInit();

// Draw screens
void drawTimerScreen(TimerData* timer, Settings* settings, uint8_t hour, uint8_t minute);
void drawStatsScreen(DayStats* today, uint16_t weekPomodoros, uint16_t monthPomodoros);
void drawWifiScreen(const char* ssid, const char* ip);

// Draw components
void drawHeader(const char* title, uint8_t cycle, uint8_t maxCycles);
void drawFooter(const char* hint, uint8_t hour, uint8_t minute);
void drawTimer(uint32_t seconds, TimerState state);
void drawProgressBar(int x, int y, int w, int h, float progress);
void drawCycleIndicator(int x, int y, uint8_t completed, uint8_t total);

// Push sprite to display
void displayUpdate();

// Utility
const char* stateToString(TimerState state);
uint16_t stateToColor(TimerState state);

#endif
