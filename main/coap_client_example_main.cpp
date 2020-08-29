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

static int resp_wait = 1;
static coap_optlist_t *optlist = NULL;
static int wait_ms;
const static char *TAG = "CoAP_client";

static void message_handler(coap_context_t *ctx, coap_session_t *session,
    coap_pdu_t *sent, coap_pdu_t *received, const coap_tid_t id) {



  unsigned char *data = NULL;
  size_t data_len;
  coap_pdu_t *pdu = NULL;
  coap_opt_t *block_opt;
  coap_opt_iterator_t opt_iter;
  unsigned char buf[4];
  coap_optlist_t *option;
  coap_tid_t tid;

  ESP_LOGE(TAG,":(");
 // ESP_LOGE(TAG,);
  printf("response class %d\n",COAP_RESPONSE_CLASS(received->code));


 // if (COAP_RESPONSE_CLASS(received->code) == 2) {
    /* Need to see if blocked response */
    block_opt = coap_check_option(received, COAP_OPTION_BLOCK2, &opt_iter);
    if (block_opt) {
      uint16_t blktype = opt_iter.type;

      if (coap_opt_block_num(block_opt) == 0) {
        printf("Received:\n");
      }
      if (coap_get_data(received, &data_len, &data)) {
        printf("%.*s", (int) data_len, data);
      }
      if (COAP_OPT_BLOCK_MORE(block_opt)) {
        /* more bit is set */

        /* create pdu with request for next block */
        pdu = coap_new_pdu(session);
        if (!pdu) {
          ESP_LOGE(TAG, "coap_new_pdu() failed");
          goto clean_up;
        }
        pdu->type = COAP_MESSAGE_CON;
        pdu->tid = coap_new_message_id(session);
        pdu->code = COAP_REQUEST_GET;

        /* add URI components from optlist */
        for (option = optlist; option; option = option->next) {
          switch (option->number) {
          case COAP_OPTION_URI_HOST:
          case COAP_OPTION_URI_PORT:
          case COAP_OPTION_URI_PATH:
          case COAP_OPTION_URI_QUERY:
            coap_add_option(pdu, option->number, option->length, option->data);
            break;
          default:
            ; /* skip other options */
          }
        }

        /* finally add updated block option from response, clear M bit */
        /* blocknr = (blocknr & 0xfffffff7) + 0x10; */
        coap_add_option(pdu, blktype,
            coap_encode_var_safe(buf, sizeof(buf),
                ((coap_opt_block_num(block_opt) + 1) << 4)
                    | COAP_OPT_BLOCK_SZX(block_opt)), buf);

        tid = coap_send(session, pdu);

        if (tid != COAP_INVALID_TID) {
          resp_wait = 1;
          wait_ms = 5 * 1000;
          return;
        }
      }
      printf("\n");
    } else {
      if (coap_get_data(received, &data_len, &data)) {
        printf("Received: %.*s\n", (int) data_len, data);
      }
    }
  //}
  clean_up: resp_wait = 0;
}

static void startClient(void*params) {
  //coap_address_t *address = resolveAddress("coap://164.90.236.67");
  char *path = "sayhello/Tarek";
  coap_address_t *dst_addr = (coap_address_t*)malloc(sizeof(coap_address_t));
//  coap_address_init(dst_addr);
//  dst_addr->addr.sin.sin_family      = AF_INET;
//  dst_addr->addr.sin.sin_port        = htons(5683);
//  dst_addr->addr.sin.sin_addr.s_addr = inet_addr("164.90.236.67");
  coap_utils_resolve_ip("164.90.236.67", dst_addr);


  coap_context_t *ctx = coap_new_context(NULL);
  coap_session_t *session = coap_new_client_session(ctx, NULL, dst_addr,
      COAP_PROTO_UDP);
  coap_register_response_handler(ctx, message_handler);

  coap_pdu_t *request = coap_new_pdu(session);
  request->type = COAP_MESSAGE_CON;
  request->tid = coap_new_message_id(session);
  request->code = COAP_REQUEST_GET;
  //coap_add_option(request, COAP_OPTION_URI_PATH, strlen(path),(uint8_t *) path);
  ESP_LOGE(TAG,"Will connect now");
  coap_send(session, request);
  coap_run_once(ctx, 0);

  vTaskDelete(NULL);
}

extern "C" {
  void app_main(void);
}

void app_main(void) {
  ESP_ERROR_CHECK( nvs_flash_init() );
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

  ESP_ERROR_CHECK(example_connect());
  xTaskCreate(startClient, "CoAP Client", 8 * 1024, NULL, 2, NULL);
}
