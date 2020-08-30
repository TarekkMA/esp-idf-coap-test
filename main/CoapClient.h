/*
 * CoapClient.h
 *
 *  Created on: Aug 29, 2020
 *      Author: tarekkma
 */

#ifndef MAIN_COAPCLIENT_H_
#define MAIN_COAPCLIENT_H_

#include "libcoap.h"
#include "coap.h"

class CoapClient {
public:
  CoapClient(const char *url, bool isIP);
  virtual ~CoapClient();
  void run();
  void get(const char *path);
private:
  coap_context_t *context;
  coap_session_t *session;
};

#endif /* MAIN_COAPCLIENT_H_ */
