#ifndef __GOURMAP_COORD_H__
#define __GOURMAP_COORD_H__

#include <glib-object.h>

G_BEGIN_DECLS

#define GOURMAP_TYPE_COORD         (gourmap_coord_get_type ())
#define GOURMAP_COORD(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GOURMAP_TYPE_COORD, GourmapCoord))
#define GOURMAP_COORD_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GOURMAP_TYPE_COORD, GourmapCoordClass))
#define GOURMAP_IS_COORD(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GOURMAP_TYPE_COORD))
#define GOURMAP_IS_COORD_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GOURMAP_TYPE_COORD))
#define GOURMAP_COORD_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GOURMAP_TYPE_COORD, GourmapCoordClass))

typedef struct GourmapCoordPrivate GourmapCoordPrivate;

typedef struct
{
	GObject parent;
	GourmapCoordPrivate	*priv;
} GourmapCoord;

typedef struct
{
	GObjectClass		 parent_class;
} GourmapCoordClass;

GType		 gourmap_coord_get_type		(void);
GourmapCoord	*gourmap_coord_new		(void);
gboolean	 gourmap_coord_import_poi_db	(GourmapCoord	*coord,
						 const char	*db_filename);

G_END_DECLS

#endif /* __GOURMAP_COORD_H__ */
