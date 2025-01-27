from flask import Flask, request, jsonify
from flask_cors import CORS
import pandas as pd
import csv
import os
from sklearn.ensemble import RandomForestClassifier
from joblib import dump, load

app = Flask(__name__)
CORS(app)

# Initialize CSV file with headers
if not os.path.exists('data.csv'):
    with open('data.csv', 'w') as f:
        writer = csv.writer(f)
        writer.writerow(['current', 'voltage', 'power', 'detection'])

# API to store training data
@app.route('/api/store-data', methods=['POST'])
def store_data():
    data = request.json
    with open('data.csv', 'a') as f:
        writer = csv.writer(f)
        writer.writerow([
            data['current'],
            data['voltage'],
            data['power'],
            data['detection']
        ])
    return jsonify({"status": "Data stored"}), 200


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)