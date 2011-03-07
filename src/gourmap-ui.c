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
#include "gourmap-util.h"

struct GourmapUiPrivate
{
	GtkWidget *main_window;
	GtkWidget *map;
	GtkWidget *addr_entry;
	WebKitWebView *web_view;
	RestProxy *proxy;
	double current_lat;
	double current_lng;
	unsigned int zoom;
};

G_DEFINE_TYPE (GourmapUi, gourmap_ui, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                        GOURMAP_TYPE_UI, GourmapUiPrivate))

static void
update_map (GourmapUi *ui, double latitude, double longitude)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	char *map_html;

	map_html = g_strdup_printf (google_map_template,
				    latitude,
				    longitude,
				    priv->zoom);
	webkit_web_view_load_string (WEBKIT_WEB_VIEW (priv->web_view),
				     map_html,
				     "text/html",
				     "UTF-8",
				     "");
	g_free (map_html);

	priv->current_lat = latitude;
	priv->current_lng = longitude;
}

static void
_got_gmap_geocode (RestProxyCall *call,
		   GError        *error,
		   GObject       *weak_object,
		   gpointer       userdata)
{
	GourmapUi *ui = GOURMAP_UI (weak_object);
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

	update_map (ui, lat, lng);
}

static gboolean
request_geocode (GourmapUi *ui, const char *address)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	RestProxyCall *call;

	if (!priv->proxy || !address)
		return FALSE;

	call = rest_proxy_new_call (priv->proxy);
	rest_proxy_call_set_function (call, "geocode/json");

	rest_proxy_call_add_params (call,
				    "address", address,
				    "sensor", "false",
				    NULL);

	rest_proxy_call_async (call, _got_gmap_geocode, (GObject*)ui, NULL, NULL);

	return TRUE;
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
		update_map (ui, priv->current_lat, priv->current_lng);
	} else {
		request_geocode (ui, addr);
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
	update_map (ui, 25.026218, 121.543545);

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
	priv->proxy = rest_proxy_new ("http://maps.google.com/maps/api/", FALSE);

	priv->zoom = 16;

	create_map_window (ui);

	gtk_widget_grab_focus (priv->addr_entry);
	gtk_widget_show_all (priv->main_window);
}

static void
gourmap_ui_class_init (GourmapUiClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (klass, sizeof (GourmapUiPrivate));
}

GourmapUi *
gourmap_ui_new ()
{
	GourmapUi *ui;
	ui = GOURMAP_UI (g_object_new (GOURMAP_TYPE_UI, NULL));
	return ui;
}
