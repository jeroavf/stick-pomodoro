#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// =============================================================================
// Stick Pomodoro - Type Definitions
// =============================================================================

// Timer states
enum TimerState {
    STATE_IDLE,
    STATE_FOCUS,
    STATE_SHORT_BREAK,
    STATE_LONG_BREAK,
    STATE_PAUSED
};

// Screen types
enum ScreenType {
    SCREEN_TIMER,
    SCREEN_STATS,
    SCREEN_WIFI
};

// Settings structure
struct Settings {
    uint8_t focusMin;
    uint8_t shortBreakMin;
    uint8_t longBreakMin;
    uint8_t cyclesForLong;
    uint8_t alertMode;
    uint8_t volume;
};

// Daily statistics
struct DayStats {
    char date[11];          // "YYYY-MM-DD\0"
    uint16_t pomodoros;
    uint32_t focusSeconds;
    uint8_t cyclesCompleted;
};

// Timer data
struct TimerData {
    TimerState state;
    TimerState prevState;
    uint32_t remainingSec;
    uint32_t totalSec;
    uint8_t currentCycle;
    uint16_t todayPomodoros;
    uint32_t todayFocusSec;
};

#endif
