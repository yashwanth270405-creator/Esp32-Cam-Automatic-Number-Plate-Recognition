/**
 * Data Logger
 * Logs recognized plates and metadata to memory and optional SPIFFS storage
 */

#ifndef DATA_LOGGER_H
#define DATA_LOGGER_H

#include <Arduino.h>
#include "config.h"

struct PlateRecord {
  String plateNumber;
  unsigned long timestamp;
  int confidence;
  String responseTime;
};

class DataLogger {
private:
  PlateRecord plateHistory[MAX_PLATE_HISTORY];
  int historyIndex = 0;
  int totalRecognitions = 0;

public:
  /**
   * Log a recognized plate
   * @param plateNumber the license plate number
   * @param confidence recognition confidence (0-100)
   * @param responseTime API response time in milliseconds
   */
  void logPlate(const String& plateNumber, int confidence = -1, unsigned long responseTime = 0) {
    PlateRecord record;
    record.plateNumber = plateNumber;
    record.timestamp = millis(); // In production, use actual RTC time
    record.confidence = confidence;
    record.responseTime = String(responseTime) + "ms";

    // Store in circular buffer
    plateHistory[historyIndex] = record;
    historyIndex = (historyIndex + 1) % MAX_PLATE_HISTORY;
    totalRecognitions++;

    if (DEBUG_MODE) Serial.printf("[LOG] Recorded plate: %s (Conf: %d%%, Time: %lums)\n", 
                                   plateNumber.c_str(), confidence, responseTime);
  }

  /**
   * Get all logged plates
   * @param count reference to store the number of valid plates
   * @return array of PlateRecord
   */
  PlateRecord* getHistory(int& count) {
    count = (totalRecognitions < MAX_PLATE_HISTORY) ? totalRecognitions : MAX_PLATE_HISTORY;
    return plateHistory;
  }

  /**
   * Get a specific plate from history
   * @param index position in history (0 = oldest)
   * @return PlateRecord at that index
   */
  PlateRecord getPlateAt(int index) {
    if (index >= 0 && index < MAX_PLATE_HISTORY) {
      return plateHistory[index];
    }
    return PlateRecord{"Unknown", 0, -1, "0ms"};
  }

  /**
   * Get total recognitions count
   * @return total number of plates recognized
   */
  int getTotalRecognitions() {
    return totalRecognitions;
  }

  /**
   * Get history size (actual valid records)
   * @return number of valid records in history
   */
  int getHistorySize() {
    return (totalRecognitions < MAX_PLATE_HISTORY) ? totalRecognitions : MAX_PLATE_HISTORY;
  }

  /**
   * Clear all logged history
   */
  void clearHistory() {
    for (int i = 0; i < MAX_PLATE_HISTORY; i++) {
      plateHistory[i] = PlateRecord{"Empty", 0, -1, "0ms"};
    }
    historyIndex = 0;
    totalRecognitions = 0;
    if (DEBUG_MODE) Serial.println("[LOG] History cleared");
  }

  /**
   * Export history as JSON string
   * @return JSON formatted string of all history records
   */
  String exportAsJSON() {
    String json = "{\"plates\":[";
    int count = getHistorySize();
    for (int i = 0; i < count; i++) {
      PlateRecord rec = plateHistory[i];
      json += "{\"plate\":\"" + rec.plateNumber + "\"";
      json += ",\"time\":\"" + rec.responseTime + "\"";
      json += ",\"confidence\":\"" + String(rec.confidence) + "\"}";
      if (i < count - 1) json += ",";
    }
    json += "],\"total\":" + String(totalRecognitions) + "}";
    return json;
  }

  /**
   * Get formatted statistics
   * @return statistics string
   */
  String getStatistics() {
    String stats = "Total Plates: " + String(totalRecognitions) + "\n";
    stats += "History Size: " + String(getHistorySize());
    return stats;
  }
};

#endif // DATA_LOGGER_H
