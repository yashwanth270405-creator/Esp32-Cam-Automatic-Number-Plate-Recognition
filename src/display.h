/**
 * OLED Display Handler
 * Manages SSD1306 OLED display initialization and content rendering
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "config.h"

class Display {
private:
  TwoWire I2Cbus = TwoWire(0);
  Adafruit_SSD1306 display;
  bool initialized = false;

public:
  Display() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &I2Cbus, OLED_RESET) {}

  /**
   * Initialize the I2C bus and OLED display
   * @return true if initialization successful, false otherwise
   */
  bool init() {
    // Initialize I2C with custom pins
    I2Cbus.begin(I2C_SDA, I2C_SCL, I2C_FREQUENCY);
    delay(100);

    // Initialize display
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      if (DEBUG_MODE) Serial.printf("[DISPLAY] SSD1306 OLED failed to initialize.\nCheck SDA: GPIO%d, SCL: GPIO%d\n", I2C_SDA, I2C_SCL);
      return false;
    }

    display.clearDisplay();
    display.setTextSize(DISPLAY_TEXT_SIZE);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("System Init...");
    display.display();

    initialized = true;
    if (DEBUG_MODE) Serial.println("[DISPLAY] Initialization successful");
    return true;
  }

  /**
   * Display simple text message (single or multi-line)
   * @param text message to display
   */
  void showText(const String& text) {
    if (!initialized) return;
    display.clearDisplay();
    display.setCursor(0, 10);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.println(text);
    display.display();
  }

  /**
   * Display status with header and content
   * @param header section title
   * @param content main message
   * @param timestamp optional timestamp
   */
  void showStatus(const String& header, const String& content, const String& timestamp = "") {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Header
    display.setCursor(0, 0);
    display.println(header);
    display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
    
    // Content
    display.setCursor(0, 15);
    display.println(content);
    
    // Timestamp footer
    if (timestamp.length() > 0) {
      display.setTextSize(0);
      display.setCursor(0, SCREEN_HEIGHT - 8);
      display.println(timestamp);
    }
    
    display.display();
  }

  /**
   * Display recognized plate number with formatting
   * @param plateNumber the recognized license plate
   * @param confidence optional confidence score (0-100)
   */
  void showPlateResult(const String& plateNumber, int confidence = -1) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 15);
    display.println("PLATE:");
    
    display.setTextSize(3);
    display.setCursor(5, 35);
    display.println(plateNumber);
    
    if (confidence >= 0) {
      display.setTextSize(1);
      display.setCursor(30, 58);
      display.printf("Conf: %d%%", confidence);
    }
    
    display.display();
  }

  /**
   * Display WiFi connection status
   * @param ssid network name
   * @param strength signal strength indicator (0-4 bars)
   * @param connected connection status
   */
  void showWiFiStatus(const String& ssid, int strength = 0, bool connected = false) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println("WiFi:");
    display.setCursor(0, 32);
    display.println(ssid.c_str());
    display.setCursor(0, 44);
    display.println(connected ? "Connected" : "Connecting...");
    display.display();
  }

  /**
   * Display plate history (scrollable list)
   * @param plates array of plate numbers
   * @param count number of plates in array
   */
  void showPlateHistory(const String plates[], int count) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("PLATE HISTORY");
    display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);
    
    int y = 15;
    for (int i = 0; i < count && i < 5; i++) {
      display.setCursor(0, y);
      display.printf("%d. %s", i + 1, plates[i].c_str());
      y += 10;
    }
    display.display();
  }

  /**
   * Display error message
   * @param errorCode error identifier
   * @param errorMsg error description
   */
  void showError(int errorCode, const String& errorMsg) {
    if (!initialized) return;
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 20);
    display.println("ERROR");
    display.setCursor(0, 32);
    display.printf("Code: %d", errorCode);
    display.setCursor(0, 44);
    display.println(errorMsg.c_str());
    display.display();
  }

  /**
   * Clear display
   */
  void clear() {
    if (!initialized) return;
    display.clearDisplay();
    display.display();
  }

  /**
   * Check if display is initialized
   * @return true if initialized, false otherwise
   */
  bool isInitialized() {
    return initialized;
  }
};

#endif // DISPLAY_H
