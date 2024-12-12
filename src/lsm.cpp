// #include "SparkFunLSM6DSO.h"
// #include "Wire.h"
// #include "SPI.h"

// LSM6DSO myIMU; // Default constructor for I2C, addr 0x6B

// // Threshold values for bad posture (customize these values based on calibration)
// const float ACCEL_X_BAD_THRESHOLD = 0.5; // X-axis threshold for bad posture (too low indicates bad posture)
// const float ACCEL_Y_THRESHOLD = 0.5; // Y-axis deviation threshold for leaning sideways
// const unsigned long BAD_POSTURE_DURATION = 60000; // Time (in ms) to trigger alert

// unsigned long badPostureStartTime = 0; // Time when bad posture started
// bool inBadPosture = false; // Flag to indicate bad posture state

// void setup() {
//   Serial.begin(9600);
//   delay(500);

//   Wire.begin();
//   delay(10);
//   if (myIMU.begin())
//     Serial.println("IMU Ready.");
//   else {
//     Serial.println("Could not connect to IMU. Freezing.");
//     while (true);
//   }

//   if (myIMU.initialize(BASIC_SETTINGS))
//     Serial.println("Settings Loaded.");
// }

// void loop() {
//   // Read accelerometer data
//   float accelX = myIMU.readFloatAccelX();
//   float accelY = myIMU.readFloatAccelY();
//   float accelZ = myIMU.readFloatAccelZ();

//   // Check if posture is bad
//   bool badPosture = (abs(accelX) < ACCEL_X_BAD_THRESHOLD) || (abs(accelY) > ACCEL_Y_THRESHOLD);

//   if (badPosture) {
//     if (!inBadPosture) {
//       // Start tracking bad posture duration
//       badPostureStartTime = millis();
//       inBadPosture = true;
//     } else if (millis() - badPostureStartTime > BAD_POSTURE_DURATION) {
//       // Trigger alert if bad posture persists
//       Serial.println("Bad posture detected for too long! Correct your posture.");
//       // You can add code here to trigger a buzzer, vibration, or notification
//     }
//   } else {
//     // Reset posture state if posture is corrected
//     inBadPosture = false;
//     badPostureStartTime = 0;
//   }

//   // Print accelerometer values for debugging
//   Serial.print("Accelerometer: X=");
//   Serial.print(accelX, 3);
//   Serial.print(" Y=");
//   Serial.print(accelY, 3);
//   Serial.print(" Z=");
//   Serial.println(accelZ, 3);

//   delay(500); // Adjust sampling rate
// }
