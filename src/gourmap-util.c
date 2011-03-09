#include <libsoup/soup.h>
#include <rest/rest-proxy.h>
#include <json-glib/json-glib.h>

#include "gourmap-util.h"

JsonNode *
json_node_from_call (RestProxyCall *call)
{
	JsonParser *parser;
	JsonNode *root = NULL;
	JsonObject *obj = NULL;
	GError *error;
	gboolean ret = FALSE;
	const char *status;

	parser = json_parser_new ();

	if (call == NULL)
		goto out;

	if (!SOUP_STATUS_IS_SUCCESSFUL (rest_proxy_call_get_status_code (call))) {
		g_message ("Error from Google Map: %s (%d)",
			   rest_proxy_call_get_status_message (call),
			   rest_proxy_call_get_status_code (call));
		goto out;
	}

	ret = json_parser_load_from_data (parser,
					  rest_proxy_call_get_payload (call),
					  rest_proxy_call_get_payload_length (call),
					  &error);
	root = json_parser_get_root (parser);

	if (root == NULL) {
		g_message ("Error from Google Map: %s",
		rest_proxy_call_get_payload (call));
		goto out;
	}

	/* check status */
	obj = json_node_get_object (root);
	status = json_object_get_string_member (obj, "status");
	if (g_strcmp0 (status, "OK") != 0) {
		g_message ("Error from Google Map: %s", status);
		root = NULL;
		goto out;
	}

	root = json_node_copy (root);
out:
	g_object_unref (parser);

	return root;
}
