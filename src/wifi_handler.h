/**
 * WiFi Handler
 * Manages WiFi connection, reconnection logic, and status monitoring
 */

#ifndef WIFI_HANDLER_H
#define WIFI_HANDLER_H

#include <Arduino.h>
#include <WiFi.h>
#include "config.h"

class WiFiHandler {
private:
  bool connected = false;
  unsigned long lastReconnectAttempt = 0;
  int reconnectAttempts = 0;

public:
  /**
   * Initialize WiFi and attempt connection
   * @param ssid WiFi network name
   * @param password WiFi network password
   * @return true if connected successfully, false otherwise
   */
  bool init(const char* ssid, const char* password) {
    if (DEBUG_MODE) Serial.print("[WiFi] Connecting to ");
    if (DEBUG_MODE) Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long startTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startTime < WIFI_TIMEOUT_MS) {
      delay(500);
      if (DEBUG_MODE) Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      reconnectAttempts = 0;
      if (DEBUG_MODE) Serial.println();
      if (DEBUG_MODE) Serial.print("[WiFi] Connected! IP: ");
      if (DEBUG_MODE) Serial.println(WiFi.localIP());
      return true;
    } else {
      connected = false;
      if (DEBUG_MODE) Serial.println();
      if (DEBUG_MODE) Serial.println("[WiFi] Connection failed");
      return false;
    }
  }

  /**
   * Check connection status and attempt reconnection if needed
   * @return true if connected, false otherwise
   */
  bool checkConnection() {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      reconnectAttempts = 0;
      return true;
    }

    connected = false;

    // Attempt reconnection with backoff
    unsigned long now = millis();
    if (now - lastReconnectAttempt > WIFI_RECOVER_TIME_MS) {
      lastReconnectAttempt = now;
      reconnectAttempts++;

      if (DEBUG_MODE) Serial.printf("[WiFi] Attempting reconnection (attempt %d)\n", reconnectAttempts);
      WiFi.reconnect();
    }

    return false;
  }

  /**
   * Get current connection status
   * @return true if connected, false otherwise
   */
  bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
  }

  /**
   * Get WiFi signal strength in dBm
   * @return signal strength in dBm
   */
  int getSignalStrength() {
    return WiFi.RSSI();
  }

  /**
   * Get signal strength as bar indicator (0-4)
   * @return bar count (0-4)
   */
  int getSignalBars() {
    int rssi = WiFi.RSSI();
    if (rssi > -50) return 4;
    if (rssi > -60) return 3;
    if (rssi > -70) return 2;
    if (rssi > -80) return 1;
    return 0;
  }

  /**
   * Get connected SSID
   * @return SSID string
   */
  String getSSID() {
    return WiFi.SSID();
  }

  /**
   * Get local IP address
   * @return IP address as string
   */
  String getLocalIP() {
    return WiFi.localIP().toString();
  }

  /**
   * Disconnect from WiFi
   */
  void disconnect() {
    WiFi.disconnect(true); // Turn off WiFi radio
    connected = false;
    if (DEBUG_MODE) Serial.println("[WiFi] Disconnected");
  }

  /**
   * Get reconnection attempt count
   * @return number of reconnection attempts
   */
  int getReconnectAttempts() {
    return reconnectAttempts;
  }
};

#endif // WIFI_HANDLER_H
