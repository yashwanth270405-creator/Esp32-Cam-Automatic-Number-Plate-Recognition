/**
 * Button Handler
 * Manages button input with debouncing
 */

#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
#include "config.h"

class ButtonHandler {
private:
  int buttonPin;
  bool lastState = LOW;
  unsigned long lastDebounceTime = 0;
  bool buttonPressed = false;

public:
  ButtonHandler(int pin) : buttonPin(pin) {}

  /**
   * Initialize button GPIO
   */
  void init() {
    pinMode(buttonPin, INPUT);
    if (DEBUG_MODE) Serial.printf("[BUTTON] Initialized on GPIO %d\n", buttonPin);
  }

  /**
   * Check if button is pressed (call in loop)
   * @return true if button press detected, false otherwise
   */
  bool checkPress() {
    int currentState = digitalRead(buttonPin);

    // Debounce logic
    if (currentState != lastState) {
      lastDebounceTime = millis();
    }

    if ((millis() - lastDebounceTime) > BUTTON_DEBOUNCE_MS) {
      if (currentState == HIGH && lastState == LOW) {
        buttonPressed = true;
        if (DEBUG_MODE) Serial.println("[BUTTON] Press detected");
        lastState = currentState;
        return true;
      }
      lastState = currentState;
    }

    return false;
  }

  /**
   * Get raw button state (not debounced)
   * @return current GPIO state
   */
  bool getRawState() {
    return digitalRead(buttonPin) == HIGH;
  }
};

#endif // BUTTON_HANDLER_H
