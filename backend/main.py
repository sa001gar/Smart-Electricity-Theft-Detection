# backend/main.py
from fastapi import FastAPI, HTTPException
from pydantic import BaseModel
import csv
import os
from datetime import datetime

app = FastAPI()

# Define the data model for receiving data
class SensorData(BaseModel):
    pole_id: str
    location: str  # Manual location set during installation
    current: float
    voltage: float
    power: float

# File to store training data
DATA_FILE = "training_data.csv"

# Ensure the CSV file exists
if not os.path.exists(DATA_FILE):
    with open(DATA_FILE, mode="w", newline="") as file:
        writer = csv.writer(file)
        writer.writerow(["pole_id", "location", "current", "voltage", "power", "timestamp"])

# Endpoint to receive and store data
@app.post("/collect-data/")
async def collect_data(data: SensorData):
    try:
        # Save data to the CSV file
        with open(DATA_FILE, mode="a", newline="") as file:
            writer = csv.writer(file)
            writer.writerow([
                data.pole_id,
                data.location,
                data.current,
                data.voltage,
                data.power,
                datetime.now().isoformat()  # Add a timestamp
            ])
        return {"status": "success", "message": "Data stored for training."}
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))


# backend/main.py (Extend the existing file)
import pickle
import numpy as np

# Load the trained AI model
with open("theft_detection_model.pkl", "rb") as file:
    model = pickle.load(file)

@app.post("/detect-theft/")
async def detect_theft(data: SensorData):
    try:
        # Prepare input data for the model
        input_data = np.array([[data.current, data.voltage, data.power]])
        
        # Predict using the AI model
        prediction = model.predict(input_data)
        
        # Return the response
        if prediction[0] == 1:
            return {
                "status": "alert",
                "message": "Electricity theft detected!",
                "pole_id": data.pole_id,
                "location": data.location
            }
        else:
            return {
                "status": "ok",
                "message": "No theft detected.",
                "pole_id": data.pole_id,
                "location": data.location
            }
    except Exception as e:
        raise HTTPException(status_code=500, detail=str(e))
