#ifndef STORAGE_H
#define STORAGE_H

#include "types.h"

// =============================================================================
// Storage Module
// =============================================================================

// Initialize storage (Preferences + LittleFS)
void storageInit();

// Settings
void loadSettings(Settings* settings);
void saveSettings(Settings* settings);

// Today stats
void loadTodayStats(DayStats* stats, const char* todayDate);
void saveTodayStats(DayStats* stats);

// History
uint16_t getWeekPomodoros();
uint16_t getMonthPomodoros();

// Reset daily stats if new day
void checkNewDay(const char* todayDate, TimerData* timer);

#endif
