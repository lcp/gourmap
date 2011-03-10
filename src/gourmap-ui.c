#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <webkit/webkit.h>

#include <rest/rest-proxy.h>
#include <json-glib/json-glib.h>

#include "google-map-template.h"
#include "gourmap-ui.h"
#include "gourmap-poi.h"
#include "gourmap-util.h"

enum {
	UI_ADDR_UPDATED,
	UI_MAP_REDRAW,
	LAST_SIGNAL
};

static int signals[LAST_SIGNAL] = { 0 };

struct GourmapUiPrivate
{
	GtkWidget *main_window;
	GtkWidget *map;
	GtkWidget *addr_entry;
	WebKitWebView *web_view;
	unsigned int zoom;
	unsigned int radius;
};

G_DEFINE_TYPE (GourmapUi, gourmap_ui, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                        GOURMAP_TYPE_UI, GourmapUiPrivate))

void
gourmap_ui_set_zoom (GourmapUi    *ui,
		     unsigned int  zoom)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	priv->zoom = zoom;
}

void
gourmap_ui_set_radius (GourmapUi    *ui,
		       unsigned int  radius)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	priv->radius = radius;
}

char *
construct_poi_markers (GList *poi_list)
{
	char *poi_array = NULL;
	char *poi_markers = NULL;
	GList *list_i;
	Restaurant *rest;

	list_i = poi_list;
	while (list_i) {
		char *prev;
		rest = (Restaurant *)list_i->data;
		if (poi_array != NULL) {
			prev = poi_array;
			poi_array = g_strdup_printf ("%s [\'%s\', %.6f, %.6f, \'%s\'],",
						     prev,
						     rest->name,
						     rest->latitude,
						     rest->longitude,
						     rest->address);
			g_free (prev);
		} else {
			poi_array = g_strdup_printf ("[\'%s\', %.6f, %.6f, \'%s\'],",
						     rest->name,
						     rest->latitude,
						     rest->longitude,
						     rest->address);
		}
		list_i = list_i->next;
	}

	if (poi_array == NULL)
		return NULL;

	poi_markers = g_strdup_printf (gmap_restaurant_markers,
				       poi_array);

	g_free (poi_array);

	return poi_markers;
}

void
gourmap_ui_update_map (GourmapUi    *ui,
		       const double  latitude,
		       const double  longitude,
		       GList        *poi_list)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	char *map_html, *poi_markers = NULL;

	if (poi_list != NULL) {
		poi_markers = construct_poi_markers (poi_list);
	}

	map_html = g_strdup_printf (google_map_template,
				    latitude,      /* Map Center latitude  */
				    longitude,     /* Map Center longitude */
				    priv->zoom,    /* Zoom Level */
				    priv->radius,  /* Circle Radius */
                                    poi_markers);
	webkit_web_view_load_string (WEBKIT_WEB_VIEW (priv->web_view),
				     map_html,
				     "text/html",
				     "UTF-8",
				     "");
	g_free (map_html);
	g_free (poi_markers);
}

static void
destroy_cb (GtkWidget *widget, gpointer data)
{
    gtk_main_quit ();
}

static void
activate_addr_entry_cb (GtkWidget *entry, gpointer data)
{
	GourmapUi *ui = GOURMAP_UI (data);
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	const gchar* addr;

	addr = gtk_entry_get_text (GTK_ENTRY (priv->addr_entry));

	if (addr[0] == '\0') {
		g_signal_emit (G_OBJECT (ui), signals[UI_MAP_REDRAW], 0);
	} else {
		g_signal_emit (G_OBJECT (ui), signals[UI_ADDR_UPDATED], 0, addr);
	}
}

static void
create_map_window (GourmapUi *ui)
{
	GourmapUiPrivate *priv;
	GtkWidget *vbox;
	GtkWidget *toolbar;
	GtkWidget *addr_label;
	GtkToolItem *item;

	priv = GET_PRIVATE (ui);
	vbox = gtk_vbox_new (FALSE, 0);

	/* map */
	priv->map = gtk_scrolled_window_new (NULL, NULL);
	priv->web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->map),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (priv->map), GTK_WIDGET (priv->web_view));

	gtk_box_pack_start (GTK_BOX (vbox), priv->map, TRUE, TRUE, 0);

	/* address */
	toolbar = gtk_toolbar_new ();
	item = gtk_tool_item_new ();
	addr_label = gtk_label_new (_("Address"));
	gtk_container_add (GTK_CONTAINER (item), addr_label);
	gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

	item = gtk_tool_item_new ();
	gtk_tool_item_set_expand (item, TRUE);
	priv->addr_entry = gtk_entry_new ();
	gtk_container_add (GTK_CONTAINER (item), priv->addr_entry);
	g_signal_connect (G_OBJECT (priv->addr_entry),
			  "activate",
			  G_CALLBACK (activate_addr_entry_cb),
			  (gpointer) ui);
	gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

	item = gtk_tool_button_new_from_stock (GTK_STOCK_OK);
	g_signal_connect (G_OBJECT (item),
			  "clicked",
			  G_CALLBACK (activate_addr_entry_cb),
			  (gpointer) ui);
	gtk_toolbar_insert (GTK_TOOLBAR (toolbar), item, -1);

	gtk_box_pack_start (GTK_BOX (vbox), toolbar, FALSE, FALSE, 0);

	/* main window */
	priv->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (priv->main_window), 1024, 768);
	gtk_widget_set_name (priv->main_window, _("Gourmap"));
	g_signal_connect (G_OBJECT (priv->main_window),
			  "destroy",
			  G_CALLBACK (destroy_cb),
			  NULL);

	gtk_container_add (GTK_CONTAINER (priv->main_window), vbox);
}

static void
gourmap_ui_init (GourmapUi *ui)
{
	GourmapUiPrivate *priv;

	priv = GET_PRIVATE (ui);

	priv->zoom = 16;
	priv->radius = 850;

	create_map_window (ui);

	gtk_widget_grab_focus (priv->addr_entry);
	gtk_widget_show_all (priv->main_window);
}

static void
gourmap_ui_finalize (GObject *object)
{
	GourmapUiPrivate *priv = GET_PRIVATE (object);
	G_OBJECT_CLASS(gourmap_ui_parent_class)->finalize(object);
}

static void
gourmap_ui_class_init (GourmapUiClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (klass, sizeof (GourmapUiPrivate));
	object_class->finalize = gourmap_ui_finalize;

	signals[UI_ADDR_UPDATED] =
		g_signal_new ("ui-addr-updated",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GourmapUiClass, ui_addr_updated),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__STRING,
			      G_TYPE_NONE, 1, G_TYPE_STRING);
	signals[UI_MAP_REDRAW] =
		g_signal_new ("ui-map-redraw",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GourmapUiClass, ui_map_redraw),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0, G_TYPE_NONE);
}

GourmapUi *
gourmap_ui_new ()
{
	GourmapUi *ui;
	ui = GOURMAP_UI (g_object_new (GOURMAP_TYPE_UI, NULL));
	return ui;
}
