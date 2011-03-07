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

/*
 * For a given parent @node, get the child node called @name and return a copy
 * of the content, or NULL. If the content is the empty string, NULL is
 * returned.
 */
char *
get_child_node_value (JsonNode	 *node,
		      const char *name)
{
	JsonNode *subnode;
	JsonObject *object;
	GValue value = {0};
	const char *string;
	char *result = NULL;

	if (!node || !name)
		return NULL;

	if (json_node_get_node_type (node) == JSON_NODE_OBJECT) {
		object = json_node_get_object (node);
	} else {
		return NULL;
	}

	if (!json_object_has_member (object, name)) {
		return NULL;
	}

	subnode = json_object_get_member (object, name);

	if (!subnode)
		return NULL;

	json_node_get_value (subnode, &value);

	string = g_value_get_string (&value);

	if (string && string[0]) {
		result = g_strdup (string);
	}

	g_value_unset (&value);

	return result;
}
