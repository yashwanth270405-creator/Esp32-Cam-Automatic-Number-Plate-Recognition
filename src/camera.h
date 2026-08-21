/**
 * Camera Handler
 * Manages ESP32-CAM initialization, configuration, and frame capture
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <Arduino.h>
#include "esp_camera.h"
#include "config.h"

class Camera {
private:
  bool initialized = false;
  camera_fb_t* frameBuffer = NULL;

public:
  /**
   * Initialize the camera with appropriate settings based on PSRAM availability
   * @return true if initialization successful, false otherwise
   */
  bool init() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = CAMERA_XCLK_FREQ;
    config.pixel_format = CAMERA_PIXEL_FORMAT;

    // Adjust frame size and quality based on PSRAM availability
    if (psramFound()) {
      config.frame_size = CAMERA_WITH_PSRAM_FRAME_SIZE;
      config.jpeg_quality = CAMERA_WITH_PSRAM_JPEG_QUALITY;
      config.fb_count = CAMERA_WITH_PSRAM_FB_COUNT;
      if (DEBUG_MODE) Serial.println("[CAMERA] PSRAM found - using high quality settings");
    } else {
      config.frame_size = CAMERA_NO_PSRAM_FRAME_SIZE;
      config.jpeg_quality = CAMERA_NO_PSRAM_JPEG_QUALITY;
      config.fb_count = CAMERA_NO_PSRAM_FB_COUNT;
      if (DEBUG_MODE) Serial.println("[CAMERA] PSRAM not found - using low memory settings");
    }

    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      if (DEBUG_MODE) Serial.printf("[CAMERA] Initialization failed with error 0x%x\n", err);
      return false;
    }
    initialized = true;
    if (DEBUG_MODE) Serial.println("[CAMERA] Initialization successful");
    return true;
  }

  /**
   * Capture a JPEG frame from the camera
   * @return pointer to frame buffer if successful, NULL otherwise
   */
  camera_fb_t* captureFrame() {
    if (!initialized) {
      if (DEBUG_MODE) Serial.println("[CAMERA] Cannot capture - camera not initialized");
      return NULL;
    }

    frameBuffer = esp_camera_fb_get();
    if (!frameBuffer) {
      if (DEBUG_MODE) Serial.println("[CAMERA] Frame capture failed");
      return NULL;
    }
    if (DEBUG_MODE) Serial.printf("[CAMERA] Frame captured: %d bytes\n", frameBuffer->len);
    return frameBuffer;
  }

  /**
   * Release the frame buffer back to the camera driver
   */
  void releaseFrame() {
    if (frameBuffer != NULL) {
      esp_camera_fb_return(frameBuffer);
      frameBuffer = NULL;
    }
  }

  /**
   * Get the current frame buffer
   * @return pointer to current frame buffer or NULL
   */
  camera_fb_t* getFrameBuffer() {
    return frameBuffer;
  }

  /**
   * Check if camera is initialized
   * @return true if initialized, false otherwise
   */
  bool isInitialized() {
    return initialized;
  }
};

#endif // CAMERA_H
