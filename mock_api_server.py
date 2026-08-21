/**
 * Mock CircuitDigest API Server
 * 
 * Run this on your development machine to test ANPR system without real API calls
 * Requires: Python 3 with flask
 * 
 * Installation:
 *   pip install flask
 * 
 * Usage:
 *   python mock_api_server.py
 * 
 * Then point your ESP32-CAM to: http://your-pc-ip:5000
 * (Update serverName in config.h)
 * 
 * Note: For HTTPS testing, use: https://your-pc-ip:443 with self-signed cert
 */

from flask import Flask, request, jsonify
import time
import json
import random
from datetime import datetime

app = Flask(__name__)

# Simulate plate database
SIMULATED_PLATES = [
    {"plate": "AB1234CD", "confidence": 95},
    {"plate": "XY9876ZZ", "confidence": 87},
    {"plate": "MN5555OP", "confidence": 92},
    {"plate": "PQ1111RS", "confidence": 88},
    {"plate": "GH2222IJ", "confidence": 90},
]

request_log = []

@app.route('/api/v1/readnumberplate', methods=['POST'])
def read_number_plate():
    """Mock CircuitDigest API endpoint"""
    
    # Verify API key
    auth_header = request.headers.get('Authorization')
    if not auth_header:
        return jsonify({"error": "Missing authorization header"}), 401
    
    # Simulate processing delay
    time.sleep(0.5)
    
    # Simulate occasional failures (10% chance)
    if random.random() < 0.1:
        response = {
            "success": False,
            "error": "Image processing failed",
            "timestamp": datetime.now().isoformat()
        }
        status_code = 500
    else:
        # Return random plate from simulation
        plate_data = random.choice(SIMULATED_PLATES)
        response = {
            "success": True,
            "number_plate": plate_data["plate"],
            "confidence": plate_data["confidence"],
            "view_image": "http://localhost:5000/image.jpg",
            "processing_time_ms": random.randint(500, 2000),
            "timestamp": datetime.now().isoformat()
        }
        status_code = 200
    
    # Log request
    request_log.append({
        "timestamp": datetime.now().isoformat(),
        "api_key": auth_header,
        "response": response.get("number_plate", "error")
    })
    
    return jsonify(response), status_code

@app.route('/status', methods=['GET'])
def status():
    """Health check and stats"""
    return jsonify({
        "status": "running",
        "requests_served": len(request_log),
        "simulated_plates": len(SIMULATED_PLATES)
    }), 200

@app.route('/requests', methods=['GET'])
def get_requests():
    """Get request log"""
    return jsonify({
        "total": len(request_log),
        "requests": request_log[-10:]  # Last 10 requests
    }), 200

@app.route('/', methods=['GET'])
def home():
    """Welcome page"""
    return f"""
    <h1>Mock CircuitDigest API Server</h1>
    <p>Status: Running</p>
    <p>Requests served: {len(request_log)}</p>
    <p>
        <a href="/status">Status</a> | 
        <a href="/requests">Requests Log</a>
    </p>
    <h2>Configuration</h2>
    <pre>
# In your ESP32 config.h, use:
const char* API_SERVER_NAME = "your-pc-ip";
const int API_SERVER_PORT = 5000;  // or 443 for HTTPS
    </pre>
    """, 200

if __name__ == '__main__':
    print("\n" + "="*50)
    print("Mock CircuitDigest API Server")
    print("="*50)
    print("\nServer running on http://localhost:5000")
    print("\nEndpoints:")
    print("  POST   /api/v1/readnumberplate - Main API")
    print("  GET    /status                 - Health check")
    print("  GET    /requests               - Request log")
    print("  GET    /                       - Welcome page")
    print("\nTo use with ESP32-CAM:")
    print("  1. Update config.h:")
    print("     const char* API_SERVER_NAME = \"<your-pc-ip>\";")
    print("     const int API_SERVER_PORT = 5000;")
    print("  2. Set SKIP_SSL_VERIFICATION = true (for HTTP)")
    print("\nPress Ctrl+C to stop")
    print("="*50 + "\n")
    
    app.run(host='0.0.0.0', port=5000, debug=True)
