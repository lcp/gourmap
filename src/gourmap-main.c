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

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <clutter-gtk/clutter-gtk.h>

#include "gourmap-coord.h"

int
main (int argc, char **argv)
{
	GourmapCoord *coord;
	const char *poi_db_filename = NULL;
	GOptionContext *context;
	const GOptionEntry options[] = {
		{ "import", 'i', 0, G_OPTION_ARG_STRING, &poi_db_filename,
		  /* TRANSLATORS: import another POI database instead of the default one */
		  _("Import a POI database"), NULL },
		{ NULL }
	};

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	if (gtk_clutter_init (&argc, &argv) != CLUTTER_INIT_SUCCESS)
		return -1;

	context = g_option_context_new ("Gourmap");
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	coord = gourmap_coord_new ();

	if (poi_db_filename == NULL)
		poi_db_filename = PACKAGE_DATA_DIR"/gourmap/poi.db";
	gourmap_coord_import_poi_db (coord, poi_db_filename);

	gourmap_coord_startup (coord);

	gtk_main ();

	g_object_unref (coord);

	return 0;
}
