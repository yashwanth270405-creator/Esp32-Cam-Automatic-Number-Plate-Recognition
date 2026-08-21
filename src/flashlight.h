/**
 * Flashlight Handler
 * Manages LED flashlight control with optional PWM brightness
 */

#ifndef FLASHLIGHT_H
#define FLASHLIGHT_H

#include <Arduino.h>
#include "config.h"

class Flashlight {
private:
  int ledPin;
  bool isPWM = false;
  int pwmChannel = 0;
  int pwmFrequency = 1000; // 1 kHz
  int pwmResolution = 8;   // 8-bit (0-255)

public:
  Flashlight(int pin) : ledPin(pin) {}

  /**
   * Initialize flashlight (simple GPIO or PWM)
   * @param usePWM true to use PWM for brightness control, false for simple on/off
   */
  void init(bool usePWM = false) {
    isPWM = usePWM;

    if (isPWM) {
      // Configure PWM channel
      ledcSetup(pwmChannel, pwmFrequency, pwmResolution);
      ledcAttachPin(ledPin, pwmChannel);
      if (DEBUG_MODE) Serial.printf("[FLASHLIGHT] PWM mode on GPIO %d\n", ledPin);
    } else {
      pinMode(ledPin, OUTPUT);
      digitalWrite(ledPin, LOW);
      if (DEBUG_MODE) Serial.printf("[FLASHLIGHT] Digital mode on GPIO %d\n", ledPin);
    }
  }

  /**
   * Turn flashlight on (full brightness)
   */
  void on() {
    if (isPWM) {
      ledcWrite(pwmChannel, 255); // Full brightness
    } else {
      digitalWrite(ledPin, HIGH);
    }
    if (DEBUG_MODE) Serial.println("[FLASHLIGHT] ON");
  }

  /**
   * Turn flashlight off
   */
  void off() {
    if (isPWM) {
      ledcWrite(pwmChannel, 0);
    } else {
      digitalWrite(ledPin, LOW);
    }
    if (DEBUG_MODE) Serial.println("[FLASHLIGHT] OFF");
  }

  /**
   * Set brightness (PWM mode only)
   * @param brightness value 0-255 (0 = off, 255 = full)
   */
  void setBrightness(int brightness) {
    if (!isPWM) {
      if (DEBUG_MODE) Serial.println("[FLASHLIGHT] PWM not enabled");
      return;
    }

    brightness = constrain(brightness, 0, 255);
    ledcWrite(pwmChannel, brightness);
    if (DEBUG_MODE) Serial.printf("[FLASHLIGHT] Brightness: %d/255\n", brightness);
  }

  /**
   * Flash for a specific duration
   * @param durationMs flash duration in milliseconds
   */
  void flash(unsigned long durationMs = FLASH_DURATION_MS) {
    on();
    delay(durationMs);
    off();
  }
};

#endif // FLASHLIGHT_H
