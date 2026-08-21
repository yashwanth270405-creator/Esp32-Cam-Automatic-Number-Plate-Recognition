/**
 * ESP32-CAM Automatic Number Plate Recognition (ANPR) System - Main Sketch
 * 
 * Refactored Version 2.0
 * - Modular architecture with separate header files
 * - Improved error handling and retry logic
 * - Data logging with plate history
 * - Enhanced display management
 * - Secure credential storage ready (NVS)
 * 
 * GitHub: https://github.com/yashwanth270405-creator/Esp32-Cam-Automatic-Number-Plate-Recognition
 */

#include "config.h"
#include "src/camera.h"
#include "src/display.h"
#include "src/wifi_handler.h"
#include "src/api_client.h"
#include "src/data_logger.h"
#include "src/button_handler.h"
#include "src/flashlight.h"

// ========== GLOBAL OBJECTS ==========
Camera camera;
Display display;
WiFiHandler wiFi;
APIClient apiClient;
DataLogger dataLogger;
ButtonHandler button(TRIGGER_BUTTON);
Flashlight flashlight(FLASHLIGHT_PIN);

// ========== SYSTEM STATE ==========
enum SystemState {
  STATE_INITIALIZING,
  STATE_READY,
  STATE_WIFI_CONNECTING,
  STATE_CAPTURING,
  STATE_PROCESSING,
  STATE_ERROR
};

SystemState currentState = STATE_INITIALIZING;
unsigned long lastStatusCheck = 0;
const unsigned long STATUS_CHECK_INTERVAL = 5000; // 5 seconds

// ========== SETUP ==========
void setup() {
  // Disable brownout detector for stability
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
  
  Serial.begin(SERIAL_BAUD_RATE);
  delay(1000);

  if (DEBUG_MODE) {
    Serial.println("\n\n================================");
    Serial.println("ESP32-CAM ANPR System v" + String(SYSTEM_VERSION));
    Serial.println("================================\n");
  }

  // Initialize display
  if (DEBUG_MODE) Serial.println("[SETUP] Initializing display...");
  if (!display.init()) {
    Serial.println("[FATAL] Display initialization failed!");
    while (true) {
      delay(1000);
    }
  }
  display.showText("ANPR System\nStarting...");
  delay(500);

  // Initialize button and flashlight
  if (DEBUG_MODE) Serial.println("[SETUP] Initializing GPIO...");
  button.init();
  flashlight.init(false); // false = simple on/off mode
  flashlight.off();
  delay(200);

  // Initialize camera
  if (DEBUG_MODE) Serial.println("[SETUP] Initializing camera...");
  display.showText("Initializing\nCamera...");
  if (!camera.init()) {
    display.showError(1, "Camera init failed");
    Serial.println("[FATAL] Camera initialization failed!");
    while (true) {
      delay(1000);
    }
  }
  delay(300);

  // Initialize WiFi
  if (DEBUG_MODE) Serial.println("[SETUP] Initializing WiFi...");
  currentState = STATE_WIFI_CONNECTING;
  display.showWiFiStatus(String(WIFI_SSID), 0, false);
  
  if (!wiFi.init(WIFI_SSID, WIFI_PASSWORD)) {
    display.showError(2, "WiFi connection failed");
    Serial.println("[WARNING] WiFi failed, will retry in loop");
  } else {
    currentState = STATE_READY;
  }

  delay(500);
  display.showText("System Ready\nPress Button");
  delay(1000);

  if (DEBUG_MODE) Serial.println("[SETUP] Initialization complete!\n");
}

// ========== MAIN LOOP ==========
void loop() {
  // Check WiFi connection periodically
  unsigned long now = millis();
  if (now - lastStatusCheck > STATUS_CHECK_INTERVAL) {
    lastStatusCheck = now;
    if (!wiFi.isConnected()) {
      if (DEBUG_MODE) Serial.println("[LOOP] WiFi disconnected, attempting reconnection...");
      display.showWiFiStatus(String(WIFI_SSID), wiFi.getSignalBars(), false);
      wiFi.checkConnection();
    } else {
      if (DEBUG_MODE) Serial.printf("[LOOP] WiFi connected. Signal: %d dBm (%d bars)\n", 
                                     wiFi.getSignalStrength(), wiFi.getSignalBars());
    }
  }

  // Check button press
  if (button.checkPress()) {
    if (wiFi.isConnected()) {
      handleCapture();
    } else {
      display.showError(3, "WiFi not connected");
      if (DEBUG_MODE) Serial.println("[LOOP] Capture requested but WiFi not connected");
      delay(DISPLAY_RESULT_DURATION_MS);
      display.showText("System Ready\nPress Button");
    }
  }

  delay(50); // Small delay to prevent watchdog trigger
}

