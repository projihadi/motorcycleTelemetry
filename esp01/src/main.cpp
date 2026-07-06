extern "C" {
#include "c_types.h"

// Explicit structural blueprint to bypass internal LwIP include traps
struct ip_addr {
  uint32_t addr;
};

#include "esp_wifi.h"

// Crucial inclusion order: user_interface sets up legacy ESP-NOW requirements
#include "espressif/espnow.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// REQUIRED FOR LINKER: Tells SDK where to store RF calibration data
uint32 user_rf_cal_sector_set(void) {
  return 187; // Standard default sector for 512KB/1MB ESP-01 modules
}
}

static const char* TAG = "ESP8266";

// ESP32 Target MAC Address
static const uint8_t mac[6] = {0x16, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};

// Explicitly packed to guarantee identical sizing on the ESP32 receiver
typedef struct __attribute__((packed)) {
  float leanAngle;
  float breakingForce;
  float corneringForce;
  uint32_t timestamp;
} MotorcycleTelemetry;

static MotorcycleTelemetry telemetryData;

// ESP8266 legacy callback signature
static void sendCallback(uint8_t* macAddress, uint8_t status) {
  // if (status == 0) {
  //   // os_printf("Delivery Success\n");
  // } else {
  //   // os_printf("Delivery Fail\n");
  // }
}

// FreeRTOS Task to handle processing
void telemetry_task(void* pvParameters) {
  // CRITICAL: Give the underlying RTOS SDK system a long moment to fully boot
  vTaskDelay(2000 / portTICK_RATE_MS);

  // Set Station Mode natively via the v1.5 SDK layer
  wifi_set_opmode(STATION_MODE);
  vTaskDelay(500 / portTICK_RATE_MS);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    os_printf("Error initializing ESP-NOW\n");
    vTaskDelete(NULL);
    return;
  }

  // Assign controller roles and reference your callback definition
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb((esp_now_send_cb_t)sendCallback);

  // Register your ESP32 peer target details (Using channel 1)
  int peer_res =
      esp_now_add_peer((uint8_t*)mac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  if (peer_res != 0) {
    // os_printf("Failed to add peer: %d\n", peer_res);
  }

  // Populate structural dataset configurations
  telemetryData.leanAngle = 42.5;
  telemetryData.breakingForce = 85.3;
  telemetryData.corneringForce = 1.2;
  telemetryData.timestamp = 0;

  while (1) {
    // Send data stream
    int result = esp_now_send((uint8_t*)mac, (uint8_t*)&telemetryData,
        sizeof(telemetryData));
    if (result != 0) {
      // os_printf("Error sending data: %d\n", result);
    }

    telemetryData.timestamp += 300;
    vTaskDelay(300 / portTICK_RATE_MS);
  }
}

// Fixed Entry point for legacy RTOS SDK
extern "C" void user_init(void) {
  xTaskCreate(telemetry_task, (const signed char*)"telemetry_task", 512, NULL,
      2, NULL);
}
