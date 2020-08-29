# Embed CA, certificate & key directly into binary
idf_component_register(
SRCS 	"coap_client_example_main.c" 
		"coap_utils.c"

INCLUDE_DIRS "."
)
