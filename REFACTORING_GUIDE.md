# ESP32-CAM ANPR System - Version 2.0 (Refactored)

## Overview

This is the **enhanced and modularized version** of the ESP32-CAM Automatic Number Plate Recognition system. The refactoring improves code maintainability, adds new features, and provides a solid foundation for future enhancements.

### Key Improvements

✅ **Modular Architecture**
- Split monolithic code into separate header files
- Each module handles a specific responsibility (Camera, Display, WiFi, API, etc.)
- Easy to test, maintain, and extend

✅ **Enhanced Error Handling**
- Retry logic for API connections
- Better error messages and display
- Graceful degradation when WiFi is unavailable

✅ **Data Logging**
- In-memory plate history (last 10 plates)
- Statistics tracking (total recognitions, upload counts)
- JSON export capability for future server integration

✅ **Improved Display Management**
- Status screens (WiFi, WiFi connection, results)
- Error display with codes
- Plate history viewer
- Formatted plate result display with confidence scores

✅ **Better Configuration**
- Centralized `config.h` for all constants
- Easy to swap hardware variants
- Security-ready (placeholders for NVS encrypted storage)

✅ **Debugging Support**
- Comprehensive Serial debug output
- Module-specific logging prefixes
- System statistics printing

## Project Structure

```
├── esp32_cam_anpr_v2.ino       Main sketch (refactored)
├── config.h                     Central configuration
├── src/
│   ├── camera.h                Camera initialization & capture
│   ├── display.h               OLED display management
│   ├── wifi_handler.h          WiFi connection & monitoring
│   ├── api_client.h            HTTPS API communication
│   ├── data_logger.h           Plate history & statistics
│   ├── button_handler.h        Button input with debouncing
│   └── flashlight.h            LED flashlight control
├── README_V1.md                Original version documentation
└── Code                        Original monolithic sketch
```

## How to Use

### 1. Configure Your System

Open `config.h` and update:

```cpp
// WiFi credentials
const char* WIFI_SSID = "Your_WiFi_SSID";
const char* WIFI_PASSWORD = "Your_WiFi_Password";

// API credentials
const char* API_KEY = "Your_CircuitDigest_API_Key";
```

### 2. Hardware Setup

Connections remain the same as v1:
- OLED SDA → GPIO 15
- OLED SCL → GPIO 14
- Button → GPIO 13
- Flashlight → GPIO 4 (optional)

### 3. Upload

1. Open `esp32_cam_anpr_v2.ino` in Arduino IDE
2. Select board: **ESP32-CAM**
3. Compile and upload
4. Open Serial Monitor (115200 baud) for debug output

### 4. Operation

1. System initializes camera, display, WiFi
2. Waits for WiFi connection
3. Displays "System Ready - Press Button"
4. Press button to capture and recognize plate
5. Plate number and confidence score displayed
6. Statistics printed to Serial

## Features

### Core Features
- ✅ Image capture via ESP32-CAM
- ✅ HTTPS upload to CircuitDigest API
- ✅ Number plate recognition
- ✅ OLED display results
- ✅ Serial monitor output

### New in V2.0
- ✅ Modular code architecture
- ✅ Data logging with plate history
- ✅ WiFi reconnection logic
- ✅ API retry mechanism
- ✅ Enhanced error handling
- ✅ System statistics tracking
- ✅ Configurable settings (one file)
- ✅ Debug mode with detailed logging
- ✅ Formatted display output
- ✅ Button debouncing

### Roadmap (Future Enhancements)
- [ ] NVS encrypted credential storage
- [ ] SPIFFS data logging to flash
- [ ] Plate history persistent storage
- [ ] Multiple ANPR API provider support
- [ ] Local OCR fallback (TinyML)
- [ ] OTA firmware updates
- [ ] Web dashboard
- [ ] MQTT integration
- [ ] Motion detection (PIR sensor)
- [ ] Battery monitoring (ADC)
- [ ] Webhook notifications

## Debug Output

When `DEBUG_MODE` is enabled in `config.h`, the system prints detailed information:

