/**
 * Configuration Header
 * Central place for all constants, pins, and settings
 */

#ifndef CONFIG_H
#define CONFIG_H

// ========== WIFI CONFIGURATION ==========
// Use NVS (Non-Volatile Storage) to store credentials securely
// For now, use placeholders - move to NVS in production
const char* WIFI_SSID = "xxx";           // Replace with your WiFi SSID
const char* WIFI_PASSWORD = "xxx";       // Replace with your WiFi Password
const unsigned int WIFI_TIMEOUT_MS = 20000; // 20 seconds
const unsigned int WIFI_RECOVER_TIME_MS = 5000; // 5 seconds between retries

// ========== API CONFIGURATION ==========
const char* API_SERVER_NAME = "www.circuitdigest.cloud";
const char* API_PATH = "/api/v1/readnumberplate";
const int API_SERVER_PORT = 443;
const char* API_KEY = "xxx";  // Replace with your CircuitDigest API key
const unsigned long API_RESPONSE_TIMEOUT = 5000; // 5 seconds
const int API_MAX_RETRIES = 2;

// ========== PIN CONFIGURATION ==========
// I2C Pins (OLED Display)
#define I2C_SDA 15
#define I2C_SCL 14
const unsigned long I2C_FREQUENCY = 100000; // 100 kHz

// GPIO Pins
#define TRIGGER_BUTTON 13
#define FLASHLIGHT_PIN 4
#define PIR_SENSOR_PIN 12      // Optional: PIR sensor for motion detection
#define BATTERY_SENSE_PIN 33   // Optional: ADC pin for battery monitoring

// ========== CAMERA CONFIGURATION ==========
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Camera settings
#define CAMERA_XCLK_FREQ 20000000  // 20 MHz
#define CAMERA_PIXEL_FORMAT PIXFORMAT_JPEG
#define CAMERA_WITH_PSRAM_FRAME_SIZE FRAMESIZE_SVGA
#define CAMERA_WITH_PSRAM_JPEG_QUALITY 5   // Lower = higher quality
#define CAMERA_WITH_PSRAM_FB_COUNT 2
#define CAMERA_NO_PSRAM_FRAME_SIZE FRAMESIZE_CIF
#define CAMERA_NO_PSRAM_JPEG_QUALITY 12
#define CAMERA_NO_PSRAM_FB_COUNT 1

// ========== OLED DISPLAY CONFIGURATION ==========
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
#define DISPLAY_TEXT_SIZE 1
#define DISPLAY_REFRESH_DELAY 500 // milliseconds

// ========== DATA LOGGING CONFIGURATION ==========
#define MAX_PLATE_HISTORY 10
#define LOG_TIMESTAMP_FORMAT "%Y-%m-%d %H:%M:%S"
#define NVS_NAMESPACE "anpr_system"
#define ENABLE_SPIFFS_LOGGING false  // Set to true to log to SPIFFS
#define SPIFFS_LOG_PATH "/anpr_log.json"

// ========== SECURITY CONFIGURATION ==========
#define ENABLE_HTTPS_PINNING false  // Set to true for production
#define SKIP_SSL_VERIFICATION true  // For development - disable in production
#define NVS_ENCRYPTION_ENABLED false // Set to true for encrypted credential storage

// ========== SYSTEM CONFIGURATION ==========
#define SERIAL_BAUD_RATE 115200
#define BROWNOUT_DETECTOR_ENABLED false  // Disable for stability
#define SYSTEM_VERSION "2.0.0"
#define DEBUG_MODE true

// ========== TIMING CONFIGURATION ==========
#define BUTTON_DEBOUNCE_MS 50
#define FLASH_DURATION_MS 100
#define DISPLAY_RESULT_DURATION_MS 3000

#endif // CONFIG_H
