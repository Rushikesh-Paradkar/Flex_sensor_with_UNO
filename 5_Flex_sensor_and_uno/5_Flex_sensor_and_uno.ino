// Pins for 5 flex sensors
const int flexPins[5] = {A0, A1, A2, A3, A4};
const float fixedResistor = 10000.0; // 10kΩ resistor

// Individual calibration (replace with your measured values!)
const float straightResistances[5] = {25000.0, 30000.0, 27000.0, 32000.0, 28000.0}; // Unbent (Ω)
const float bentResistances[5] = {80000.0, 90000.0, 85000.0, 95000.0, 82000.0};     // 90° bent (Ω)
const float straightAngle = 0.0;  // 0° = straight
const float bentAngle = 90.0;     // 90° = fully bent

// Noise filtering (moving average)
const int numReadings = 10;
float readings[5][numReadings]; // Stores readings for each sensor
int index[5] = {0};             // Index for each sensor
float totals[5] = {0};          // Running totals for each sensor

void setup() {
  Serial.begin(9600);
  // Initialize all readings to 0
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < numReadings; j++) {
      readings[i][j] = 0;
    }
  }
}

void loop() {
  for (int i = 0; i < 5; i++) {
    // Read raw analog value
    int rawValue = analogRead(flexPins[i]);
    
    // Apply moving average filter
    totals[i] -= readings[i][index[i]];
    readings[i][index[i]] = rawValue;
    totals[i] += readings[i][index[i]];
    index[i] = (index[i] + 1) % numReadings;
    float avgValue = totals[i] / numReadings;
    
    // Convert to voltage (0–5V)
    float voltage = avgValue * (5.0 / 1023.0);
    
    // Calculate flex sensor resistance
    float flexResistance = fixedResistor * (5.0 / voltage - 1.0);
    
    // Map resistance to angle (0°–90°)
    float angle = (flexResistance - straightResistances[i]) * (bentAngle - straightAngle) /
                  (bentResistances[i] - straightResistances[i]) + straightAngle;
    
    // Clamp angle to 0°–90°
    angle = constrain(angle, straightAngle, bentAngle);
    
    // Print results
    Serial.print("Flex ");
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(angle, 1);
    Serial.print("° | ");
  }
  Serial.println(); // New line for next reading
  delay(1000);      // Faster sampling (adjust as needed)
}