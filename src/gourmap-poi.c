#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <stdio.h>
#include <math.h>
#include <glib.h>

#include "gourmap-poi.h"

struct GourmapPoiPrivate
{
	GList *restaurants;
};

G_DEFINE_TYPE (GourmapPoi, gourmap_poi, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o),\
			GOURMAP_TYPE_POI, GourmapPoiPrivate))

#define DEG_TO_RAD 0.017453292519943295769236907684886
#define EARTH_RADIUS_IN_METERS 6372797.560856

gboolean
gourmap_poi_import_db (GourmapPoi *poi,
		       const char *db_filename)
{
	GourmapPoiPrivate *priv = GET_PRIVATE (poi);
	GKeyFile *db_file = g_key_file_new ();
	char **names;

	if (!g_key_file_load_from_file (db_file, db_filename, G_KEY_FILE_NONE, NULL)) {
		g_warning ("Failed to open data file: %s", db_filename);
		return FALSE;
	}

	names = g_key_file_get_groups (db_file, NULL);

	for (int i = 0; names[i]; i++) {
		Restaurant *restaurant = g_new0 (Restaurant, 1);
		restaurant->name = g_strdup (names[i]);
		restaurant->latitude = g_key_file_get_double (db_file, names[i], "Latitude", NULL);
		restaurant->longitude = g_key_file_get_double (db_file, names[i], "Longitude", NULL);
		restaurant->address = g_key_file_get_string (db_file, names[i], "Address", NULL);

		priv->restaurants = g_list_append (priv->restaurants, (gpointer)restaurant);
	}

	g_strfreev (names);

	return TRUE;
}

static double
sphere_distance (double from_lat,
		 double from_lng,
		 double to_lat,
		 double to_lng)
{
	/* http://en.wikipedia.org/wiki/Haversine_formula */
	double arc_lat = (from_lat - to_lat) * DEG_TO_RAD;
	double arc_lng = (from_lng - to_lng) * DEG_TO_RAD;
	double lat_h = sin(arc_lat * 0.5);
	double lng_h = sin(arc_lng * 0.5);
	double tmp = cos(from_lat*DEG_TO_RAD) * cos(to_lat*DEG_TO_RAD);
	lat_h *= lat_h;
	lng_h *= lng_h;

	return (2.0 * asin(sqrt(lat_h + tmp*lng_h))) * EARTH_RADIUS_IN_METERS;
}

GList *
gourmap_poi_find_poi (GourmapPoi *poi,
		      double latitude,
		      double longitude,
		      unsigned int radius)
{
	GourmapPoiPrivate *priv = GET_PRIVATE (poi);
	Restaurant *rest;
	GList *list_i;
	GList *poi_list = NULL;

	list_i = priv->restaurants;
	while (list_i) {
		double dist;
		rest = (Restaurant *)list_i->data;
		dist = sphere_distance (rest->latitude,
					rest->longitude,
					latitude,
					longitude);
		if (dist <= radius)
			poi_list = g_list_append (poi_list, (gpointer)rest);

		list_i = list_i->next;
	}

	return poi_list;
}

static void
gourmap_poi_finalize (GObject *object)
{
	GourmapPoiPrivate *priv = GET_PRIVATE (object);
	Restaurant *rest;
	GList *list_i;

	list_i = priv->restaurants;
	while (list_i) {
		rest = (Restaurant *)list_i->data;
		g_free (rest->name);
		g_free (rest->address);
		g_free (rest);
		list_i = list_i->next;
	}
	g_list_free (priv->restaurants);
	G_OBJECT_CLASS (gourmap_poi_parent_class)->finalize (object);
}

static void
gourmap_poi_class_init (GourmapPoiClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GourmapPoiPrivate));
	object_class->finalize = gourmap_poi_finalize;
}

static void
gourmap_poi_init (GourmapPoi *self)
{
	GourmapPoiPrivate *priv = GET_PRIVATE (self);
	priv->restaurants = NULL;
}

GourmapPoi*
gourmap_poi_new (void)
{
  return g_object_new (GOURMAP_TYPE_POI, NULL);
}

