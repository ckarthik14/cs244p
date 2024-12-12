#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "SparkFunLSM6DSO.h"
#include "nvs.h"
#include "nvs_flash.h"

// Wi-Fi credentials (global for consistency)
char ssid[50];
char pass[50];

// Flask server URL
const char* serverName = "http://ec2-3-82-44-54.compute-1.amazonaws.com:5000";

// Create an LSM6DSO IMU object
LSM6DSO myIMU;

// Buzzer pin
#define BUZZER_PIN 33

// Bad posture detection thresholds
const float ACCEL_X_BAD_THRESHOLD = 0.8;
const float ACCEL_Y_THRESHOLD = 0.3;
const unsigned long BAD_POSTURE_DURATION = 1000; // 1 second (adjust as needed)

// Variables for posture tracking
unsigned long badPostureStartTime = 0;
bool inBadPosture = false;

// Function to initialize NVS
esp_err_t initNVS() {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  return err;
}

// Function to access NVS and retrieve SSID and Password
void nvs_access() {
  esp_err_t err = initNVS();
  ESP_ERROR_CHECK(err);

  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READONLY, &my_handle);
  if (err != ESP_OK) {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  } else {
    size_t ssid_len = sizeof(ssid);
    size_t pass_len = sizeof(pass);

    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    if (err == ESP_OK) {
      err = nvs_get_str(my_handle, "pass", pass, &pass_len);
    }

    if (err == ESP_OK) {
      Serial.println("SSID and Password retrieved successfully.");
    } else if (err == ESP_ERR_NVS_NOT_FOUND) {
      Serial.println("SSID and Password not found in NVS!");
      ssid[0] = '\0';
      pass[0] = '\0';
    } else {
      Serial.printf("Error (%s) reading SSID/Password!\n", esp_err_to_name(err));
    }
    nvs_close(my_handle);
  }
}

// Helper function for buzzing
void buzz(int frequency, int duration) {
  tone(BUZZER_PIN, frequency, duration);
  delay(duration);  // Wait for the duration of the tone
  noTone(BUZZER_PIN);  // Stop the tone
}

// Placeholder function to read heartbeat from your sensor
// Replace this with your actual heartbeat reading code.
float readHeartbeat() {
  // In a real scenario, read from your hardware sensor here.
  // For example, if you have a pulse sensor, you'd use the library function.
  // Here, we just return a simulated value.
  return 72.0 + (random(0, 10) - 5); // A fake heartbeat around 72 BPM
}

void setup() {
  Serial.begin(9600);
  delay(5000);

  Wire.begin();
  delay(10);
  if (myIMU.begin())
    Serial.println("IMU Ready.");
  else {
    Serial.println("Could not connect to IMU. Freezing.");
    while (true);
  }

  if (myIMU.initialize(BASIC_SETTINGS))
    Serial.println("Settings Loaded.");

  // Configure buzzer pin
  pinMode(BUZZER_PIN, OUTPUT);

  // Retrieve Wi-Fi credentials from NVS
  nvs_access();

  if (strlen(ssid) == 0 || strlen(pass) == 0) {
    Serial.println("SSID or Password not set in NVS.");
    while (1) delay(1000);
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi: ");
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to Wi-Fi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to Wi-Fi.");
    while (1) delay(1000); // Halt if connection fails
  }
}

void loop() {
  // Read accelerometer data
  float accelX = myIMU.readFloatAccelX();
  float accelY = myIMU.readFloatAccelY();
  float accelZ = myIMU.readFloatAccelZ();

  // Read heartbeat data
  float heartbeat = readHeartbeat();

  // Check for bad posture
  bool badPosture = (abs(accelX) < ACCEL_X_BAD_THRESHOLD) || (abs(accelY) > ACCEL_Y_THRESHOLD);

  if (badPosture) {
    if (!inBadPosture) {
      badPostureStartTime = millis();
      inBadPosture = true;
    } else if (millis() - badPostureStartTime > BAD_POSTURE_DURATION) {
      // Trigger alert if bad posture persists
      Serial.println("Bad posture detected for too long! Correct your posture.");
      buzz(1000, 500);  // Buzz with 1000 Hz for 500 ms
    }
  } else {
    // Reset posture state if corrected
    inBadPosture = false;
    badPostureStartTime = 0;
  }

  // Print data for debugging
  Serial.print("Accel X: ");
  Serial.print(accelX);
  Serial.print(", Accel Y: ");
  Serial.print(accelY);
  Serial.print(", Accel Z: ");
  Serial.print(accelZ);
  Serial.print(", Heartbeat: ");
  Serial.println(heartbeat);

  // Post data to Flask server along with heartbeat
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String url = String(serverName) + "/send_data?accelX=" + String(accelX) 
                 + "&accelY=" + String(accelY) 
                 + "&accelZ=" + String(accelZ)
                 + "&heartbeat=" + String((int)heartbeat);
    Serial.println("Sending data to server:");
    Serial.println(url);

    http.begin(url);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending GET request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi disconnected. Reconnecting...");
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nReconnected to Wi-Fi.");
  }

  delay(1000); // Loop delay
}
