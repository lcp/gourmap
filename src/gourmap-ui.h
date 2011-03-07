#ifndef __GOURMAP_UI_H__
#define __GOURMAP_UI_H__

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
        GObject parent;
        GourmapUiPrivate        *priv;
} GourmapUi;

typedef struct
{
        GObjectClass             parent_class;
} GourmapUiClass;

GType            gourmap_ui_get_type            (void);
GourmapUi       *gourmap_ui_new                 (void);

G_END_DECLS

#endif /* __GOURMAP_UI_H__ */
