/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2011 Gary Ching-Pang Lin <glin@suse.com>
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
