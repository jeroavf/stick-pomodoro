#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "types.h"

// =============================================================================
// Web Server Module
// =============================================================================

// Start WiFi AP and web server
void webServerStart(Settings* settings, TimerData* timer);

// Stop WiFi and web server
void webServerStop();

// Handle client requests (call in loop)
void webServerHandle();

// Check if WiFi is active
bool webServerIsActive();

// Get AP SSID
const char* webServerGetSSID();

// Get AP IP
const char* webServerGetIP();

#endif
