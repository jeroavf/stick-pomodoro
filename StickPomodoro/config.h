#ifndef CONFIG_H
#define CONFIG_H

// =============================================================================
// Stick Pomodoro - Configuration
// =============================================================================

// Display
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 135
#define SCREEN_ROTATION 1  // Landscape mode

// Timer defaults (minutes)
#define DEFAULT_FOCUS_MIN 25
#define DEFAULT_SHORT_BREAK_MIN 5
#define DEFAULT_LONG_BREAK_MIN 15
#define DEFAULT_CYCLES_FOR_LONG 4

// Alert modes
#define ALERT_NONE 0
#define ALERT_SOUND 1
#define ALERT_VISUAL 2
#define ALERT_BOTH 3

// Volume levels
#define VOLUME_LOW 0
#define VOLUME_MEDIUM 1
#define VOLUME_HIGH 2

// Button timing (ms)
#define BUTTON_DEBOUNCE_MS 50
#define BUTTON_LONG_PRESS_MS 2000

// WiFi AP
#define WIFI_AP_PASSWORD "pomodoro123"
#define WIFI_AP_CHANNEL 1

// Storage
#define PREFS_NAMESPACE "pomodoro"
#define HISTORY_FILE "/history.json"
#define MAX_HISTORY_DAYS 30

// Display update
#define DISPLAY_UPDATE_MS 100

// Buzzer frequencies
#define BUZZER_FREQ_LOW 1000
#define BUZZER_FREQ_MID 2000
#define BUZZER_FREQ_HIGH 3000

#endif
