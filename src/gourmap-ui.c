/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*-
 *
 * Copyright (C) 2013 Gary Ching-Pang Lin <glin@suse.com>
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

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <champlain-gtk/champlain-gtk.h>

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
	unsigned int zoom;
	unsigned int radius;

	ChamplainView *champ_view;
	ChamplainMarkerLayer *marker_layer;
};

G_DEFINE_TYPE (GourmapUi, gourmap_ui, G_TYPE_OBJECT)

#define GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
                        GOURMAP_TYPE_UI, GourmapUiPrivate))

void
gourmap_ui_show_up (GourmapUi *ui)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);

	gtk_widget_grab_focus (priv->addr_entry);
	gtk_widget_show_all (priv->main_window);
}

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

void
gourmap_ui_set_addr (GourmapUi  *ui,
		     const char *addr)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	gtk_entry_set_text (GTK_ENTRY (priv->addr_entry), addr);
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
	ClutterActor *marker;
	ClutterColor *color;
	Restaurant *rest;
	GList *list_i;

	/* remove previous markers */
	champlain_marker_layer_remove_all (priv->marker_layer);

	champlain_view_set_zoom_level (priv->champ_view, priv->zoom);

	champlain_view_go_to (priv->champ_view, my_lat, my_lng);

	color = clutter_color_new (0xff, 0x20, 0x15, 0xbb);
	marker = champlain_label_new_with_text (_("I'm here"),
						"Serif 14", NULL, color);
	clutter_color_free (color);
	champlain_location_set_location (CHAMPLAIN_LOCATION (marker),
					 my_lat, my_lng);
	champlain_marker_layer_add_marker (priv->marker_layer,
					   CHAMPLAIN_MARKER (marker));

	/* Put restaurant markers */
	list_i = poi_list;
	color = clutter_color_new (0xff, 0x20, 0xff, 0xbb);
	while (list_i != NULL) {
		rest = (Restaurant *)list_i->data;
		marker = champlain_label_new_with_text (rest->name,
							"Serif 10",
							NULL, color);
		champlain_location_set_location (CHAMPLAIN_LOCATION (marker),
						 rest->latitude,
						 rest->longitude);
		champlain_marker_layer_add_marker (priv->marker_layer,
						   CHAMPLAIN_MARKER (marker));
		list_i = g_list_next (list_i);
	}
	clutter_color_free (color);

	champlain_marker_layer_show_all_markers (priv->marker_layer);
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


void
gourmap_ui_select (GourmapUi  *ui,
		   const guint index)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	GtkTreeModel *model = GTK_TREE_MODEL (priv->store);
	GtkTreeIter iter;
	GtkTreeSelection *select;

	g_debug ("random selection: %u", index);

	select = gtk_tree_view_get_selection (GTK_TREE_VIEW (priv->treeview));
	gtk_tree_model_iter_nth_child (model, &iter, NULL, index);
	gtk_tree_selection_select_iter (select, &iter);
}

void
gourmap_ui_warning (GourmapUi  *ui,
		    const char *str)
{
	GourmapUiPrivate *priv = GET_PRIVATE (ui);
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new (GTK_WINDOW (priv->main_window),
					 GTK_DIALOG_MODAL,
					 GTK_MESSAGE_WARNING,
					 GTK_BUTTONS_OK,
					 "%s", str);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
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
	hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
	vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

	/* map */
	priv->map = gtk_champlain_embed_new ();
	priv->champ_view = gtk_champlain_embed_get_view (GTK_CHAMPLAIN_EMBED (priv->map));
	clutter_actor_set_reactive (CLUTTER_ACTOR (priv->champ_view), TRUE);
	g_object_set (G_OBJECT (priv->champ_view),
		      "kinetic-mode", TRUE,
		      NULL);
	priv->marker_layer = champlain_marker_layer_new_full (CHAMPLAIN_SELECTION_SINGLE);
	champlain_view_add_layer (priv->champ_view, CHAMPLAIN_LAYER (priv->marker_layer));

	/* sidebar */
	vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);

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
