#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <rest/rest-proxy.h>
#include <json-glib/json-glib.h>

#include "gourmap-coord.h"
#include "gourmap-ui.h"
#include "gourmap-poi.h"
#include "gourmap-util.h"

struct GourmapCoordPrivate
{
	GourmapUi *ui;
	GourmapPoi *poi;
	RestProxy *proxy;
	char *db_file;
	double current_lat;
	double current_lng;
	unsigned int zoom;
	unsigned int radius;
	GList *poi_list;
};

G_DEFINE_TYPE (GourmapCoord, gourmap_coord, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                        GOURMAP_TYPE_COORD, GourmapCoordPrivate))

gboolean
gourmap_coord_import_poi_db (GourmapCoord *coord,
			     const char   *db_filename)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (coord);

	return gourmap_poi_import_db (priv->poi, db_filename);
}

static void
_got_gmap_geocode (RestProxyCall *call,
		   GError        *error,
		   GObject       *weak_object,
		   gpointer       userdata)
{
	GourmapCoord *coord = GOURMAP_COORD (weak_object);
	GourmapCoordPrivate *priv = GET_PRIVATE (coord);
	JsonNode *root, *node;
	JsonObject *obj;
	JsonArray *array;
	double lat, lng;

	root = json_node_from_call (call);
	if (!root) {
		g_message ("Not a vaild json");
		/* TODO notification for user */
		return;
	}

	/* interpret json */
	/* http://code.google.com/intl/zh-TW/apis/maps/documentation/geocoding/index.html#JSON */
	obj = json_node_get_object (root);
	if (!json_object_has_member (obj, "results")) {
		g_message ("No results");
		/* TODO notification for user */
		return;
	}
	node = json_object_get_member (obj, "results");
	array = json_node_get_array (node);

	/* Always take the first result :-p */
	node = json_array_get_element (array, 0);
	obj = json_node_get_object (node);
	node = json_object_get_member (obj, "geometry");
	obj = json_node_get_object (node);
	node = json_object_get_member (obj, "location");
	obj = json_node_get_object (node);

	lat = json_object_get_double_member (obj, "lat");
	lng = json_object_get_double_member (obj, "lng");

	g_debug ("lat = %.6f, lng = %.6f", lat, lng);

	if (priv->current_lat != lat || priv->current_lng != lng) {
		g_list_free (priv->poi_list);
		priv->poi_list = gourmap_poi_find_poi (priv->poi,
						       lat,
						       lng,
						       priv->radius);
	}

	gourmap_ui_update_map (priv->ui, lat, lng, priv->poi_list);

	priv->current_lat = lat;
	priv->current_lng = lng;
}

static void
gourmap_coord_addr_updated_cb (GourmapUi    *ui,
			       const char   *address,
			       GourmapCoord *coord)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (coord);
	RestProxyCall *call;

	if (!priv->proxy || !address)
		return;

	call = rest_proxy_new_call (priv->proxy);
	rest_proxy_call_set_function (call, "geocode/json");

	rest_proxy_call_add_params (call,
				    "address", address,
				    "sensor", "false",
				    NULL);

	rest_proxy_call_async (call, _got_gmap_geocode, (GObject*)coord, NULL, NULL);
}

static void
gourmap_coord_map_redraw_cb (GourmapUi    *ui,
			     GourmapCoord *coord)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (coord);

	if (priv->poi_list == NULL) {
		priv->poi_list = gourmap_poi_find_poi (priv->poi,
						       priv->current_lat,
						       priv->current_lng,
						       priv->radius);
	}

	gourmap_ui_update_map (priv->ui,
			       priv->current_lat,
			       priv->current_lng,
			       priv->poi_list);
}

static void
gourmap_coord_random_button_cb (GourmapUi    *ui,
				GourmapCoord *coord)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (coord);

	g_debug ("random button clicked");

	if (priv->poi_list == NULL)
		return;

	/* TODO select a restaurant randomly */
}

static void
gourmap_coord_init (GourmapCoord *coord)
{
	GourmapCoordPrivate *priv;
	priv = GET_PRIVATE (coord);

	priv->proxy = rest_proxy_new ("http://maps.google.com/maps/api/", FALSE);
	/* Default location: Taipei 101 building */
	priv->current_lat = 25.033867;
	priv->current_lng = 121.564126;
	priv->zoom = 16;
	priv->radius = 850;

	priv->db_file = NULL;
	priv->poi = gourmap_poi_new ();
	priv->poi_list = NULL;

	priv->ui = gourmap_ui_new ();
	gourmap_ui_set_zoom (priv->ui, priv->zoom);
	gourmap_ui_set_radius (priv->ui, priv->radius);
	g_signal_connect (G_OBJECT (priv->ui),
			  "ui-addr-updated",
			  G_CALLBACK (gourmap_coord_addr_updated_cb),
			  (gpointer) coord);
	g_signal_connect (G_OBJECT (priv->ui),
			  "ui-map-redraw",
			  G_CALLBACK (gourmap_coord_map_redraw_cb),
			  (gpointer) coord);
	g_signal_connect (G_OBJECT (priv->ui),
			  "ui-random",
			  G_CALLBACK (gourmap_coord_random_button_cb),
			  (gpointer) coord);

	gourmap_coord_map_redraw_cb (priv->ui, coord);
}

static void
gourmap_coord_dispose (GObject *object)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (object);

	g_object_unref (priv->proxy);
	g_object_unref (priv->ui);
	g_object_unref (priv->poi);

	G_OBJECT_CLASS (gourmap_coord_parent_class)->dispose (object);
}

static void
gourmap_coord_finalize (GObject *object)
{
	GourmapCoordPrivate *priv = GET_PRIVATE (object);

	g_free (priv->db_file);

	G_OBJECT_CLASS (gourmap_coord_parent_class)->finalize (object);
}

static void
gourmap_coord_class_init (GourmapCoordClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (klass, sizeof (GourmapCoordPrivate));
	object_class->dispose = gourmap_coord_dispose;
	object_class->finalize = gourmap_coord_finalize;
}

GourmapCoord *
gourmap_coord_new ()
{
	GourmapCoord *coord;
	coord = GOURMAP_COORD (g_object_new (GOURMAP_TYPE_COORD, NULL));
	return coord;
}
