import pandas as pd
from sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import classification_report
from joblib import dump

# Load data
df = pd.read_csv('data.csv')

# Split features/target
X = df[['current', 'voltage', 'power']]
y = df['detection']

# Split data
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, stratify=y)

# Train model
model = RandomForestClassifier(
    n_estimators=100,
    class_weight='balanced',  # Handle imbalanced data
    random_state=42
)
model.fit(X_train, y_train)

# Evaluate
y_pred = model.predict(X_test)
print(classification_report(y_test, y_pred))

# Save model
dump(model, 'model.joblib')