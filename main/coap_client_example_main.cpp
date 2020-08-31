#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"

#include "nvs_flash.h"

#include "protocol_examples_common.h"

#include "libcoap.h"
#include "coap.h"

#include "coap_utils.h"

#include "CoapClient.h"

const static char *TAG = "CoAP_client";

static void startClient(void *params) {
  coap_set_log_level(LOG_DEBUG);
  CoapClient client("164.90.236.67", true);
  client.get("sayhello/esp32");
  client.run();
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  client.obs("sw/5");
  while (1) {
    ESP_LOGI(TAG, "client loop");
    client.run();
  }

  vTaskDelete(NULL);
}

extern "C" {
void app_main(void);
}

void app_main(void) {
  ESP_ERROR_CHECK(example_connect());
  xTaskCreate(startClient, "CoAP Client", 8 * 1024, NULL, 2, NULL);
}
