#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gourmap-coord.h"

int
main (int argc, char **argv)
{
	GourmapCoord *coord;
	const char *poi_db_filename = NULL;
	GOptionContext *context;
	const GOptionEntry options[] = {
		{ "import", 'i', 0, G_OPTION_ARG_STRING, &poi_db_filename,
		  /* TRANSLATORS: use another config file instead of the default one */
		  _("Import a POI database"), NULL },
		{ NULL }
	};

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init (&argc, &argv);

	context = g_option_context_new ("Gourmap");
	g_option_context_add_main_entries (context, options, NULL);
	g_option_context_parse (context, &argc, &argv, NULL);
	g_option_context_free (context);

	coord = gourmap_coord_new ();
	gourmap_coord_import_poi_db (coord, poi_db_filename);

	gtk_main ();

	g_object_unref (coord);

	return 0;
}
