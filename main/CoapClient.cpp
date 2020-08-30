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


void message_handler(struct coap_context_t *context,
    coap_session_t *session,
    coap_pdu_t *sent,
    coap_pdu_t *received,
    const coap_tid_t id){

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
  coap_register_response_handler(context,message_handler);
}

void CoapClient::run(){
  coap_run_once(context, 0);
}

CoapClient::~CoapClient() {
  coap_free_context(context);
}


void CoapClient::get(const char *path){

}



