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
gboolean	 gourmap_coord_startup		(GourmapCoord	*coord);

G_END_DECLS

#endif /* __GOURMAP_COORD_H__ */
