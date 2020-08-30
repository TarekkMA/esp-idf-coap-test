# Embed CA, certificate & key directly into binary
idf_component_register(
SRCS 	"coap_client_example_main.cpp" 
		"coap_utils.cpp"
		"CoapClient.cpp"

INCLUDE_DIRS "."
)
