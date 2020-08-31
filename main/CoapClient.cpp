/*
 * CoapClient.cpp
 *
 *  Created on: Aug 29, 2020
 *      Author: tarekkma
 */

#include "CoapClient.h"
#include "coap_utils.h"
#include "libcoap.h"
#include "coap.h"
#include <netdb.h>

static void message_handler(struct coap_context_t *context, coap_session_t *session,
    coap_pdu_t *sent, coap_pdu_t *received, const coap_tid_t id) {
  unsigned char *data = NULL;
  size_t data_len;
  if (coap_get_data(received, &data_len, &data)) {
    for (int i = 0; i < data_len; i++) {
      printf("[%d] => %d\n", i, data[i]);
    }
  }
}

CoapClient::CoapClient(const char *url, bool isIP) {
  coap_address_t dst_addr;
  if (isIP) {
    coap_utils_resolve_ip(url, &dst_addr);
  } else {
    coap_utils_resolve_address(url, &dst_addr);
  }
  context = coap_new_context(NULL);
  session = coap_new_client_session(context, NULL, &dst_addr,
  COAP_PROTO_UDP);
  coap_register_response_handler(context, message_handler);
}

void CoapClient::run() {
  coap_run_once(context, 0);
}

CoapClient::~CoapClient() {
  coap_free_context(context);
}

void CoapClient::get(const char *path) {
  coap_pdu_t *request = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_GET,
      coap_new_message_id(session), coap_session_max_pdu_size(session));
  coap_add_option(request, COAP_OPTION_URI_PATH, strlen(path), (uint8_t*) path);
  coap_send(session, request);
}

void CoapClient::obs(const char *path) {
  coap_pdu_t *request = coap_pdu_init(COAP_MESSAGE_CON, COAP_REQUEST_GET,
      coap_new_message_id(session), coap_session_max_pdu_size(session));
  coap_add_option(request, COAP_OPTION_URI_PATH, strlen(path), (uint8_t*) path);
  coap_add_option(request, COAP_OPTION_OBSERVE, COAP_OBSERVE_ESTABLISH, NULL);
  coap_send(session, request);
}
//
//int build_send_pdu(coap_context_t *context, coap_session_t *session,
//    uint8_t msgtype, uint8_t request_code, const char *uri, const char *query,
//    unsigned char *data, size_t length, int observe) {
//
//  coap_pdu_t *pdu;
//  (void) context;
//  char buf[1024];
//  size_t buflen;
//  char *sbuf = buf;
//  int res;
//  coap_optlist_t *optlist_chain = NULL;
//
//  /* Create the pdu with the appropriate options */
//  pdu = coap_pdu_init(msgtype, request_code, coap_new_message_id(session),
//      coap_session_max_pdu_size(session));
//  if (!pdu)
//    return 0;
//
//  /*
//   * Create uniqueness token for this request for handling unsolicited /
//   * delayed responses
//   */
//  token++;
//  if (!coap_add_token(pdu, sizeof(token), (unsigned char*) &token)) {
//    coap_log(LOG_DEBUG, "cannot add token to request\n");
//    goto error;
//  }
//
//  if (uri) {
//    /* Add in the URI options */
//    buflen = sizeof(buf);
//    res = coap_split_path((const uint8_t*) uri, strlen(uri), sbuf, &buflen);
//    while (res--) {
//      if (!coap_insert_optlist(&optlist_chain,
//          coap_new_optlist(COAP_OPTION_URI_PATH, coap_opt_length(sbuf),
//              coap_opt_value(sbuf))))
//        goto error;
//      sbuf += coap_opt_size(sbuf);
//    }
//  }
//
//  if (query) {
//    /* Add in the QUERY options */
//    buflen = sizeof(buf);
//    res = coap_split_query((const uint8_t*) query, strlen(query), sbuf,
//        &buflen);
//    while (res--) {
//      if (!coap_insert_optlist(&optlist_chain,
//          coap_new_optlist(COAP_OPTION_URI_QUERY, coap_opt_length(sbuf),
//              coap_opt_value(sbuf))))
//        goto error;
//      sbuf += coap_opt_size(sbuf);
//    }
//  }
//
//  if (request_code == COAP_REQUEST_GET && observe) {
//    /* Indicate that we want to observe this resource */
//    if (!coap_insert_optlist(&optlist_chain,
//        coap_new_optlist(COAP_OPTION_OBSERVE,
//        COAP_OBSERVE_ESTABLISH, NULL)))
//      goto error;
//  }
//
//  /* ... Other code / options etc. ... */
//
//  /* Add in all the options (after internal sorting) to the pdu */
//  if (!coap_add_optlist_pdu(pdu, &optlist_chain))
//    goto error;
//
//  if (data && length) {
//    /* Add in the specified data */
//    if (!coap_add_data(pdu, length, data))
//      goto error;
//  }
//
//  if (coap_send(session, pdu) == COAP_INVALID_TID)
//    goto error;
//  return 1;
//
//  error:
//
//
//  return 0;
//
//}
