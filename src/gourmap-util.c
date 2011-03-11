/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2011 Gary Ching-Pang Lin <glin@novell.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

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
