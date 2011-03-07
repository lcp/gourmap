#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>

#include "gourmap-ui.h"

int
main (int argc, char **argv)
{
	GourmapUi *ui;

#ifdef ENABLE_NLS
	bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
	textdomain (GETTEXT_PACKAGE);
#endif

	gtk_init (&argc, &argv);

	ui = gourmap_ui_new ();

	gtk_main ();

	return 0;
}
