#ifndef __GOURMAP_UTIL_H__
#define __GOURMAP_UTIL_H__

#include <libsoup/soup.h>
#include <rest/rest-proxy.h>
#include <json-glib/json-glib.h>

JsonNode	*json_node_from_call	(RestProxyCall	*call);

#endif /* __GOURMAP_UTIL_H__ */
