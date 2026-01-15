#include "storage.h"
#include "config.h"
#include <Preferences.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// =============================================================================
// Storage Module Implementation
// =============================================================================

static Preferences prefs;
static char lastDate[11] = "";

void storageInit() {
    prefs.begin(PREFS_NAMESPACE, false);

    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS mount failed");
    }
}

void loadSettings(Settings* settings) {
    settings->focusMin = prefs.getUChar("focus", DEFAULT_FOCUS_MIN);
    settings->shortBreakMin = prefs.getUChar("short", DEFAULT_SHORT_BREAK_MIN);
    settings->longBreakMin = prefs.getUChar("long", DEFAULT_LONG_BREAK_MIN);
    settings->cyclesForLong = prefs.getUChar("cycles", DEFAULT_CYCLES_FOR_LONG);
    settings->alertMode = prefs.getUChar("alert", ALERT_BOTH);
    settings->volume = prefs.getUChar("volume", VOLUME_MEDIUM);
    settings->beepCount = prefs.getUChar("beeps", DEFAULT_BEEP_COUNT);
    settings->flashCount = prefs.getUChar("flashes", DEFAULT_FLASH_COUNT);
}

void saveSettings(Settings* settings) {
    prefs.putUChar("focus", settings->focusMin);
    prefs.putUChar("short", settings->shortBreakMin);
    prefs.putUChar("long", settings->longBreakMin);
    prefs.putUChar("cycles", settings->cyclesForLong);
    prefs.putUChar("alert", settings->alertMode);
    prefs.putUChar("volume", settings->volume);
    prefs.putUChar("beeps", settings->beepCount);
    prefs.putUChar("flashes", settings->flashCount);
}

void loadTodayStats(DayStats* stats, const char* todayDate) {
    strncpy(stats->date, todayDate, 10);
    stats->date[10] = '\0';

    // Try to load from file
    if (!LittleFS.exists(HISTORY_FILE)) {
        stats->pomodoros = 0;
        stats->focusSeconds = 0;
        stats->cyclesCompleted = 0;
        return;
    }

    File file = LittleFS.open(HISTORY_FILE, "r");
    if (!file) {
        stats->pomodoros = 0;
        stats->focusSeconds = 0;
        stats->cyclesCompleted = 0;
        return;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        stats->pomodoros = 0;
        stats->focusSeconds = 0;
        stats->cyclesCompleted = 0;
        return;
    }

    JsonArray days = doc["days"].as<JsonArray>();
    for (JsonObject day : days) {
        if (strcmp(day["date"].as<const char*>(), todayDate) == 0) {
            stats->pomodoros = day["pomodoros"] | 0;
            stats->focusSeconds = day["focus_sec"] | 0;
            stats->cyclesCompleted = day["cycles"] | 0;
            return;
        }
    }

    stats->pomodoros = 0;
    stats->focusSeconds = 0;
    stats->cyclesCompleted = 0;
}

void saveTodayStats(DayStats* stats) {
    JsonDocument doc;
    JsonArray days;

    // Load existing data
    if (LittleFS.exists(HISTORY_FILE)) {
        File file = LittleFS.open(HISTORY_FILE, "r");
        if (file) {
            deserializeJson(doc, file);
            file.close();
        }
    }

    if (doc.containsKey("days")) {
        days = doc["days"].as<JsonArray>();
    } else {
        days = doc["days"].to<JsonArray>();
    }

    // Find or create today's entry
    bool found = false;
    for (JsonObject day : days) {
        if (strcmp(day["date"].as<const char*>(), stats->date) == 0) {
            day["pomodoros"] = stats->pomodoros;
            day["focus_sec"] = stats->focusSeconds;
            day["cycles"] = stats->cyclesCompleted;
            found = true;
            break;
        }
    }

    if (!found) {
        JsonObject newDay = days.add<JsonObject>();
        newDay["date"] = stats->date;
        newDay["pomodoros"] = stats->pomodoros;
        newDay["focus_sec"] = stats->focusSeconds;
        newDay["cycles"] = stats->cyclesCompleted;
    }

    // Keep only last N days
    while (days.size() > MAX_HISTORY_DAYS) {
        days.remove(0);
    }

    // Save
    File file = LittleFS.open(HISTORY_FILE, "w");
    if (file) {
        serializeJson(doc, file);
        file.close();
    }
}

uint16_t getWeekPomodoros() {
    // Simplified: return stored value or 0
    return prefs.getUShort("week_pom", 0);
}

uint16_t getMonthPomodoros() {
    return prefs.getUShort("month_pom", 0);
}

void checkNewDay(const char* todayDate, TimerData* timer) {
    if (strcmp(lastDate, todayDate) != 0) {
        // New day - reset daily counters
        if (strlen(lastDate) > 0) {
            // Save previous day stats before reset
            DayStats prevStats;
            strncpy(prevStats.date, lastDate, 10);
            prevStats.pomodoros = timer->todayPomodoros;
            prevStats.focusSeconds = timer->todayFocusSec;
            prevStats.cyclesCompleted = 0;
            saveTodayStats(&prevStats);
        }

        strncpy(lastDate, todayDate, 10);
        lastDate[10] = '\0';

        timer->todayPomodoros = 0;
        timer->todayFocusSec = 0;
    }
}
