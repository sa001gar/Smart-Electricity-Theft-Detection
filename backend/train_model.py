# backend/train_model.py
import pandas as pd
import numpy as np
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
import pickle

# Load data from the CSV file
data = pd.read_csv("training_data.csv")

# Extract features and labels
# Label: 0 (Normal), 1 (Theft) - Add labels manually for the training data
data["label"] = [0 if power <= 6 else 1 for power in data["power"]]  # Example threshold
X = data[["current", "voltage", "power"]]
y = data["label"]

# Split into training and testing sets
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train the model
model = RandomForestClassifier()
model.fit(X_train, y_train)

# Test the model
y_pred = model.predict(X_test)
print("Accuracy:", accuracy_score(y_test, y_pred))

# Save the model
with open("theft_detection_model.pkl", "wb") as file:
    pickle.dump(model, file)
