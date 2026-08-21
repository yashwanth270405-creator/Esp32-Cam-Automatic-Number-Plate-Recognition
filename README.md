# Esp32-Cam-Automatic-Number-Plate-Recognition
ESP32-CAM based Automatic Number Plate Recognition (ANPR) system that captures vehicle images, processes the number plate using a cloud-based recognition service, and displays the recognized license plate number on an OLED display.
# ESP32-CAM Automatic Number Plate Recognition (ANPR)

## 📌 Project Overview

This project implements an **Automatic Number Plate Recognition (ANPR)** system using an **ESP32-CAM**. The camera captures an image of a vehicle's number plate and sends it through a secure HTTPS connection to the CircuitDigest cloud API. The cloud server processes the image using machine-learning-based recognition and returns the detected number plate information.

The recognized number plate is displayed on a **0.96-inch OLED display** connected to the ESP32-CAM.

## ✨ Features

- 📷 Vehicle image capture using ESP32-CAM
- 🔍 Automatic number plate recognition
- ☁️ Cloud-based image processing
- 🔐 HTTPS communication with the recognition server
- 📺 OLED display for displaying results
- 🔘 Push-button triggered scanning
- 💡 Flashlight support for image capture
- 📡 Wi-Fi connectivity
- 🖥️ Serial Monitor output for debugging

## 🧰 Hardware Requirements

- ESP32-CAM
- 0.96-inch OLED display (SSD1306)
- Push button
- 10K resistor
- USB-to-UART converter
- Breadboard
- Jumper wires
- Optional flashlight/LED

The original project specifies GPIO14 and GPIO15 for the OLED I2C connection and GPIO13 for the trigger button. citeturn0view0

## 🔌 Pin Connections

| Component | ESP32-CAM |
|---|---|
| OLED SDA | GPIO 15 |
| OLED SCL | GPIO 14 |
| OLED VCC | 3.3V |
| OLED GND | GND |
| Push Button | GPIO 13 |
| Flashlight/LED | GPIO 4 |
| USB-UART TX | ESP32-CAM RX |
| USB-UART RX | ESP32-CAM TX |
| USB-UART VCC | 5V |
| USB-UART GND | GND |

**Note:** Check your particular ESP32-CAM board before connecting power, as board variants can differ.

## 💻 Software Requirements

- Arduino IDE
- ESP32 board package for Arduino IDE
- ESP32-CAM board
- USB-to-UART programmer
- Wi-Fi network
- CircuitDigest Cloud API key

### Required Arduino Libraries

Install the following libraries through the Arduino IDE Library Manager:

- `Adafruit GFX Library`
- `Adafruit SSD1306`

The project also uses the ESP32 libraries for Wi-Fi, HTTPS communication, camera operation, and I2C. citeturn0view0

## 🔑 API Key Setup

The ESP32-CAM communicates with the CircuitDigest cloud server through an API.

Before running the project:

1. Create/obtain a CircuitDigest Cloud API key.
2. Open the Arduino source code.
3. Enter your Wi-Fi SSID.
4. Enter your Wi-Fi password.
5. Enter your API key.
6. Keep the server endpoint configured according to the project/API documentation.

## 🚀 Uploading the Code

1. Connect the ESP32-CAM to a USB-to-UART converter.
2. Connect TX and RX correctly:
   - USB-UART TX → ESP32-CAM RX
   - USB-UART RX → ESP32-CAM TX
3. Connect GND.
4. Connect 5V power.
5. Put the ESP32-CAM into programming mode if required by your board.
6. Open the project in Arduino IDE.
7. Select the appropriate ESP32-CAM board.
8. Select the correct COM port.
9. Enter your Wi-Fi credentials and API key.
10. Compile and upload the program.
11. Return the ESP32-CAM to normal operating mode.

## ▶️ How to Use

After successful programming:

1. Power on the ESP32-CAM.
2. The device connects to the configured Wi-Fi network.
3. The OLED displays the system initialization status.
4. Position the camera toward a vehicle number plate.
5. Press the trigger button.
6. The ESP32-CAM captures an image.
7. The image is uploaded to the cloud server.
8. The server performs number plate recognition.
9. The ESP32-CAM receives the response.
10. The recognized number plate is displayed on the OLED.
11. The result is also available through the Serial Monitor.

The recognition time depends on network speed and server response time. Clear, properly focused number-plate images improve OCR accuracy. citeturn0view0

## 🛠️ Troubleshooting

### OLED is not displaying anything

Check:

- SDA → GPIO15
- SCL → GPIO14
- VCC and GND
- OLED I2C address (`0x3C`)
- SSD1306 library installation

### ESP32-CAM cannot connect to Wi-Fi

Check:

- Wi-Fi SSID
- Wi-Fi password
- Wi-Fi availability
- ESP32-CAM power supply

### Camera capture fails

Check:

- Camera module connection
- ESP32-CAM board selection
- Available PSRAM
- Power supply stability

### Server connection fails

Check:

- Internet connection
- API key
- Server/API endpoint
- HTTPS connection
- Network speed

### Number plate is incorrectly detected

Make sure:

- The camera is properly focused.
- The number plate is clearly visible.
- There is sufficient lighting.
- The plate occupies enough of the captured image.
- The image is not blurred.


## ⚠️ Security Notes

- Do not commit your Wi-Fi password.
- Do not commit your API key.
- Use placeholders in public source code.
- Avoid sharing sensitive API responses publicly.
- If an API key is accidentally uploaded to GitHub, revoke and regenerate it.


## 👨‍💻 Project

**Project:** ESP32-CAM Automatic Number Plate Recognition  
**Platform:** ESP32-CAM  
**Programming Language:** C/C++ (Arduino)  
**Recognition:** Cloud-based AI/OCR API  
**Display:** 0.96" SSD1306 OLED  
**Communication:** Wi-Fi / HTTPS
