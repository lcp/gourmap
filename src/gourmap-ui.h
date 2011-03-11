#ifndef __GOURMAP_UI_H__
#define __GOURMAP_UI_H__

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define GOURMAP_TYPE_UI         (gourmap_ui_get_type ())
#define GOURMAP_UI(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), GOURMAP_TYPE_UI, GourmapUi))
#define GOURMAP_UI_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), GOURMAP_TYPE_UI, GourmapUiClass))
#define GOURMAP_IS_UI(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), GOURMAP_TYPE_UI))
#define GOURMAP_IS_UI_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), GOURMAP_TYPE_UI))
#define GOURMAP_UI_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), GOURMAP_TYPE_UI, GourmapUiClass))

typedef struct GourmapUiPrivate GourmapUiPrivate;

typedef struct
{
	GObject			 parent;
	GourmapUiPrivate	*priv;
} GourmapUi;

typedef struct
{
	GObjectClass		 parent_class;

	void			(*ui_addr_updated) (GourmapUi *ui, const char *addr);
	void			(*ui_map_redraw) (GourmapUi *ui);
	void			(*ui_random) (GourmapUi *ui);
	void			(*ui_rest_selected) (GourmapUi *ui, guint index);
} GourmapUiClass;

GType		 gourmap_ui_get_type	(void);
GourmapUi	*gourmap_ui_new		(void);

void		 gourmap_ui_set_zoom	(GourmapUi	*ui,
					 unsigned int	 zoom);
void		 gourmap_ui_set_radius	(GourmapUi	*ui,
					 unsigned int	 radius);
void		 gourmap_ui_update_map	(GourmapUi	*ui,
					 const double	 my_lat,
					 const double	 my_lng,
					 const double	 center_lat,
					 const double	 center_lng,
					 GList		*poi_list);
void		 gourmap_ui_update_list (GourmapUi	*ui,
					 GList		*poi_list);
G_END_DECLS

#endif /* __GOURMAP_UI_H__ */
