from flask import Flask, request, jsonify
from flask_cors import CORS
from joblib import load
import threading
import time

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes
model = load('model.joblib')  # Load trained model

# In-memory storage for the latest data
latest_data = None
lock = threading.Lock()

@app.route('/api/analyse', methods=['POST'])
def analyse():
    global latest_data
    data = request.json
    
    # Prepare features
    features = [[
        data['current'],
        data['voltage'],
        data['power']
    ]]
    
    # Predict
    prediction = model.predict(features)[0]
    
    result = {
        "pole_id": data['pole_id'],
        "area_name": data['area_name'],
        "location": data['location'],
        "current": data['current'],
        "voltage": data['voltage'],
        "power": data['power'],
        "detection": int(prediction)
    }
    
    # Update latest_data thread-safely
    with lock:
        latest_data = result
    
    return jsonify(result)

@app.route('/api/latest', methods=['GET'])
def get_latest():
    with lock:
        if latest_data:
            return jsonify(latest_data)
        else:
            return jsonify({"error": "No data available"}), 404

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