```
[SETUP] Initializing display...
[CAMERA] Initialization successful
[WiFi] Connecting to MyNetwork
[WiFi] Connected! IP: 192.168.1.100
[LOOP] WiFi connected. Signal: -55 dBm (4 bars)
[CAPTURE] Starting capture sequence...
[CAMERA] Frame captured: 15234 bytes
[API] Connecting to www.circuitdigest.cloud:443 (attempt 1)
[API] Server connection successful
[API] Image uploaded to server
[API] Response received (542 bytes)
[API] Plate recognized: AB1234CD
[LOG] Recorded plate: AB1234CD (Conf: 95%, Time: 2145ms)

[STATS] ================= SYSTEM STATISTICS =================
  Total Recognitions: 1
  API Uploads: 1
  WiFi Signal: -55 dBm (4 bars)
  Device IP: 192.168.1.100
  Recent Plates:
    1. AB1234CD (Conf: 95%)
========================================================
```

## Configuration Reference

### WiFi Settings
```cpp
const char* WIFI_SSID = "xxx";
const char* WIFI_PASSWORD = "xxx";
const unsigned int WIFI_TIMEOUT_MS = 20000;
const unsigned int WIFI_RECOVER_TIME_MS = 5000;
```

### API Settings
```cpp
const char* API_SERVER_NAME = "www.circuitdigest.cloud";
const char* API_KEY = "xxx";
const unsigned long API_RESPONSE_TIMEOUT = 5000;
const int API_MAX_RETRIES = 2;
```

### Pin Configuration
```cpp
#define I2C_SDA 15              // OLED data
#define I2C_SCL 14              // OLED clock
#define TRIGGER_BUTTON 13       // Capture trigger
#define FLASHLIGHT_PIN 4        // LED flashlight
#define PIR_SENSOR_PIN 12       // Motion sensor (future)
#define BATTERY_SENSE_PIN 33    // Battery level (future)
```

### System Settings
```cpp
#define DEBUG_MODE true         // Enable serial logging
#define MAX_PLATE_HISTORY 10    // Store last N plates
#define ENABLE_SPIFFS_LOGGING false  // Log to flash (future)
#define SKIP_SSL_VERIFICATION true   // Dev mode (change for production)
```

## Troubleshooting

### Display not showing
- Check SDA (GPIO 15) and SCL (GPIO 14) connections
- Verify SSD1306 address is 0x3C (shown in config.h)
- Check I2C frequency: 100kHz

### WiFi connection fails
- Verify SSID and password in config.h
- Check WiFi signal strength
- Look for reconnection attempts in Serial Monitor

### Plate not recognized
- Ensure good lighting on the plate
- Position plate in center of frame
- Verify API key is correct
- Check API response in Serial Monitor for errors

### Button press not working
- Verify GPIO 13 connection
- Check button debounce setting (50ms default)
- Confirm WiFi is connected before capturing

## Next Steps

1. **Secure credentials** - Implement NVS encryption for WiFi/API keys (see `config.h` comments)
2. **Persistent storage** - Add SPIFFS logging for audit trail
3. **Multiple APIs** - Create abstraction layer to support multiple ANPR services
4. **Local fallback** - Integrate TinyML for offline plate recognition
5. **Web interface** - Add lightweight HTTP server on ESP32
6. **Analytics** - Build backend dashboard for plate history analysis

## API Response Parsing

The system expects CircuitDigest API responses with this JSON structure:

```json
{
  "number_plate": "AB1234CD",
  "confidence": 95,
  "view_image": "https://...",
  ...
}
```

The JSON parser in `api_client.h` extracts these fields and handles missing data gracefully.

## Support & Issues

For issues or suggestions:
1. Check debug output in Serial Monitor
2. Review troubleshooting section above
3. Open an issue on GitHub with:
   - Serial Monitor output
   - config.h settings (with credentials removed)
   - Error messages displayed

## License

Same as original project. See repository for details.

---

**Version:** 2.0 (Refactored)  
**Last Updated:** 2026-08-21  
**Compatibility:** ESP32-CAM, Arduino IDE 1.8+
