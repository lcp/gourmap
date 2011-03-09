#ifndef __GOURMAP_POI_H_
#define __GOURMAP_POI_H_

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GOURMAP_TYPE_POI gourmap_poi_get_type()

#define GOURMAP_POI(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GOURMAP_TYPE_POI, GourmapPoi))
#define GOURMAP_POI_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GOURMAP_TYPE_POI, GourmapPoiClass))
#define GOURMAP_IS_POI(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GOURMAP_TYPE_POI))
#define GOURMAP_IS_POI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GOURMAP_TYPE_POI))
#define GOURMAP_POI_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GOURMAP_TYPE_POI, GourmapPoiClass))

typedef struct
{
	double latitude;
	double longitude;
	char *name;
	char *address;
} Restaurant;

typedef struct GourmapPoiPrivate GourmapPoiPrivate;

typedef struct
{
	GObject parent;
	GourmapPoiPrivate        *priv;
} GourmapPoi;

typedef struct
{
	GObjectClass parent_class;
} GourmapPoiClass;

GType		 gourmap_poi_get_type	(void);
GourmapPoi	*gourmap_poi_new	(void);

gboolean	 gourmap_poi_import_db	(GourmapPoi	*poi,
					 const char	*db_filename);
GList		*gourmap_poi_find_poi	(GourmapPoi	*poi,
					 double		 latitude,
					 double		 longitude,
					 unsigned int	 radius);

G_END_DECLS

#endif /* __GOURMAP_POI_H__ */
