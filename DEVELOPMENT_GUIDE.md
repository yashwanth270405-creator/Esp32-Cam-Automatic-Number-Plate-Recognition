# ESP32-CAM ANPR System - Development Setup Guide

## Quick Start for Development

This guide helps you set up a complete development environment with mock servers for testing without hardware.

## Prerequisites

- Arduino IDE 1.8 or later
- ESP32 board package installed
- Python 3.7+ (for mock server)
- USB cable for ESP32-CAM programming

## Step 1: Set Up Mock API Server (Optional)

For development without real CircuitDigest API:

### Install Dependencies

```bash
pip install flask
```

### Start Mock Server

```bash
python mock_api_server.py
```

Server runs on `http://localhost:5000`

## Step 2: Configure for Development

Open `config.h` and update:

```cpp
// For local testing:
const char* API_SERVER_NAME = "192.168.1.YOUR_PC_IP";  // Get with: ipconfig (Windows) or ifconfig (Linux/Mac)
const int API_SERVER_PORT = 5000;

// For production (CircuitDigest):
// const char* API_SERVER_NAME = "www.circuitdigest.cloud";
// const int API_SERVER_PORT = 443;

// WiFi
const char* WIFI_SSID = "Your_Network";
const char* WIFI_PASSWORD = "Your_Password";
const char* API_KEY = "test-key-for-development";

// Enable debug output
#define DEBUG_MODE true

// Skip SSL verification (development only!)
#define SKIP_SSL_VERIFICATION true
```

## Step 3: Run Unit Tests

Test core logic on your computer before deploying to ESP32:

```bash
# Linux/Mac
g++ -o test_anpr test_anpr.cpp && ./test_anpr

# Windows (using MinGW or MSVC)
g++ -o test_anpr.exe test_anpr.cpp && test_anpr.exe
```

Expected output:
```
==================================================
ESP32-CAM ANPR System - Unit Tests
==================================================

[TEST] JSON Parsing
  ✓ String extraction: AB1234CD
  ✓ Number extraction: 95
  ✓ Missing key handling: empty string
  ✓ Complex JSON parsing: XY9876ZZ
  ✓ Invalid JSON handling: empty string

[TEST] Data Logger
  ✓ Log single plate: 1 total
  ✓ Log multiple plates: 3 total
  ✓ Retrieve from history: AB1234CD (95%)
  ✓ Circular buffer overflow: 10 max (stored 15)
  ✓ History wrapping: oldest entry is TEST10

[TEST] Button Debounce Logic
  ✓ Noise rejection: glitch filtered
  ✓ Valid press detection: 1 press(es) detected

[TEST] Image Chunking Logic
  ✓ Chunk count: 15234 bytes -> 15 chunks of 1024 bytes
  ✓ Last chunk size: 834 bytes
  ✓ Exact multiple: 10240 -> 10 chunks (no remainder)

==================================================
✅ All tests passed!
==================================================
```

## Step 4: Upload to ESP32-CAM

1. Open `esp32_cam_anpr_v2.ino` in Arduino IDE
2. Install required libraries via Library Manager:
   - Adafruit GFX Library
   - Adafruit SSD1306
3. Select: Tools → Board → ESP32-CAM
4. Select your COM port
5. Click Upload
6. Open Serial Monitor (115200 baud) to see debug output

## Step 5: Test the System

### With Mock Server

1. Start mock API server: `python mock_api_server.py`
2. ESP32-CAM connects to WiFi
3. Press button on ESP32-CAM
4. Mock server returns random plate number
5. OLED displays result
6. Check logs: `http://localhost:5000/requests`

### With Real API

1. Update `config.h` with CircuitDigest server details
2. Upload sketch
3. Press button and test with actual vehicles

## Development Workflow

### Making Changes

1. Edit header files in `src/` or `config.h`
2. Run unit tests: `g++ -o test_anpr test_anpr.cpp && ./test_anpr`
3. If tests pass, upload to ESP32-CAM
4. Monitor Serial output for issues

### Adding New Features

1. Create new header file in `src/` (e.g., `src/motion_sensor.h`)
2. Implement class following existing patterns
3. Add to main sketch: `#include "src/motion_sensor.h"`
4. Create corresponding unit tests
5. Update `config.h` with new settings

## Serial Monitor Commands (Future)

For interactive debugging (requires additional code):

```
> stats              - Print system statistics
> history            - Show plate history
> reset              - Clear history
> wifi status        - WiFi connection info
> camera test        - Capture test image
> api test           - Test API connection
```

## Troubleshooting

### Mock Server Issues

```bash
# Connection refused
- Ensure firewall allows port 5000
- Check your PC IP matches config.h
- Run: python mock_api_server.py -v

# Module not found
- Install Flask: pip install flask
- Use venv: python -m venv env && source env/bin/activate
```

### Serial Monitor Issues

```bash
# No output
- Select correct COM port
- Check baud rate: 115200
- Ensure USB cable is data cable (not power-only)
- Try: Press EN button on ESP32

# Garbled output
- Change baud rate to 115200
- Check USB cable quality
```

### WiFi Issues

- Check SSID and password in config.h
- Verify ESP32 is within range
- See Serial Monitor for connection attempts
- Try power cycle of ESP32

## Next Steps

1. **Implement HTTPS** - Add proper certificate validation
2. **Add NVS Storage** - Encrypt credentials in flash
3. **Extend API Support** - Add multiple ANPR providers
4. **Build Web Dashboard** - Monitor plates remotely
5. **Add Sensors** - Temperature, battery, motion detection

## Resources

- [ESP32 Documentation](https://docs.espressif.com/projects/esp-idf/)
- [Arduino IDE Docs](https://www.arduino.cc/en/Guide)
- [CircuitDigest API Docs](https://www.circuitdigest.com/)
- [Adafruit SSD1306 Guide](https://learn.adafruit.com/)

## Support

For issues:
1. Check Serial Monitor debug output
2. Run unit tests to verify logic
3. Consult troubleshooting section
4. Open GitHub issue with details

---

**Last Updated:** 2026-08-21  
**Version:** 2.0 Development Guide
