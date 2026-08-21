/**
 * API Client
 * Handles HTTPS communication with CircuitDigest cloud service and response parsing
 */

#ifndef API_CLIENT_H
#define API_CLIENT_H

#include <Arduino.h>
#include <WiFiClientSecure.h>
#include "config.h"
#include "camera.h"

struct APIResponse {
  bool success = false;
  String plateNumber = "";
  String imageLink = "";
  int confidence = -1;
  String errorMessage = "";
  unsigned long responseTime = 0;
};

class APIClient {
private:
  WiFiClientSecure client;
  int uploadCount = 0;

public:
  APIClient() {
    // Configure SSL for development (disable in production)
    if (SKIP_SSL_VERIFICATION) {
      client.setInsecure();
    }
  }

  /**
   * Extract JSON string value by key
   * Simple JSON parser for specific key extraction
   * @param jsonString complete JSON response
   * @param key the key to search for (with quotes)
   * @return extracted value or empty string if not found
   */
  String extractJsonValue(const String& jsonString, const String& key) {
    int keyIndex = jsonString.indexOf(key);
    if (keyIndex == -1) {
      return "";
    }

    // Find the colon after the key
    int colonIndex = jsonString.indexOf(':', keyIndex);
    if (colonIndex == -1) {
      return "";
    }

    // Find the opening quote
    int startIndex = jsonString.indexOf('"', colonIndex) + 1;
    if (startIndex == 0) {
      return "";
    }

    // Find the closing quote
    int endIndex = jsonString.indexOf('"', startIndex);
    if (endIndex == -1) {
      return "";
    }

    return jsonString.substring(startIndex, endIndex);
  }

  /**
   * Extract JSON numeric value by key
   * @param jsonString complete JSON response
   * @param key the key to search for (with quotes)
   * @return extracted numeric value or -1 if not found
   */
  int extractJsonNumber(const String& jsonString, const String& key) {
    int keyIndex = jsonString.indexOf(key);
    if (keyIndex == -1) {
      return -1;
    }

    int colonIndex = jsonString.indexOf(':', keyIndex);
    if (colonIndex == -1) {
      return -1;
    }

    // Skip whitespace and get the number
    int numStart = colonIndex + 1;
    while (numStart < jsonString.length() && jsonString[numStart] == ' ') {
      numStart++;
    }

    int numEnd = numStart;
    while (numEnd < jsonString.length() && isdigit(jsonString[numEnd])) {
      numEnd++;
    }

    if (numEnd > numStart) {
      return jsonString.substring(numStart, numEnd).toInt();
    }
    return -1;
  }

  /**
   * Send photo to API server for number plate recognition
   * @param frame camera frame buffer
   * @return APIResponse struct with results
   */
  APIResponse sendPhoto(camera_fb_t* frame) {
    APIResponse response;
    unsigned long startTime = millis();

    if (!frame) {
      response.success = false;
      response.errorMessage = "Invalid frame buffer";
      if (DEBUG_MODE) Serial.println("[API] Invalid frame buffer");
      return response;
    }

    // Attempt connection with retries
    int retries = 0;
    while (retries < API_MAX_RETRIES) {
      if (DEBUG_MODE) Serial.printf("[API] Connecting to %s:%d (attempt %d)\n", API_SERVER_NAME, API_SERVER_PORT, retries + 1);

      if (client.connect(API_SERVER_NAME, API_SERVER_PORT)) {
        if (DEBUG_MODE) Serial.println("[API] Server connection successful");
        break;
      }

      retries++;
      if (retries < API_MAX_RETRIES) {
        delay(1000);
      }
    }

    if (!client.connected()) {
      response.success = false;
      response.errorMessage = "Failed to connect to server";
      if (DEBUG_MODE) Serial.println("[API] Connection failed after retries");
      return response;
    }

    // Prepare request
    uploadCount++;
    String filename = String(API_KEY) + "_" + String(uploadCount) + ".jpeg";
    String head = "--CircuitDigest\r\nContent-Disposition: form-data; name=\"imageFile\"; filename=\"" + filename + "\"\r\nContent-Type: image/jpeg\r\n\r\n";
    String tail = "\r\n--CircuitDigest--\r\n";

    uint32_t imageLen = frame->len;
    uint32_t extraLen = head.length() + tail.length();
    uint32_t totalLen = imageLen + extraLen;

    // Send HTTP request
    client.println("POST " + String(API_PATH) + " HTTP/1.1");
    client.println("Host: " + String(API_SERVER_NAME));
    client.println("Content-Length: " + String(totalLen));
    client.println("Content-Type: multipart/form-data; boundary=CircuitDigest");
    client.println("Authorization:" + String(API_KEY));
    client.println();

    client.print(head);

    // Send image data in chunks
    uint8_t* fbBuf = frame->buf;
    size_t fbLen = frame->len;
    for (size_t n = 0; n < fbLen; n += 1024) {
      if (n + 1024 < fbLen) {
        client.write(fbBuf, 1024);
        fbBuf += 1024;
      } else {
        size_t remainder = fbLen % 1024;
        client.write(fbBuf, remainder);
      }
    }

    client.print(tail);
    if (DEBUG_MODE) Serial.println("[API] Image uploaded to server");

    // Wait for response
    String apiResponse;
    unsigned long responseStartTime = millis();

    while (client.connected() && millis() - responseStartTime < API_RESPONSE_TIMEOUT) {
      if (client.available()) {
        char c = client.read();
        apiResponse += c;
      }
    }

    client.stop();
    response.responseTime = millis() - startTime;

    if (apiResponse.length() == 0) {
      response.success = false;
      response.errorMessage = "No response from server";
      if (DEBUG_MODE) Serial.println("[API] No response received");
      return response;
    }

    if (DEBUG_MODE) Serial.printf("[API] Response received (%d bytes)\n", apiResponse.length());
    if (DEBUG_MODE) Serial.println(apiResponse);

    // Parse response
    response.plateNumber = extractJsonValue(apiResponse, "\"number_plate\"");
    response.imageLink = extractJsonValue(apiResponse, "\"view_image\"");
    response.confidence = extractJsonNumber(apiResponse, "\"confidence\"");

    if (response.plateNumber.length() > 0) {
      response.success = true;
      if (DEBUG_MODE) Serial.printf("[API] Plate recognized: %s\n", response.plateNumber.c_str());
    } else {
      response.success = false;
      response.errorMessage = "Plate not recognized in response";
      if (DEBUG_MODE) Serial.println("[API] Plate extraction failed");
    }

    return response;
  }

  /**
   * Get total upload count
   * @return number of uploads performed
   */
  int getUploadCount() {
    return uploadCount;
  }
};

#endif // API_CLIENT_H
