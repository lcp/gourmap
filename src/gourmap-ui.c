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
	UI_RANDOM,
	UI_REST_SELECTED,
	LAST_SIGNAL
};

static int signals[LAST_SIGNAL] = { 0 };

enum
{
	NAME_COLUMN,
	INDEX_COLUMN,
	N_COLUMNS
};

struct GourmapUiPrivate
{
	GtkWidget *main_window;
	GtkWidget *map;
	GtkWidget *addr_entry;
	GtkWidget *treeview;
	GtkWidget *rand_button;
	GtkTreeStore *store;
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
		       const double  my_lat,
		       const double  my_lng,
		       const double  center_lat,
		       const double  center_lng,
		       GList        *poi_list)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	char *map_html, *poi_markers = NULL;

	if (poi_list != NULL) {
		poi_markers = construct_poi_markers (poi_list);
	}

	map_html = g_strdup_printf (google_map_template,
				    my_lat,        /* My latitude  */
				    my_lng,        /* My longitude */
				    center_lat,    /* Map Center latitude  */
				    center_lng,    /* Map Center longitude */
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


void
gourmap_ui_update_list (GourmapUi *ui,
		        GList     *poi_list)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	GList *list_i;
	guint index = 0;

	gtk_tree_store_clear (priv->store);

	if (poi_list == NULL) {
		gtk_widget_set_sensitive (priv->rand_button, FALSE);
		return;
	}

	for (list_i = poi_list; list_i; list_i = list_i->next) {
		Restaurant *rest = (Restaurant *)list_i->data;
		GtkTreeIter iter;
		gtk_tree_store_append (priv->store, &iter, NULL);
		gtk_tree_store_set (priv->store, &iter,
				    NAME_COLUMN, rest->name,
				    INDEX_COLUMN, index++,
				    -1);
	}

	gtk_widget_set_sensitive (priv->rand_button, TRUE);
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
random_button_cb (GtkWidget *button, gpointer data)
{
	GourmapUi *ui = GOURMAP_UI (data);
	g_signal_emit (G_OBJECT (ui), signals[UI_RANDOM], 0);
}

static void
tree_selection_changed_cb (GtkTreeSelection *selection,
			   gpointer data)
{
	GourmapUi *ui = GOURMAP_UI (data);
	GtkTreeIter iter;
	GtkTreeModel *model;
	char *name;
	guint index;

	if (gtk_tree_selection_get_selected (selection, &model, &iter)) {
		gtk_tree_model_get (model, &iter,
				   NAME_COLUMN, &name,
				   INDEX_COLUMN, &index,
				   -1);
		g_debug ("%s : %u is selected", name, index);

		g_signal_emit (G_OBJECT (ui), signals[UI_REST_SELECTED], 0, index);

		g_free (name);
	}
}

static void
create_map_window (GourmapUi *ui)
{
	GourmapUiPrivate *priv;
	GtkWidget *hbox;
	GtkWidget *vbox1, *vbox2;
	GtkWidget *toolbar;
	GtkWidget *addr_label;
	GtkToolItem *item;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeSelection *select;

	priv = GET_PRIVATE (ui);
	hbox = gtk_hbox_new (FALSE, 0);
	vbox1 = gtk_vbox_new (FALSE, 0);
	vbox2 = gtk_vbox_new (FALSE, 0);

	/* map */
	priv->map = gtk_scrolled_window_new (NULL, NULL);
	priv->web_view = WEBKIT_WEB_VIEW (webkit_web_view_new ());
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (priv->map),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_container_add (GTK_CONTAINER (priv->map), GTK_WIDGET (priv->web_view));


	/* sidebar */
	vbox2 = gtk_vbox_new (FALSE, 0);

	/* restaurant list */
	priv->store = gtk_tree_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_UINT);
	priv->treeview = gtk_tree_view_new_with_model (GTK_TREE_MODEL (priv->store));
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (_("Restaurant List"),
							   renderer,
							   "text", NAME_COLUMN,
							   NULL);
	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
	gtk_tree_selection_set_mode (select, GTK_SELECTION_SINGLE);
	g_signal_connect (G_OBJECT (select), "changed",
			  G_CALLBACK (tree_selection_changed_cb),
			  (gpointer) ui);
	gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), column);

	gtk_box_pack_start (GTK_BOX (vbox2), priv->treeview, TRUE, TRUE, 0);

	/* random button */
	priv->rand_button = gtk_button_new_with_label (_("Random Selection!"));
	g_signal_connect (G_OBJECT (priv->rand_button),
			  "clicked",
			  G_CALLBACK (random_button_cb),
			  (gpointer) ui);
	gtk_box_pack_start (GTK_BOX (vbox2), priv->rand_button, FALSE, FALSE, 0);

	gtk_box_pack_start (GTK_BOX (hbox), priv->map, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox2, FALSE, FALSE, 0);

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

	gtk_box_pack_start (GTK_BOX (vbox1), hbox, TRUE, TRUE, 0);
	gtk_box_pack_start (GTK_BOX (vbox1), toolbar, FALSE, FALSE, 0);

	/* main window */
	priv->main_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (priv->main_window), 1024, 768);
	gtk_widget_set_name (priv->main_window, _("Gourmap"));
	g_signal_connect (G_OBJECT (priv->main_window),
			  "destroy",
			  G_CALLBACK (destroy_cb),
			  NULL);

	gtk_container_add (GTK_CONTAINER (priv->main_window), vbox1);
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
	signals[UI_RANDOM] =
		g_signal_new ("ui-random",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GourmapUiClass, ui_random),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__VOID,
			      G_TYPE_NONE, 0, G_TYPE_NONE);
	signals[UI_REST_SELECTED] =
		g_signal_new ("ui-rest-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST,
			      G_STRUCT_OFFSET (GourmapUiClass, ui_rest_selected),
			      NULL, NULL,
			      g_cclosure_marshal_VOID__UINT,
			      G_TYPE_NONE, 1, G_TYPE_UINT);
}

GourmapUi *
gourmap_ui_new ()
{
	GourmapUi *ui;
	ui = GOURMAP_UI (g_object_new (GOURMAP_TYPE_UI, NULL));
	return ui;
}
