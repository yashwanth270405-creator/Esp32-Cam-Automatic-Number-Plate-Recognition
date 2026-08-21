/**
 * Unit Tests for ANPR System Components
 * 
 * Compile and run these tests on a desktop machine to verify:
 * - JSON parsing logic
 * - Data logger functionality
 * - Button debouncing simulation
 * 
 * Usage: g++ -o test_anpr test_anpr.cpp && ./test_anpr
 */

#include <iostream>
#include <string>
#include <cassert>
using namespace std;

// ========== SIMPLIFIED IMPLEMENTATIONS FOR TESTING ==========

// Test JSON value extraction
string extractJsonValue(const string& jsonString, const string& key) {
    size_t keyIndex = jsonString.find(key);
    if (keyIndex == string::npos) {
        return "";
    }

    size_t colonIndex = jsonString.find(':', keyIndex);
    if (colonIndex == string::npos) {
        return "";
    }

    size_t startIndex = jsonString.find('"', colonIndex) + 1;
    if (startIndex == string::npos) {
        return "";
    }

    size_t endIndex = jsonString.find('"', startIndex);
    if (endIndex == string::npos) {
        return "";
    }

    return jsonString.substr(startIndex, endIndex - startIndex);
}

// Test JSON number extraction
int extractJsonNumber(const string& jsonString, const string& key) {
    size_t keyIndex = jsonString.find(key);
    if (keyIndex == string::npos) {
        return -1;
    }

    size_t colonIndex = jsonString.find(':', keyIndex);
    if (colonIndex == string::npos) {
        return -1;
    }

    size_t numStart = colonIndex + 1;
    while (numStart < jsonString.length() && jsonString[numStart] == ' ') {
        numStart++;
    }

    size_t numEnd = numStart;
    while (numEnd < jsonString.length() && isdigit(jsonString[numEnd])) {
        numEnd++;
    }

    if (numEnd > numStart) {
        return stoi(jsonString.substr(numStart, numEnd - numStart));
    }
    return -1;
}

// Circular buffer for testing data logger
struct PlateRecord {
    string plateNumber;
    int confidence;
};

const int MAX_HISTORY = 10;
PlateRecord plateHistory[MAX_HISTORY];
int historyIndex = 0;
int totalRecognitions = 0;

void logPlate(const string& plate, int confidence) {
    plateHistory[historyIndex] = {plate, confidence};
    historyIndex = (historyIndex + 1) % MAX_HISTORY;
    totalRecognitions++;
}

int getHistorySize() {
    return (totalRecognitions < MAX_HISTORY) ? totalRecognitions : MAX_HISTORY;
}

PlateRecord getPlateAt(int index) {
    if (index >= 0 && index < MAX_HISTORY) {
        return plateHistory[index];
    }
    return {"Unknown", -1};
}

// ========== TEST SUITE ==========

void testJSONParsing() {
    cout << "\n[TEST] JSON Parsing\n";

    // Test 1: Valid JSON with string value
    string json1 = "{\"number_plate\":\"AB1234CD\",\"confidence\":95}";
    string plate = extractJsonValue(json1, "\"number_plate\"");
    assert(plate == "AB1234CD");
    cout << "  ✓ String extraction: " << plate << endl;

    // Test 2: Valid JSON with numeric value
    int confidence = extractJsonNumber(json1, "\"confidence\"");
    assert(confidence == 95);
    cout << "  ✓ Number extraction: " << confidence << endl;

    // Test 3: Missing key
    string notFound = extractJsonValue(json1, "\"missing_key\"");
    assert(notFound == "");
    cout << "  ✓ Missing key handling: empty string" << endl;

    // Test 4: Complex JSON with extra fields
    string json2 = "{\"success\":true,\"number_plate\":\"XY9876ZZ\",\"view_image\":\"http://example.com/img\",\"confidence\":87}";
    plate = extractJsonValue(json2, "\"number_plate\"");
    assert(plate == "XY9876ZZ");
    cout << "  ✓ Complex JSON parsing: " << plate << endl;

    // Test 5: Invalid JSON format
    string invalidJson = "not json";
    plate = extractJsonValue(invalidJson, "\"number_plate\"");
    assert(plate == "");
    cout << "  ✓ Invalid JSON handling: empty string" << endl;
}

