#ifndef __GOURMAP_UTIL_H__
#define __GOURMAP_UTIL_H__

#include <libsoup/soup.h>
#include <rest/rest-proxy.h>
#include <json-glib/json-glib.h>

JsonNode	*json_node_from_call	(RestProxyCall	*call);

char		*get_child_node_value	(JsonNode	*node,
					 const char	*name);
#endif /* __GOURMAP_UTIL_H__ */
