// #include <Arduino.h>
// #include "esp_system.h"
// #include "nvs.h"
// #include <Wire.h>
// #include <SPI.h>
// #include "nvs_flash.h"

// // Function to initialize NVS
// esp_err_t initNVS() {
//   esp_err_t err = nvs_flash_init();
//   if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
//       err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
//     // Erase NVS partition and retry initialization
//     ESP_ERROR_CHECK(nvs_flash_erase());
//     err = nvs_flash_init();
//   }
//   return err;
// }

// void setup() {
//   Serial.begin(9600);
//   delay(1000);

//   Serial.println("\nPrint is working\n");

//   // Initialize NVS
//   esp_err_t err = initNVS();
//   ESP_ERROR_CHECK(err);

//   Serial.println("\nOpening Non-Volatile Storage (NVS) handle...");
//   nvs_handle_t my_handle;
//   err = nvs_open("storage", NVS_READWRITE, &my_handle);
//   if (err != ESP_OK) {
//     Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//   } else {
//     Serial.println("Done");

//     // Wi-Fi credentials
//     const char* ssid = "Karthik iPhone";
//     const char* pass = "87654321";

//     // Write SSID to NVS
//     Serial.print("Updating SSID in NVS... ");
//     err = nvs_set_str(my_handle, "ssid", ssid);
//     Serial.println((err != ESP_OK) ? "Failed!" : "Done");

//     // Write password to NVS
//     Serial.print("Updating password in NVS... ");
//     err = nvs_set_str(my_handle, "pass", pass);
//     Serial.println((err != ESP_OK) ? "Failed!" : "Done");

//     // Commit the written values
//     Serial.print("Committing updates in NVS... ");
//     err = nvs_commit(my_handle);
//     Serial.println((err != ESP_OK) ? "Failed!" : "Done");

//     // Close NVS handle
//     nvs_close(my_handle);
//   }
// }

// void loop() {
//   // Empty loop
// }