// ========== CAPTURE AND PROCESS HANDLER ==========
void handleCapture() {
  if (DEBUG_MODE) Serial.println("\n[CAPTURE] Starting capture sequence...");
  currentState = STATE_CAPTURING;

  display.showText("Capturing\nImage...");
  delay(200);

  // Turn on flashlight (optional)
  // flashlight.flash(FLASH_DURATION_MS);

  // Capture frame
  camera_fb_t* frame = camera.captureFrame();
  if (!frame) {
    if (DEBUG_MODE) Serial.println("[CAPTURE] Frame capture failed");
    display.showError(10, "Camera capture failed");
    camera.releaseFrame();
    delay(DISPLAY_RESULT_DURATION_MS);
    display.showText("System Ready\nPress Button");
    return;
  }

  display.showText("Connecting to\nServer...");
  if (DEBUG_MODE) Serial.println("[CAPTURE] Frame captured, sending to API...");

  currentState = STATE_PROCESSING;

  // Send to API
  APIResponse apiResponse = apiClient.sendPhoto(frame);
  camera.releaseFrame(); // Release frame buffer after sending

  if (DEBUG_MODE) Serial.printf("[CAPTURE] API Response - Success: %s, Time: %lums\n", 
                                 apiResponse.success ? "true" : "false", apiResponse.responseTime);

  // Display result
  if (apiResponse.success) {
    if (DEBUG_MODE) Serial.printf("[CAPTURE] Plate recognized: %s (Confidence: %d%%)\n", 
                                   apiResponse.plateNumber.c_str(), apiResponse.confidence);
    
    // Log the result
    dataLogger.logPlate(apiResponse.plateNumber, apiResponse.confidence, apiResponse.responseTime);
    
    // Display result
    display.showPlateResult(apiResponse.plateNumber, apiResponse.confidence);
    delay(DISPLAY_RESULT_DURATION_MS);
  } else {
    if (DEBUG_MODE) Serial.printf("[CAPTURE] API Error: %s\n", apiResponse.errorMessage.c_str());
    display.showError(11, apiResponse.errorMessage);
    delay(DISPLAY_RESULT_DURATION_MS);
  }

  currentState = STATE_READY;
  display.showText("System Ready\nPress Button");

  // Print statistics
  printSystemStats();
}

// ========== HELPER FUNCTIONS ==========

/**
 * Print current system statistics to serial
 */
void printSystemStats() {
  if (DEBUG_MODE) {
    Serial.println("\n[STATS] ================= SYSTEM STATISTICS =================");
    Serial.printf("  Total Recognitions: %d\n", dataLogger.getTotalRecognitions());
    Serial.printf("  API Uploads: %d\n", apiClient.getUploadCount());
    Serial.printf("  WiFi Signal: %d dBm (%d bars)\n", wiFi.getSignalStrength(), wiFi.getSignalBars());
    Serial.printf("  Device IP: %s\n", wiFi.getLocalIP().c_str());
    
    if (dataLogger.getHistorySize() > 0) {
      Serial.println("  Recent Plates:");
      int count = dataLogger.getHistorySize();
      for (int i = 0; i < count; i++) {
        PlateRecord rec = dataLogger.getPlateAt(i);
        Serial.printf("    %d. %s (Conf: %d%%)\n", i + 1, rec.plateNumber.c_str(), rec.confidence);
      }
    }
    Serial.println("========================================================\n");
  }
}

/**
 * Get human-readable system state
 */
String getStateName(SystemState state) {
  switch (state) {
    case STATE_INITIALIZING: return "Initializing";
    case STATE_READY: return "Ready";
    case STATE_WIFI_CONNECTING: return "WiFi Connecting";
    case STATE_CAPTURING: return "Capturing";
    case STATE_PROCESSING: return "Processing";
    case STATE_ERROR: return "Error";
    default: return "Unknown";
  }
}
