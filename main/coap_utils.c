/*
 * coap_utils.c
 *
 *  Created on: Aug 29, 2020
 *      Author: tarekkma
 */
#include <sys/socket.h>
#include <netdb.h>
#include "esp_log.h"
#include "libcoap.h"
#include "coap_dtls.h"
#include "coap.h"
#include "net.h"

#define DNS_MAX_RETRY 3
#define DNS_WAIT_TIME 1000

int coap_utils_resolve_address(const char *urlStr, coap_address_t *res_add) {
  const static char *TAG = "coap_utils_resolve_address";
  if (res_add == NULL) {
    ESP_LOGE(TAG, "res_add is NULL");
    return 1;
  }

  ESP_LOGI(TAG, "Starting DNS lookup for %s", urlStr);

  static coap_uri_t uri;
  if (coap_split_uri((const uint8_t*) urlStr, strlen(urlStr), &uri) == -1) {
    ESP_LOGE(TAG, "CoAP urlStr was malformed");
    return 1;
  }

  struct hostent *hp;
  char *cHostname = (char*) calloc(1, uri.host.length + 1);
  int retryCount = 0;
  while (1) {
    retryCount++;
    memcpy(cHostname, uri.host.s, uri.host.length);
    hp = gethostbyname(cHostname);
    if (hp == NULL) {
      if (retryCount >= DNS_MAX_RETRY) {
        ESP_LOGE(TAG, "DNS lookup failed after %d tries", DNS_MAX_RETRY);
        return 1;
      }
      ESP_LOGE(TAG, "[%d/%d] DNS lookup failed", retryCount, DNS_MAX_RETRY);
      vTaskDelay(DNS_WAIT_TIME / portTICK_PERIOD_MS);
      continue;
    }
    free(cHostname);
    break;
  }

  if (hp->h_addrtype != AF_INET) {
    ESP_LOGE(TAG, "DNS was not ipv4");
    return 1;
  }

  coap_address_init(res_add);
  res_add->addr.sin.sin_family = AF_INET;
  res_add->addr.sin.sin_port = htons(uri.port);
  memcpy(&res_add->addr.sin.sin_addr, hp->h_addr, sizeof(res_add->addr.sin.sin_addr));

  char tmpbuf[INET6_ADDRSTRLEN];
  inet_ntop(AF_INET, &res_add->addr.sin.sin_addr, tmpbuf, sizeof(tmpbuf));
  ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", tmpbuf);

  return 0;
}

int coap_utils_resolve_ip(const char *ipStr, coap_address_t *res_add) {
  const static char *TAG = "coap_utils_resolve_ip";

  if (res_add == NULL) {
    ESP_LOGE(TAG, "res_add is NULL");
    return 1;
  }

  coap_address_init(res_add);
  res_add->addr.sin.sin_family = AF_INET;
  res_add->addr.sin.sin_port = htons(5683);
  res_add->addr.sin.sin_addr.s_addr = inet_addr(ipStr);

  return 0;
}