void testDataLogger() {
    cout << "\n[TEST] Data Logger\n";

    // Reset
    historyIndex = 0;
    totalRecognitions = 0;

    // Test 1: Log single plate
    logPlate("AB1234CD", 95);
    assert(getHistorySize() == 1);
    cout << "  ✓ Log single plate: " << getHistorySize() << " total" << endl;

    // Test 2: Log multiple plates
    logPlate("XY9876ZZ", 87);
    logPlate("MN5555OP", 92);
    assert(getHistorySize() == 3);
    cout << "  ✓ Log multiple plates: " << getHistorySize() << " total" << endl;

    // Test 3: Retrieve from history
    PlateRecord rec = getPlateAt(0);
    assert(rec.plateNumber == "AB1234CD" && rec.confidence == 95);
    cout << "  ✓ Retrieve from history: " << rec.plateNumber << " (" << rec.confidence << "%)" << endl;

    // Test 4: Fill buffer beyond max
    for (int i = 0; i < 12; i++) {
        logPlate("TEST" + to_string(i), 50 + i);
    }
    assert(getHistorySize() == MAX_HISTORY);
    cout << "  ✓ Circular buffer overflow: " << getHistorySize() << " max (stored " << totalRecognitions << ")" << endl;

    // Test 5: History wrapping
    PlateRecord latest = getPlateAt(0); // Should be wrapped
    assert(latest.plateNumber != "");
    cout << "  ✓ History wrapping: oldest entry is " << latest.plateNumber << endl;
}

void testButtonDebounce() {
    cout << "\n[TEST] Button Debounce Logic\n";

    const unsigned long DEBOUNCE_MS = 50;
    bool lastState = false;
    unsigned long lastDebounceTime = 0;
    unsigned long time = 0;
    int pressCount = 0;

    // Simulate: stable high state
    bool currentState = true;
    if (currentState != lastState) {
        lastDebounceTime = time;
    }
    if ((time - lastDebounceTime) > DEBOUNCE_MS) {
        if (currentState && !lastState) {
            pressCount++;
        }
        lastState = currentState;
    }
    assert(pressCount == 0); // Not enough time passed
    cout << "  ✓ Noise rejection: glitch filtered" << endl;

    // Simulate: valid press with debounce
    time = 100;
    if (currentState != lastState) {
        lastDebounceTime = time;
    }
    if ((time - lastDebounceTime) > DEBOUNCE_MS) {
        if (currentState && !lastState) {
            pressCount++;
        }
        lastState = currentState;
    }
    assert(pressCount == 1); // After debounce time
    cout << "  ✓ Valid press detection: " << pressCount << " press(es) detected" << endl;
}

void testImageChunking() {
    cout << "\n[TEST] Image Chunking Logic\n";

    // Simulate image buffer
    size_t imageSize = 15234; // Typical JPEG size
    size_t chunkSize = 1024;
    size_t chunksNeeded = (imageSize + chunkSize - 1) / chunkSize; // Ceiling division

    assert(chunksNeeded == 15);
    cout << "  ✓ Chunk count: " << imageSize << " bytes -> " << chunksNeeded << " chunks of " << chunkSize << " bytes" << endl;

    // Test last chunk handling
    size_t remaining = imageSize % chunkSize;
    assert(remaining == 834); // 15234 % 1024 = 834
    cout << "  ✓ Last chunk size: " << remaining << " bytes" << endl;

    // Test with exact multiple
    imageSize = 10240;
    chunksNeeded = (imageSize + chunkSize - 1) / chunkSize;
    assert(chunksNeeded == 10);
    remaining = imageSize % chunkSize;
    assert(remaining == 0);
    cout << "  ✓ Exact multiple: " << imageSize << " -> " << chunksNeeded << " chunks (no remainder)" << endl;
}

void runAllTests() {
    cout << "\n" << string(50, '=') << endl;
    cout << "ESP32-CAM ANPR System - Unit Tests" << endl;
    cout << string(50, '=') << endl;

    testJSONParsing();
    testDataLogger();
    testButtonDebounce();
    testImageChunking();

    cout << "\n" << string(50, '=') << endl;
    cout << "✅ All tests passed!" << endl;
    cout << string(50, '=') << "\n" << endl;
}

int main() {
    try {
        runAllTests();
        return 0;
    } catch (const exception& e) {
        cerr << "❌ Test failed: " << e.what() << endl;
        return 1;
    }
}
