#include "espNow.hpp" // Include the new header
#include "esp_err.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_now.h"
#include "esp_wifi.h"
#include "nvs_flash.h"
#include <string.h>
#include <types.hpp>

static const char* TAG = "Wifi";
static const uint8_t mac[6] = {0x16, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE};

static void onDataRecieved(const esp_now_recv_info_t* receivedInfo, const uint8_t* data, int len) {
  if (len == sizeof(MotorcycleTelemetry)) {
    MotorcycleTelemetry telemetryData;
    memcpy(&telemetryData, data, sizeof(MotorcycleTelemetry));
    if (telemetryQueue != NULL) {
      xQueueOverwrite(telemetryQueue, &telemetryData);
    }
  }
}

// static void onDataSent(const esp_now_send_info_t* txInfo,
//     esp_now_send_status_t status) {
//   ESP_LOGI(TAG, "Packet Send Status: %s",
//       status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
// }

void initWifi(void) {
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t wifiConfig = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&wifiConfig));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_mac(WIFI_IF_STA, mac));
  ESP_ERROR_CHECK(esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE));
  ESP_ERROR_CHECK(esp_now_init());

  ESP_ERROR_CHECK(esp_now_register_recv_cb(onDataRecieved));
  // ESP_ERROR_CHECK(esp_now_register_send_cb(onDataSent));

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, mac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  ESP_ERROR_CHECK(esp_now_add_peer(&peerInfo));
}
