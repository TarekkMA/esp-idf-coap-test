/*
 * coap_utils.h
 *
 *  Created on: Aug 29, 2020
 *      Author: tarekkma
 */

#ifndef MAIN_COAP_UTILS_H_
#define MAIN_COAP_UTILS_H_

#include "coap.h"

int coap_utils_resolve_address(const char*urlStr,coap_address_t *res_add);


#endif /* MAIN_COAP_UTILS_H_ */
