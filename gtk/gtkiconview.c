/* gtkiconview.c
 * Copyright (C) 2002, 2004  Anders Carlsson <andersca@gnu.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include <string.h>

#include <gdk/gdkkeysyms.h>

#include "gtkiconview.h"
#include "gtkcelllayout.h"
#include "gtkcellrenderer.h"
#include "gtkcellrenderertext.h"
#include "gtkcellrendererpixbuf.h"

#include "gtkbindings.h"
#include "gtkdnd.h"
#include "gtkmain.h"
#include "gtkintl.h"
#include "gtkwindow.h"
#include "gtkentry.h"
#include "gtkcombobox.h"
#include "gtktextbuffer.h"
#include "gtktreednd.h"
#include "gtkprivate.h"


#undef DEBUG_ICON_VIEW

#define SCROLL_EDGE_SIZE 15

#define GTK_ICON_VIEW_GET_PRIVATE(obj) ((GtkIconViewPrivate*)gtk_icon_view_get_instance_private ((GtkIconView*)obj))

typedef struct _GtkIconViewItem GtkIconViewItem;
struct _GtkIconViewItem
{
  GtkTreeIter iter;
  gint index;
  
  gint row, col;

  /* Bounding box */
  gint x, y, width, height;

  /* Individual cells.
   * box[i] is the actual area occupied by cell i,
   * before, after are used to calculate the cell 
   * area relative to the box. 
   * See gtk_icon_view_get_cell_area().
   */
  gint n_cells;
  GdkRectangle *box;
  gint *before;
  gint *after;

  guint selected : 1;
  guint selected_before_rubberbanding : 1;

};

typedef struct _GtkIconViewCellInfo GtkIconViewCellInfo;
struct _GtkIconViewCellInfo
{
  GtkCellRenderer *cell;

  guint expand : 1;
  guint pack : 1;
  guint editing : 1;

  gint position;

  GSList *attributes;

  GtkCellLayoutDataFunc func;
  gpointer func_data;
  GDestroyNotify destroy;
};

typedef struct _GtkIconViewChild GtkIconViewChild;
struct _GtkIconViewChild
{
  GtkWidget *widget;
  GtkIconViewItem *item;
  gint cell;
};

struct _GtkIconViewPrivate
{
  gint width, height;

  GtkSelectionMode selection_mode;

  GdkWindow *bin_window;

  GList *children;

  GtkTreeModel *model;
  
  GList *items;
  
  GtkAdjustment *hadjustment;
  GtkAdjustment *vadjustment;

  guint layout_idle_id;
  
  gboolean doing_rubberband;
  gint rubberband_x1, rubberband_y1;
  gint rubberband_x2, rubberband_y2;

  guint scroll_timeout_id;
  gint scroll_value_diff;
  gint event_last_x, event_last_y;

  GtkIconViewItem *anchor_item;
  GtkIconViewItem *cursor_item;
  GtkIconViewItem *edited_item;
  GtkCellEditable *editable;

  GtkIconViewItem *last_single_clicked;

  GList *cell_list;
  guint n_cells;

  gint cursor_cell;

  GtkOrientation item_orientation;

  gint columns;
  gint item_width;
  gint spacing;
  gint row_spacing;
  gint column_spacing;
  gint margin;
  gint item_padding;

  gint text_column;
  gint markup_column;
  gint pixbuf_column;

  gint pixbuf_cell;
  gint text_cell;

  gint tooltip_column;

  /* Drag-and-drop. */
  GdkModifierType start_button_mask;
  gint pressed_button;
  gint press_start_x;
  gint press_start_y;

  GdkDragAction source_actions;
  GdkDragAction dest_actions;

  GtkTreeRowReference *dest_item;
  GtkIconViewDropPosition dest_pos;

  /* scroll to */
  GtkTreeRowReference *scroll_to_path;
  gfloat scroll_to_row_align;
  gfloat scroll_to_col_align;
  guint scroll_to_use_align : 1;

  guint source_set : 1;
  guint dest_set : 1;
  guint reorderable : 1;
  guint empty_view_drop :1;

  guint modify_selection_pressed : 1;
  guint extend_selection_pressed : 1;

  guint draw_focus : 1;
};

/* Signals */
enum
{
  ITEM_ACTIVATED,
  SELECTION_CHANGED,
  SELECT_ALL,
  UNSELECT_ALL,
  SELECT_CURSOR_ITEM,
  TOGGLE_CURSOR_ITEM,
  MOVE_CURSOR,
  ACTIVATE_CURSOR_ITEM,
  LAST_SIGNAL
};

/* Properties */
enum
{
  PROP_0,
  PROP_PIXBUF_COLUMN,
  PROP_TEXT_COLUMN,
  PROP_MARKUP_COLUMN,  
  PROP_SELECTION_MODE,
  PROP_ORIENTATION,
  PROP_ITEM_ORIENTATION,
  PROP_MODEL,
  PROP_COLUMNS,
  PROP_ITEM_WIDTH,
  PROP_SPACING,
  PROP_ROW_SPACING,
  PROP_COLUMN_SPACING,
  PROP_MARGIN,
  PROP_REORDERABLE,
  PROP_TOOLTIP_COLUMN,
  PROP_ITEM_PADDING
};

/* GObject vfuncs */
static void             gtk_icon_view_cell_layout_init          (GtkCellLayoutIface *iface);
static void             gtk_icon_view_finalize                  (GObject            *object);
static void             gtk_icon_view_set_property              (GObject            *object,
								 guint               prop_id,
								 const GValue       *value,
								 GParamSpec         *pspec);
static void             gtk_icon_view_get_property              (GObject            *object,
								 guint               prop_id,
								 GValue             *value,
								 GParamSpec         *pspec);

/* GtkObject vfuncs */
static void             gtk_icon_view_destroy                   (GtkObject          *object);

/* GtkWidget vfuncs */
static void             gtk_icon_view_realize                   (GtkWidget          *widget);
static void             gtk_icon_view_unrealize                 (GtkWidget          *widget);
static void             gtk_icon_view_style_set                 (GtkWidget        *widget,
						                 GtkStyle         *previous_style);
static void             gtk_icon_view_state_changed             (GtkWidget        *widget,
			                                         GtkStateType      previous_state);
static void             gtk_icon_view_size_request              (GtkWidget          *widget,
								 GtkRequisition     *requisition);
static void             gtk_icon_view_size_allocate             (GtkWidget          *widget,
								 GtkAllocation      *allocation);
static gboolean         gtk_icon_view_expose                    (GtkWidget          *widget,
								 GdkEventExpose     *expose);
static gboolean         gtk_icon_view_motion                    (GtkWidget          *widget,
								 GdkEventMotion     *event);
static gboolean         gtk_icon_view_button_press              (GtkWidget          *widget,
								 GdkEventButton     *event);
static gboolean         gtk_icon_view_button_release            (GtkWidget          *widget,
								 GdkEventButton     *event);
static gboolean         gtk_icon_view_key_press                 (GtkWidget          *widget,
								 GdkEventKey        *event);
static gboolean         gtk_icon_view_key_release               (GtkWidget          *widget,
								 GdkEventKey        *event);

/* GtkContainer vfuncs */
static void             gtk_icon_view_remove                    (GtkContainer       *container,
								 GtkWidget          *widget);
static void             gtk_icon_view_forall                    (GtkContainer       *container,
								 gboolean            include_internals,
								 GtkCallback         callback,
								 gpointer            callback_data);

/* GtkIconView vfuncs */
static void             gtk_icon_view_set_adjustments           (GtkIconView        *icon_view,
								 GtkAdjustment      *hadj,
								 GtkAdjustment      *vadj);
static void             gtk_icon_view_real_select_all           (GtkIconView        *icon_view);
static void             gtk_icon_view_real_unselect_all         (GtkIconView        *icon_view);
static void             gtk_icon_view_real_select_cursor_item   (GtkIconView        *icon_view);
static void             gtk_icon_view_real_toggle_cursor_item   (GtkIconView        *icon_view);
static gboolean         gtk_icon_view_real_activate_cursor_item (GtkIconView        *icon_view);

 /* Internal functions */
static void                 gtk_icon_view_adjustment_changed             (GtkAdjustment          *adjustment,
									  GtkIconView            *icon_view);
static void                 gtk_icon_view_layout                         (GtkIconView            *icon_view);
static void                 gtk_icon_view_paint_item                     (GtkIconView            *icon_view,
									  cairo_t *cr,

									  GtkIconViewItem        *item,
									  GdkRectangle           *area,
									  GdkDrawable *drawable,
									  gint         x,
									  gint         y,
									  gboolean     draw_focus);
static void                 gtk_icon_view_paint_rubberband               (GtkIconView            *icon_view,
								          cairo_t *cr,
									  GdkRectangle           *area);
static void                 gtk_icon_view_queue_draw_path                (GtkIconView *icon_view,
									  GtkTreePath *path);
static void                 gtk_icon_view_queue_draw_item                (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static void                 gtk_icon_view_queue_layout                   (GtkIconView            *icon_view);
static void                 gtk_icon_view_set_cursor_item                (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  gint                    cursor_cell);
static void                 gtk_icon_view_start_rubberbanding            (GtkIconView            *icon_view,
									  gint                    x,
									  gint                    y);
static void                 gtk_icon_view_stop_rubberbanding             (GtkIconView            *icon_view);
static void                 gtk_icon_view_update_rubberband_selection    (GtkIconView            *icon_view);
static gboolean             gtk_icon_view_item_hit_test                  (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  gint                    x,
									  gint                    y,
									  gint                    width,
									  gint                    height);
static gboolean             gtk_icon_view_unselect_all_internal          (GtkIconView            *icon_view);
static void                 gtk_icon_view_calculate_item_size            (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static void                 gtk_icon_view_calculate_item_size2           (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  gint                   *max_height);
static void                 gtk_icon_view_update_rubberband              (gpointer                data);
static void                 gtk_icon_view_item_invalidate_size           (GtkIconViewItem        *item);
static void                 gtk_icon_view_invalidate_sizes               (GtkIconView            *icon_view);
static void                 gtk_icon_view_add_move_binding               (GtkBindingSet          *binding_set,
									  guint                   keyval,
									  guint                   modmask,
									  GtkMovementStep         step,
									  gint                    count);
static gboolean             gtk_icon_view_real_move_cursor               (GtkIconView            *icon_view,
									  GtkMovementStep         step,
									  gint                    count);
static void                 gtk_icon_view_move_cursor_up_down            (GtkIconView            *icon_view,
									  gint                    count);
static void                 gtk_icon_view_move_cursor_page_up_down       (GtkIconView            *icon_view,
									  gint                    count);
static void                 gtk_icon_view_move_cursor_left_right         (GtkIconView            *icon_view,
									  gint                    count);
static void                 gtk_icon_view_move_cursor_start_end          (GtkIconView            *icon_view,
									  gint                    count);
static void                 gtk_icon_view_scroll_to_item                 (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static void                 gtk_icon_view_select_item                    (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static void                 gtk_icon_view_unselect_item                  (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static gboolean             gtk_icon_view_select_all_between             (GtkIconView            *icon_view,
									  GtkIconViewItem        *anchor,
									  GtkIconViewItem        *cursor);
static GtkIconViewItem *    gtk_icon_view_get_item_at_coords             (GtkIconView            *icon_view,
									  gint                    x,
									  gint                    y,
									  gboolean                only_in_cell,
									  GtkIconViewCellInfo   **cell_at_pos);
static void                 gtk_icon_view_get_cell_area                  (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  GtkIconViewCellInfo    *cell_info,
									  GdkRectangle           *cell_area);
static void                 gtk_icon_view_get_cell_box                   (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  GtkIconViewCellInfo    *info,
									  GdkRectangle           *box);
static GtkIconViewCellInfo *gtk_icon_view_get_cell_info                  (GtkIconView            *icon_view,
									  GtkCellRenderer        *renderer);
static void                 gtk_icon_view_set_cell_data                  (GtkIconView            *icon_view,
									  GtkIconViewItem        *item);
static void                 gtk_icon_view_cell_layout_pack_start         (GtkCellLayout          *layout,
									  GtkCellRenderer        *renderer,
									  gboolean                expand);
static void                 gtk_icon_view_cell_layout_pack_end           (GtkCellLayout          *layout,
									  GtkCellRenderer        *renderer,
									  gboolean                expand);
static void                 gtk_icon_view_cell_layout_add_attribute      (GtkCellLayout          *layout,
									  GtkCellRenderer        *renderer,
									  const gchar            *attribute,
									  gint                    column);
static void                 gtk_icon_view_cell_layout_clear              (GtkCellLayout          *layout);
static void                 gtk_icon_view_cell_layout_clear_attributes   (GtkCellLayout          *layout,
									  GtkCellRenderer        *renderer);
static void                 gtk_icon_view_cell_layout_set_cell_data_func (GtkCellLayout          *layout,
									  GtkCellRenderer        *cell,
									  GtkCellLayoutDataFunc   func,
									  gpointer                func_data,
									  GDestroyNotify          destroy);
static void                 gtk_icon_view_cell_layout_reorder            (GtkCellLayout          *layout,
									  GtkCellRenderer        *cell,
									  gint                    position);
static GList *              gtk_icon_view_cell_layout_get_cells          (GtkCellLayout          *layout);

static void                 gtk_icon_view_item_activate_cell             (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  GtkIconViewCellInfo    *cell_info,
									  GdkEvent               *event);
static void                 gtk_icon_view_item_selected_changed          (GtkIconView            *icon_view,
		                                                          GtkIconViewItem        *item);
static void                 gtk_icon_view_put                            (GtkIconView            *icon_view,
									  GtkWidget              *widget,
									  GtkIconViewItem        *item,
									  gint                    cell);
static void                 gtk_icon_view_remove_widget                  (GtkCellEditable        *editable,
									  GtkIconView            *icon_view);
static void                 gtk_icon_view_start_editing                  (GtkIconView            *icon_view,
									  GtkIconViewItem        *item,
									  GtkIconViewCellInfo    *cell_info,
									  GdkEvent               *event);
static void                 gtk_icon_view_stop_editing                   (GtkIconView            *icon_view,
									  gboolean                cancel_editing);

/* Source side drag signals */
static void gtk_icon_view_drag_begin       (GtkWidget        *widget,
                                            GdkDragContext   *context);
static void gtk_icon_view_drag_end         (GtkWidget        *widget,
                                            GdkDragContext   *context);
static void gtk_icon_view_drag_data_get    (GtkWidget        *widget,
                                            GdkDragContext   *context,
                                            GtkSelectionData *selection_data,
                                            guint             info,
                                            guint             time);
static void gtk_icon_view_drag_data_delete (GtkWidget        *widget,
                                            GdkDragContext   *context);

/* Target side drag signals */
static void     gtk_icon_view_drag_leave         (GtkWidget        *widget,
                                                  GdkDragContext   *context,
                                                  guint             time);
static gboolean gtk_icon_view_drag_motion        (GtkWidget        *widget,
                                                  GdkDragContext   *context,
                                                  gint              x,
                                                  gint              y,
                                                  guint             time);
static gboolean gtk_icon_view_drag_drop          (GtkWidget        *widget,
                                                  GdkDragContext   *context,
                                                  gint              x,
                                                  gint              y,
                                                  guint             time);
static void     gtk_icon_view_drag_data_received (GtkWidget        *widget,
                                                  GdkDragContext   *context,
                                                  gint              x,
                                                  gint              y,
                                                  GtkSelectionData *selection_data,
                                                  guint             info,
                                                  guint             time);
static gboolean gtk_icon_view_maybe_begin_drag   (GtkIconView             *icon_view,
					   	  GdkEventMotion          *event);

static void     remove_scroll_timeout            (GtkIconView *icon_view);

static void     adjust_wrap_width                (GtkIconView     *icon_view,
						  GtkIconViewItem *item);

/* GtkBuildable */
static GtkBuildableIface *parent_buildable_iface;
static void     gtk_icon_view_buildable_init             (GtkBuildableIface *iface);
static gboolean gtk_icon_view_buildable_custom_tag_start (GtkBuildable  *buildable,
							  GtkBuilder    *builder,
							  GObject       *child,
							  const gchar   *tagname,
							  GMarkupParser *parser,
							  gpointer      *data);
static void     gtk_icon_view_buildable_custom_tag_end   (GtkBuildable  *buildable,
							  GtkBuilder    *builder,
							  GObject       *child,
							  const gchar   *tagname,
							  gpointer      *data);

static guint icon_view_signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE_WITH_CODE (GtkIconView, gtk_icon_view, GTK_TYPE_CONTAINER,
			 G_ADD_PRIVATE (GtkIconView)
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_CELL_LAYOUT,
						gtk_icon_view_cell_layout_init)
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_BUILDABLE,
						gtk_icon_view_buildable_init))

static void
gtk_icon_view_class_init (GtkIconViewClass *klass)
{
  GObjectClass *gobject_class;
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;
  GtkContainerClass *container_class;
  GtkBindingSet *binding_set;
  
  binding_set = gtk_binding_set_by_class (klass);

  gobject_class = (GObjectClass *) klass;
  object_class = (GtkObjectClass *) klass;
  widget_class = (GtkWidgetClass *) klass;
  container_class = (GtkContainerClass *) klass;

  gobject_class->finalize = gtk_icon_view_finalize;
  gobject_class->set_property = gtk_icon_view_set_property;
  gobject_class->get_property = gtk_icon_view_get_property;

  object_class->destroy = gtk_icon_view_destroy;
  
  widget_class->realize = gtk_icon_view_realize;
  widget_class->unrealize = gtk_icon_view_unrealize;
  widget_class->style_set = gtk_icon_view_style_set;
  widget_class->get_accessible = NULL;
  widget_class->size_request = gtk_icon_view_size_request;
  widget_class->size_allocate = gtk_icon_view_size_allocate;
  widget_class->expose_event = gtk_icon_view_expose;
  widget_class->motion_notify_event = gtk_icon_view_motion;
  widget_class->button_press_event = gtk_icon_view_button_press;
  widget_class->button_release_event = gtk_icon_view_button_release;
  widget_class->key_press_event = gtk_icon_view_key_press;
  widget_class->key_release_event = gtk_icon_view_key_release;
  widget_class->drag_begin = gtk_icon_view_drag_begin;
  widget_class->drag_end = gtk_icon_view_drag_end;
  widget_class->drag_data_get = gtk_icon_view_drag_data_get;
  widget_class->drag_data_delete = gtk_icon_view_drag_data_delete;
  widget_class->drag_leave = gtk_icon_view_drag_leave;
  widget_class->drag_motion = gtk_icon_view_drag_motion;
  widget_class->drag_drop = gtk_icon_view_drag_drop;
  widget_class->drag_data_received = gtk_icon_view_drag_data_received;
  widget_class->state_changed = gtk_icon_view_state_changed;

  container_class->remove = gtk_icon_view_remove;
  container_class->forall = gtk_icon_view_forall;

  klass->set_scroll_adjustments = gtk_icon_view_set_adjustments;
  klass->select_all = gtk_icon_view_real_select_all;
  klass->unselect_all = gtk_icon_view_real_unselect_all;
  klass->select_cursor_item = gtk_icon_view_real_select_cursor_item;
  klass->toggle_cursor_item = gtk_icon_view_real_toggle_cursor_item;
  klass->activate_cursor_item = gtk_icon_view_real_activate_cursor_item;  
  klass->move_cursor = gtk_icon_view_real_move_cursor;
  
  /* Properties */
  /**
   * GtkIconView:selection-mode:
   * 
   * The ::selection-mode property specifies the selection mode of
   * icon view. If the mode is #GTK_SELECTION_MULTIPLE, rubberband selection
   * is enabled, for the other modes, only keyboard selection is possible.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_SELECTION_MODE,
				   g_param_spec_enum ("selection-mode",
						      P_("Selection mode"),
						      P_("The selection mode"),
						      GTK_TYPE_SELECTION_MODE,
						      GTK_SELECTION_SINGLE,
						      GTK_PARAM_READWRITE));

  /**
   * GtkIconView:pixbuf-column:
   *
   * The ::pixbuf-column property contains the number of the model column
   * containing the pixbufs which are displayed. The pixbuf column must be 
   * of type #GDK_TYPE_PIXBUF. Setting this property to -1 turns off the
   * display of pixbufs.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_PIXBUF_COLUMN,
				   g_param_spec_int ("pixbuf-column",
						     P_("Pixbuf column"),
						     P_("Model column used to retrieve the icon pixbuf from"),
						     -1, G_MAXINT, -1,
						     GTK_PARAM_READWRITE));

  /**
   * GtkIconView:text-column:
   *
   * The ::text-column property contains the number of the model column
   * containing the texts which are displayed. The text column must be 
   * of type #G_TYPE_STRING. If this property and the :markup-column 
   * property are both set to -1, no texts are displayed.   
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_TEXT_COLUMN,
				   g_param_spec_int ("text-column",
						     P_("Text column"),
						     P_("Model column used to retrieve the text from"),
						     -1, G_MAXINT, -1,
						     GTK_PARAM_READWRITE));

  
  /**
   * GtkIconView:markup-column:
   *
   * The ::markup-column property contains the number of the model column
   * containing markup information to be displayed. The markup column must be 
   * of type #G_TYPE_STRING. If this property and the :text-column property 
   * are both set to column numbers, it overrides the text column.
   * If both are set to -1, no texts are displayed.   
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_MARKUP_COLUMN,
				   g_param_spec_int ("markup-column",
						     P_("Markup column"),
						     P_("Model column used to retrieve the text if using Pango markup"),
						     -1, G_MAXINT, -1,
						     GTK_PARAM_READWRITE));
  
  g_object_class_install_property (gobject_class,
                                   PROP_MODEL,
                                   g_param_spec_object ("model",
							P_("Icon View Model"),
							P_("The model for the icon view"),
							GTK_TYPE_TREE_MODEL,
							GTK_PARAM_READWRITE));
  
  /**
   * GtkIconView:columns:
   *
   * The columns property contains the number of the columns in which the
   * items should be displayed. If it is -1, the number of columns will
   * be chosen automatically to fill the available area.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_COLUMNS,
				   g_param_spec_int ("columns",
						     P_("Number of columns"),
						     P_("Number of columns to display"),
						     -1, G_MAXINT, -1,
						     GTK_PARAM_READWRITE));
  

  /**
   * GtkIconView:item-width:
   *
   * The item-width property specifies the width to use for each item. 
   * If it is set to -1, the icon view will automatically determine a 
   * suitable item size.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
				   PROP_ITEM_WIDTH,
				   g_param_spec_int ("item-width",
						     P_("Width for each item"),
						     P_("The width used for each item"),
						     -1, G_MAXINT, -1,
						     GTK_PARAM_READWRITE));  

  /**
   * GtkIconView:spacing:
   *
   * The spacing property specifies the space which is inserted between
   * the cells (i.e. the icon and the text) of an item.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_SPACING,
                                   g_param_spec_int ("spacing",
						     P_("Spacing"),
						     P_("Space which is inserted between cells of an item"),
						     0, G_MAXINT, 0,
						     GTK_PARAM_READWRITE));

  /**
   * GtkIconView:row-spacing:
   *
   * The row-spacing property specifies the space which is inserted between
   * the rows of the icon view.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ROW_SPACING,
                                   g_param_spec_int ("row-spacing",
						     P_("Row Spacing"),
						     P_("Space which is inserted between grid rows"),
						     0, G_MAXINT, 6,
						     GTK_PARAM_READWRITE));

  /**
   * GtkIconView:column-spacing:
   *
   * The column-spacing property specifies the space which is inserted between
   * the columns of the icon view.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_COLUMN_SPACING,
                                   g_param_spec_int ("column-spacing",
						     P_("Column Spacing"),
						     P_("Space which is inserted between grid columns"),
						     0, G_MAXINT, 6,
						     GTK_PARAM_READWRITE));

  /**
   * GtkIconView:margin:
   *
   * The margin property specifies the space which is inserted 
   * at the edges of the icon view.
   *
   * Since: 2.6
   */
  g_object_class_install_property (gobject_class,
                                   PROP_MARGIN,
                                   g_param_spec_int ("margin",
						     P_("Margin"),
						     P_("Space which is inserted at the edges of the icon view"),
						     0, G_MAXINT, 6,
						     GTK_PARAM_READWRITE));

  /**
   * GtkIconView:orientation:
   *
   * The orientation property specifies how the cells (i.e. the icon and 
   * the text) of the item are positioned relative to each other.
   *
   * Since: 2.6
   *
   * Deprecated: 2.22: Use the #GtkIconView::item-orientation property
   */
  g_object_class_install_property (gobject_class,
				   PROP_ORIENTATION,
				   g_param_spec_enum ("orientation",
						      P_("Orientation"),
						      P_("How the text and icon of each item are positioned relative to each other"),
						      GTK_TYPE_ORIENTATION,
						      GTK_ORIENTATION_VERTICAL,
						      GTK_PARAM_READWRITE | G_PARAM_DEPRECATED));

  /**
   * GtkIconView:item-orientation:
   *
   * The item-orientation property specifies how the cells (i.e. the icon and
   * the text) of the item are positioned relative to each other.
   *
   * Since: 2.22
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ITEM_ORIENTATION,
                                   g_param_spec_enum ("item-orientation",
                                                      P_("Item Orientation"),
                                                      P_("How the text and icon of each item are positioned relative to each other"),
                                                      GTK_TYPE_ORIENTATION,
                                                      GTK_ORIENTATION_VERTICAL,
                                                      GTK_PARAM_READWRITE));

  /**
   * GtkIconView:reorderable:
   *
   * The reorderable property specifies if the items can be reordered
   * by DND.
   *
   * Since: 2.8
   */
  g_object_class_install_property (gobject_class,
                                   PROP_REORDERABLE,
                                   g_param_spec_boolean ("reorderable",
							 P_("Reorderable"),
							 P_("View is reorderable"),
							 FALSE,
							 G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class,
                                     PROP_TOOLTIP_COLUMN,
                                     g_param_spec_int ("tooltip-column",
                                                       P_("Tooltip Column"),
                                                       P_("The column in the model containing the tooltip texts for the items"),
                                                       -1,
                                                       G_MAXINT,
                                                       -1,
                                                       GTK_PARAM_READWRITE));

  /**
   * GtkIconView:item-padding:
   *
   * The item-padding property specifies the padding around each
   * of the icon view's item.
   *
   * Since: 2.18
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ITEM_PADDING,
                                   g_param_spec_int ("item-padding",
						     P_("Item Padding"),
						     P_("Padding around icon view items"),
						     0, G_MAXINT, 6,
						     GTK_PARAM_READWRITE));



  /* Style properties */
  gtk_widget_class_install_style_property (widget_class,
                                           g_param_spec_boxed ("selection-box-color",
                                                               P_("Selection Box Color"),
                                                               P_("Color of the selection box"),
                                                               GDK_TYPE_COLOR,
                                                               GTK_PARAM_READABLE));

  gtk_widget_class_install_style_property (widget_class,
                                           g_param_spec_uchar ("selection-box-alpha",
                                                               P_("Selection Box Alpha"),
                                                               P_("Opacity of the selection box"),
                                                               0, 0xff,
                                                               0x40,
                                                               GTK_PARAM_READABLE));

  /* Signals */
  /**
   * GtkIconView::set-scroll-adjustments
   * @horizontal: the horizontal #GtkAdjustment
   * @vertical: the vertical #GtkAdjustment
   *
   * Set the scroll adjustments for the icon view. Usually scrolled containers
   * like #GtkScrolledWindow will emit this signal to connect two instances
   * of #GtkScrollbar to the scroll directions of the #GtkIconView.
   */
  widget_class->set_scroll_adjustments_signal =
    g_signal_new (I_("set-scroll-adjustments"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtkIconViewClass, set_scroll_adjustments),
		  NULL, NULL, 
		  NULL,
		  G_TYPE_NONE, 2,
		  GTK_TYPE_ADJUSTMENT, GTK_TYPE_ADJUSTMENT);

  /**
   * GtkIconView::item-activated:
   * @iconview: the object on which the signal is emitted
   * @path: the #GtkTreePath for the activated item
   *
   * The ::item-activated signal is emitted when the method
   * gtk_icon_view_item_activated() is called or the user double 
   * clicks an item. It is also emitted when a non-editable item
   * is selected and one of the keys: Space, Return or Enter is
   * pressed.
   */
  icon_view_signals[ITEM_ACTIVATED] =
    g_signal_new (I_("item-activated"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (GtkIconViewClass, item_activated),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__BOXED,
		  G_TYPE_NONE, 1,
		  GTK_TYPE_TREE_PATH);

  /**
   * GtkIconView::selection-changed:
   * @iconview: the object on which the signal is emitted
   *
   * The ::selection-changed signal is emitted when the selection
   * (i.e. the set of selected items) changes.
   */
  icon_view_signals[SELECTION_CHANGED] =
    g_signal_new (I_("selection-changed"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_FIRST,
		  G_STRUCT_OFFSET (GtkIconViewClass, selection_changed),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  
  /**
   * GtkIconView::select-all:
   * @iconview: the object on which the signal is emitted
   *
   * A <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user selects all items.
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control selection
   * programmatically.
   * 
   * The default binding for this signal is Ctrl-a.
   */
  icon_view_signals[SELECT_ALL] =
    g_signal_new (I_("select-all"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, select_all),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);
  
  /**
   * GtkIconView::unselect-all:
   * @iconview: the object on which the signal is emitted
   *
   * A <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user unselects all items.
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control selection
   * programmatically.
   * 
   * The default binding for this signal is Ctrl-Shift-a. 
   */
  icon_view_signals[UNSELECT_ALL] =
    g_signal_new (I_("unselect-all"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, unselect_all),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * GtkIconView::select-cursor-item:
   * @iconview: the object on which the signal is emitted
   *
   * A <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user selects the item that is currently
   * focused.
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control selection
   * programmatically.
   * 
   * There is no default binding for this signal.
   */
  icon_view_signals[SELECT_CURSOR_ITEM] =
    g_signal_new (I_("select-cursor-item"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, select_cursor_item),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * GtkIconView::toggle-cursor-item:
   * @iconview: the object on which the signal is emitted
   *
   * A <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user toggles whether the currently
   * focused item is selected or not. The exact effect of this 
   * depend on the selection mode.
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control selection
   * programmatically.
   * 
   * There is no default binding for this signal is Ctrl-Space.
   */
  icon_view_signals[TOGGLE_CURSOR_ITEM] =
    g_signal_new (I_("toggle-cursor-item"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, toggle_cursor_item),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__VOID,
		  G_TYPE_NONE, 0);

  /**
   * GtkIconView::activate-cursor-item:
   * @iconview: the object on which the signal is emitted
   *
   * A <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user activates the currently 
   * focused item. 
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control activation
   * programmatically.
   * 
   * The default bindings for this signal are Space, Return and Enter.
   */
  icon_view_signals[ACTIVATE_CURSOR_ITEM] =
    g_signal_new (I_("activate-cursor-item"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, activate_cursor_item),
		  NULL, NULL,
		  NULL,
		  G_TYPE_BOOLEAN, 0);
  
  /**
   * GtkIconView::move-cursor:
   * @iconview: the object which received the signal
   * @step: the granularity of the move, as a #GtkMovementStep
   * @count: the number of @step units to move
   *
   * The ::move-cursor signal is a
   * <link linkend="keybinding-signals">keybinding signal</link>
   * which gets emitted when the user initiates a cursor movement.
   *
   * Applications should not connect to it, but may emit it with
   * g_signal_emit_by_name() if they need to control the cursor
   * programmatically.
   *
   * The default bindings for this signal include
   * <itemizedlist>
   * <listitem>Arrow keys which move by individual steps</listitem>
   * <listitem>Home/End keys which move to the first/last item</listitem>
   * <listitem>PageUp/PageDown which move by "pages"</listitem>
   * </itemizedlist>
   *
   * All of these will extend the selection when combined with
   * the Shift modifier.
   */
  icon_view_signals[MOVE_CURSOR] =
    g_signal_new (I_("move-cursor"),
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
		  G_STRUCT_OFFSET (GtkIconViewClass, move_cursor),
		  NULL, NULL,
		  NULL,
		  G_TYPE_BOOLEAN, 2,
		  GTK_TYPE_MOVEMENT_STEP,
		  G_TYPE_INT);

  /* Key bindings */
  gtk_binding_entry_add_signal (binding_set, GDK_a, GDK_CONTROL_MASK, 
				"select-all", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_a, GDK_CONTROL_MASK | GDK_SHIFT_MASK, 
				"unselect-all", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_space, GDK_CONTROL_MASK, 
				"toggle-cursor-item", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Space, GDK_CONTROL_MASK,
				"toggle-cursor-item", 0);

  gtk_binding_entry_add_signal (binding_set, GDK_space, 0, 
				"activate-cursor-item", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Space, 0,
				"activate-cursor-item", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_Return, 0, 
				"activate-cursor-item", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_ISO_Enter, 0, 
				"activate-cursor-item", 0);
  gtk_binding_entry_add_signal (binding_set, GDK_KP_Enter, 0, 
				"activate-cursor-item", 0);

  gtk_icon_view_add_move_binding (binding_set, GDK_Up, 0,
				  GTK_MOVEMENT_DISPLAY_LINES, -1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Up, 0,
				  GTK_MOVEMENT_DISPLAY_LINES, -1);

  gtk_icon_view_add_move_binding (binding_set, GDK_Down, 0,
				  GTK_MOVEMENT_DISPLAY_LINES, 1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Down, 0,
				  GTK_MOVEMENT_DISPLAY_LINES, 1);

  gtk_icon_view_add_move_binding (binding_set, GDK_p, GDK_CONTROL_MASK,
				  GTK_MOVEMENT_DISPLAY_LINES, -1);

  gtk_icon_view_add_move_binding (binding_set, GDK_n, GDK_CONTROL_MASK,
				  GTK_MOVEMENT_DISPLAY_LINES, 1);

  gtk_icon_view_add_move_binding (binding_set, GDK_Home, 0,
				  GTK_MOVEMENT_BUFFER_ENDS, -1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Home, 0,
				  GTK_MOVEMENT_BUFFER_ENDS, -1);

  gtk_icon_view_add_move_binding (binding_set, GDK_End, 0,
				  GTK_MOVEMENT_BUFFER_ENDS, 1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_End, 0,
				  GTK_MOVEMENT_BUFFER_ENDS, 1);

  gtk_icon_view_add_move_binding (binding_set, GDK_Page_Up, 0,
				  GTK_MOVEMENT_PAGES, -1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Page_Up, 0,
				  GTK_MOVEMENT_PAGES, -1);

  gtk_icon_view_add_move_binding (binding_set, GDK_Page_Down, 0,
				  GTK_MOVEMENT_PAGES, 1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Page_Down, 0,
				  GTK_MOVEMENT_PAGES, 1);

  gtk_icon_view_add_move_binding (binding_set, GDK_Right, 0, 
				  GTK_MOVEMENT_VISUAL_POSITIONS, 1);
  gtk_icon_view_add_move_binding (binding_set, GDK_Left, 0, 
				  GTK_MOVEMENT_VISUAL_POSITIONS, -1);

  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Right, 0, 
				  GTK_MOVEMENT_VISUAL_POSITIONS, 1);
  gtk_icon_view_add_move_binding (binding_set, GDK_KP_Left, 0, 
				  GTK_MOVEMENT_VISUAL_POSITIONS, -1);
}

static void
gtk_icon_view_buildable_init (GtkBuildableIface *iface)
{
  parent_buildable_iface = g_type_interface_peek_parent (iface);
  iface->add_child = _gtk_cell_layout_buildable_add_child;
  iface->custom_tag_start = gtk_icon_view_buildable_custom_tag_start;
  iface->custom_tag_end = gtk_icon_view_buildable_custom_tag_end;
}

static void
gtk_icon_view_cell_layout_init (GtkCellLayoutIface *iface)
{
  iface->pack_start = gtk_icon_view_cell_layout_pack_start;
  iface->pack_end = gtk_icon_view_cell_layout_pack_end;
  iface->clear = gtk_icon_view_cell_layout_clear;
  iface->add_attribute = gtk_icon_view_cell_layout_add_attribute;
  iface->set_cell_data_func = gtk_icon_view_cell_layout_set_cell_data_func;
  iface->clear_attributes = gtk_icon_view_cell_layout_clear_attributes;
  iface->reorder = gtk_icon_view_cell_layout_reorder;
  iface->get_cells = gtk_icon_view_cell_layout_get_cells;
}

static void
gtk_icon_view_init (GtkIconView *icon_view)
{
  icon_view->priv = GTK_ICON_VIEW_GET_PRIVATE (icon_view);
  
  icon_view->priv->width = 0;
  icon_view->priv->height = 0;
  icon_view->priv->selection_mode = GTK_SELECTION_SINGLE;
  icon_view->priv->pressed_button = -1;
  icon_view->priv->press_start_x = -1;
  icon_view->priv->press_start_y = -1;
  icon_view->priv->text_column = -1;
  icon_view->priv->markup_column = -1;  
  icon_view->priv->pixbuf_column = -1;
  icon_view->priv->text_cell = -1;
  icon_view->priv->pixbuf_cell = -1;  
  icon_view->priv->tooltip_column = -1;  

  gtk_widget_set_can_focus (GTK_WIDGET (icon_view), TRUE);
  
  gtk_icon_view_set_adjustments (icon_view, NULL, NULL);

  icon_view->priv->cell_list = NULL;
  icon_view->priv->n_cells = 0;
  icon_view->priv->cursor_cell = -1;

  icon_view->priv->item_orientation = GTK_ORIENTATION_VERTICAL;

  icon_view->priv->columns = -1;
  icon_view->priv->item_width = -1;
  icon_view->priv->spacing = 0;
  icon_view->priv->row_spacing = 6;
  icon_view->priv->column_spacing = 6;
  icon_view->priv->margin = 6;
  icon_view->priv->item_padding = 6;

  icon_view->priv->draw_focus = TRUE;
}

static void
gtk_icon_view_destroy (GtkObject *object)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (object);
  
  gtk_icon_view_stop_editing (icon_view, TRUE);

  gtk_icon_view_set_model (icon_view, NULL);
  
  if (icon_view->priv->layout_idle_id != 0)
    {
      g_source_remove (icon_view->priv->layout_idle_id);
      icon_view->priv->layout_idle_id = 0;
    }

  if (icon_view->priv->scroll_to_path != NULL)
    {
      gtk_tree_row_reference_free (icon_view->priv->scroll_to_path);
      icon_view->priv->scroll_to_path = NULL;
    }

  remove_scroll_timeout (icon_view);

  if (icon_view->priv->hadjustment != NULL)
    {
      g_object_unref (icon_view->priv->hadjustment);
      icon_view->priv->hadjustment = NULL;
    }

  if (icon_view->priv->vadjustment != NULL)
    {
      g_object_unref (icon_view->priv->vadjustment);
      icon_view->priv->vadjustment = NULL;
    }
  
  GTK_OBJECT_CLASS (gtk_icon_view_parent_class)->destroy (object);
}

/* GObject methods */
static void
gtk_icon_view_finalize (GObject *object)
{
  gtk_icon_view_cell_layout_clear (GTK_CELL_LAYOUT (object));

  G_OBJECT_CLASS (gtk_icon_view_parent_class)->finalize (object);
}


static void
gtk_icon_view_set_property (GObject      *object,
			    guint         prop_id,
			    const GValue *value,
			    GParamSpec   *pspec)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (object);

  switch (prop_id)
    {
    case PROP_SELECTION_MODE:
      gtk_icon_view_set_selection_mode (icon_view, g_value_get_enum (value));
      break;
    case PROP_PIXBUF_COLUMN:
      gtk_icon_view_set_pixbuf_column (icon_view, g_value_get_int (value));
      break;
    case PROP_TEXT_COLUMN:
      gtk_icon_view_set_text_column (icon_view, g_value_get_int (value));
      break;
    case PROP_MARKUP_COLUMN:
      gtk_icon_view_set_markup_column (icon_view, g_value_get_int (value));
      break;
    case PROP_MODEL:
      gtk_icon_view_set_model (icon_view, g_value_get_object (value));
      break;
    case PROP_ORIENTATION:
    case PROP_ITEM_ORIENTATION:
      gtk_icon_view_set_item_orientation (icon_view, g_value_get_enum (value));
      break;
    case PROP_COLUMNS:
      gtk_icon_view_set_columns (icon_view, g_value_get_int (value));
      break;
    case PROP_ITEM_WIDTH:
      gtk_icon_view_set_item_width (icon_view, g_value_get_int (value));
      break;
    case PROP_SPACING:
      gtk_icon_view_set_spacing (icon_view, g_value_get_int (value));
      break;
    case PROP_ROW_SPACING:
      gtk_icon_view_set_row_spacing (icon_view, g_value_get_int (value));
      break;
    case PROP_COLUMN_SPACING:
      gtk_icon_view_set_column_spacing (icon_view, g_value_get_int (value));
      break;
    case PROP_MARGIN:
      gtk_icon_view_set_margin (icon_view, g_value_get_int (value));
      break;
    case PROP_REORDERABLE:
      gtk_icon_view_set_reorderable (icon_view, g_value_get_boolean (value));
      break;
      
    case PROP_TOOLTIP_COLUMN:
      gtk_icon_view_set_tooltip_column (icon_view, g_value_get_int (value));
      break;

    case PROP_ITEM_PADDING:
      gtk_icon_view_set_item_padding (icon_view, g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
gtk_icon_view_get_property (GObject      *object,
			    guint         prop_id,
			    GValue       *value,
			    GParamSpec   *pspec)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (object);

  switch (prop_id)
    {
    case PROP_SELECTION_MODE:
      g_value_set_enum (value, icon_view->priv->selection_mode);
      break;
    case PROP_PIXBUF_COLUMN:
      g_value_set_int (value, icon_view->priv->pixbuf_column);
      break;
    case PROP_TEXT_COLUMN:
      g_value_set_int (value, icon_view->priv->text_column);
      break;
    case PROP_MARKUP_COLUMN:
      g_value_set_int (value, icon_view->priv->markup_column);
      break;
    case PROP_MODEL:
      g_value_set_object (value, icon_view->priv->model);
      break;
    case PROP_ORIENTATION:
    case PROP_ITEM_ORIENTATION:
      g_value_set_enum (value, icon_view->priv->item_orientation);
      break;
    case PROP_COLUMNS:
      g_value_set_int (value, icon_view->priv->columns);
      break;
    case PROP_ITEM_WIDTH:
      g_value_set_int (value, icon_view->priv->item_width);
      break;
    case PROP_SPACING:
      g_value_set_int (value, icon_view->priv->spacing);
      break;
    case PROP_ROW_SPACING:
      g_value_set_int (value, icon_view->priv->row_spacing);
      break;
    case PROP_COLUMN_SPACING:
      g_value_set_int (value, icon_view->priv->column_spacing);
      break;
    case PROP_MARGIN:
      g_value_set_int (value, icon_view->priv->margin);
      break;
    case PROP_REORDERABLE:
      g_value_set_boolean (value, icon_view->priv->reorderable);
      break;
    case PROP_TOOLTIP_COLUMN:
      g_value_set_int (value, icon_view->priv->tooltip_column);
      break;

    case PROP_ITEM_PADDING:
      g_value_set_int (value, icon_view->priv->item_padding);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

/* GtkWidget signals */
static void
gtk_icon_view_realize (GtkWidget *widget)
{
  GtkIconView *icon_view;
  GdkWindowAttr attributes;
  gint attributes_mask;
  
  icon_view = GTK_ICON_VIEW (widget);

  gtk_widget_set_realized (widget, TRUE);

  /* Make the main, clipping window */
  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = widget->allocation.width;
  attributes.height = widget->allocation.height;
  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.visual = gtk_widget_get_visual (widget);
  attributes.colormap = gtk_widget_get_colormap (widget);
  attributes.event_mask = GDK_VISIBILITY_NOTIFY_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

  widget->window = gdk_window_new (gtk_widget_get_parent_window (widget),
				   &attributes, attributes_mask);
  gdk_window_set_back_pixmap (widget->window, NULL, FALSE);
  gdk_window_set_user_data (widget->window, widget);

  /* Make the window for the icon view */
  attributes.x = 0;
  attributes.y = 0;
  attributes.width = MAX (icon_view->priv->width, widget->allocation.width);
  attributes.height = MAX (icon_view->priv->height, widget->allocation.height);
  attributes.event_mask = (GDK_EXPOSURE_MASK |
			   GDK_SCROLL_MASK |
			   GDK_POINTER_MOTION_MASK |
			   GDK_BUTTON_PRESS_MASK |
			   GDK_BUTTON_RELEASE_MASK |
			   GDK_KEY_PRESS_MASK |
			   GDK_KEY_RELEASE_MASK) |
    gtk_widget_get_events (widget);
  
  icon_view->priv->bin_window = gdk_window_new (widget->window,
						&attributes, attributes_mask);
  gdk_window_set_user_data (icon_view->priv->bin_window, widget);

  widget->style = gtk_style_attach (widget->style, widget->window);
  gdk_window_set_background (icon_view->priv->bin_window, &widget->style->base[widget->state]);

  gdk_window_show (icon_view->priv->bin_window);
}

static void
gtk_icon_view_unrealize (GtkWidget *widget)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (widget);

  gdk_window_set_user_data (icon_view->priv->bin_window, NULL);
  gdk_window_destroy (icon_view->priv->bin_window);
  icon_view->priv->bin_window = NULL;

  GTK_WIDGET_CLASS (gtk_icon_view_parent_class)->unrealize (widget);
}

static void
gtk_icon_view_state_changed (GtkWidget      *widget,
		 	     GtkStateType    previous_state)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  if (gtk_widget_get_realized (widget))
    {
      gdk_window_set_background (widget->window, &widget->style->base[widget->state]);
      gdk_window_set_background (icon_view->priv->bin_window, &widget->style->base[widget->state]);
    }

  gtk_widget_queue_draw (widget);
}

static void
gtk_icon_view_style_set (GtkWidget *widget,
			 GtkStyle *previous_style)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  if (gtk_widget_get_realized (widget))
    {
      gdk_window_set_background (widget->window, &widget->style->base[widget->state]);
      gdk_window_set_background (icon_view->priv->bin_window, &widget->style->base[widget->state]);
    }

  gtk_widget_queue_resize (widget);
}

static void
gtk_icon_view_size_request (GtkWidget      *widget,
			    GtkRequisition *requisition)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);
  GList *tmp_list;

  requisition->width = icon_view->priv->width;
  requisition->height = icon_view->priv->height;

  tmp_list = icon_view->priv->children;

  while (tmp_list)
    {
      GtkIconViewChild *child = tmp_list->data;
      GtkRequisition child_requisition;

      tmp_list = tmp_list->next;

      if (gtk_widget_get_visible (child->widget))
        gtk_widget_size_request (child->widget, &child_requisition);
    }  
}

static void
gtk_icon_view_allocate_children (GtkIconView *icon_view)
{
  GList *list;

  for (list = icon_view->priv->children; list; list = list->next)
    {
      GtkIconViewChild *child = list->data;
      GtkAllocation allocation;

      /* totally ignore our child's requisition */
      if (child->cell < 0)
	{
	  allocation.x = child->item->x + icon_view->priv->item_padding;
	  allocation.y = child->item->y + icon_view->priv->item_padding;
	  allocation.width = child->item->width - icon_view->priv->item_padding * 2;
	  allocation.height = child->item->height - icon_view->priv->item_padding * 2;
	}
      else
	{
	  GdkRectangle *box = &child->item->box[child->cell];

	  allocation.x = box->x;
	  allocation.y = box->y;
	  allocation.width = box->width;
	  allocation.height = box->height;
	}

      gtk_widget_size_allocate (child->widget, &allocation);
    }
}

static void
gtk_icon_view_size_allocate (GtkWidget      *widget,
			     GtkAllocation  *allocation)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  GtkAdjustment *hadjustment, *vadjustment;

  widget->allocation = *allocation;
  
  if (gtk_widget_get_realized (widget))
    {
      gdk_window_move_resize (widget->window,
			      allocation->x, allocation->y,
			      allocation->width, allocation->height);
      gdk_window_resize (icon_view->priv->bin_window,
			 MAX (icon_view->priv->width, allocation->width),
			 MAX (icon_view->priv->height, allocation->height));
    }

  gtk_icon_view_layout (icon_view);
  
  gtk_icon_view_allocate_children (icon_view);

  hadjustment = icon_view->priv->hadjustment;
  vadjustment = icon_view->priv->vadjustment;

  hadjustment->page_size = allocation->width;
  hadjustment->page_increment = allocation->width * 0.9;
  hadjustment->step_increment = allocation->width * 0.1;
  hadjustment->lower = 0;
  hadjustment->upper = MAX (allocation->width, icon_view->priv->width);

  if (hadjustment->value > hadjustment->upper - hadjustment->page_size)
    gtk_adjustment_set_value (hadjustment, MAX (0, hadjustment->upper - hadjustment->page_size));

  vadjustment->page_size = allocation->height;
  vadjustment->page_increment = allocation->height * 0.9;
  vadjustment->step_increment = allocation->height * 0.1;
  vadjustment->lower = 0;
  vadjustment->upper = MAX (allocation->height, icon_view->priv->height);

  if (vadjustment->value > vadjustment->upper - vadjustment->page_size)
    gtk_adjustment_set_value (vadjustment, MAX (0, vadjustment->upper - vadjustment->page_size));

  if (gtk_widget_get_realized (widget) &&
      icon_view->priv->scroll_to_path)
    {
      GtkTreePath *path;
      path = gtk_tree_row_reference_get_path (icon_view->priv->scroll_to_path);
      gtk_tree_row_reference_free (icon_view->priv->scroll_to_path);
      icon_view->priv->scroll_to_path = NULL;

      gtk_icon_view_scroll_to_path (icon_view, path,
				    icon_view->priv->scroll_to_use_align,
				    icon_view->priv->scroll_to_row_align,
				    icon_view->priv->scroll_to_col_align);
      gtk_tree_path_free (path);
    }
  else
    {
      gtk_adjustment_changed (hadjustment);
      gtk_adjustment_changed (vadjustment);
    }
}

static gboolean
gtk_icon_view_expose (GtkWidget *widget,
		      GdkEventExpose *expose)
{
  GtkIconView *icon_view;
  GList *icons;
  cairo_t *cr;
  GtkTreePath *path;
  gint dest_index;
  GtkIconViewDropPosition dest_pos;
  GtkIconViewItem *dest_item = NULL;

  icon_view = GTK_ICON_VIEW (widget);

  if (expose->window != icon_view->priv->bin_window)
    return FALSE;

  /* If a layout has been scheduled, do it now so that all
   * cell view items have valid sizes before we proceed. */
  if (icon_view->priv->layout_idle_id != 0)
    gtk_icon_view_layout (icon_view);

  cr = gdk_cairo_create (icon_view->priv->bin_window);
  cairo_set_line_width (cr, 1.);

  gtk_icon_view_get_drag_dest_item (icon_view, &path, &dest_pos);

  if (path)
    {
      dest_index = gtk_tree_path_get_indices (path)[0];
      gtk_tree_path_free (path);
    }
  else
    dest_index = -1;

  for (icons = icon_view->priv->items; icons; icons = icons->next) 
    {
      GtkIconViewItem *item = icons->data;
      GdkRectangle area;
      
      area.x = item->x;
      area.y = item->y;
      area.width = item->width;
      area.height = item->height;
	
      if (gdk_region_rect_in (expose->region, &area) == GDK_OVERLAP_RECTANGLE_OUT)
	continue;
      
      gtk_icon_view_paint_item (icon_view, cr, item, &expose->area, 
				icon_view->priv->bin_window,
				item->x, item->y,
				icon_view->priv->draw_focus); 
 
      if (dest_index == item->index)
	dest_item = item;
    }

  if (dest_item)
    {
      switch (dest_pos)
	{
	case GTK_ICON_VIEW_DROP_INTO:
	  gtk_paint_focus (widget->style,
			   icon_view->priv->bin_window,
			   gtk_widget_get_state (widget),
			   NULL,
			   widget,
			   "iconview-drop-indicator",
			   dest_item->x, dest_item->y,
			   dest_item->width, dest_item->height);
	  break;
	case GTK_ICON_VIEW_DROP_ABOVE:
	  gtk_paint_focus (widget->style,
			   icon_view->priv->bin_window,
			   gtk_widget_get_state (widget),
			   NULL,
			   widget,
			   "iconview-drop-indicator",
			   dest_item->x, dest_item->y - 1,
			   dest_item->width, 2);
	  break;
	case GTK_ICON_VIEW_DROP_LEFT:
	  gtk_paint_focus (widget->style,
			   icon_view->priv->bin_window,
			   gtk_widget_get_state (widget),
			   NULL,
			   widget,
			   "iconview-drop-indicator",
			   dest_item->x - 1, dest_item->y,
			   2, dest_item->height);
	  break;
	case GTK_ICON_VIEW_DROP_BELOW:
	  gtk_paint_focus (widget->style,
			   icon_view->priv->bin_window,
			   gtk_widget_get_state (widget),
			   NULL,
			   widget,
			   "iconview-drop-indicator",
			   dest_item->x, dest_item->y + dest_item->height - 1,
			   dest_item->width, 2);
	  break;
	case GTK_ICON_VIEW_DROP_RIGHT:
	  gtk_paint_focus (widget->style,
			   icon_view->priv->bin_window,
			   gtk_widget_get_state (widget),
			   NULL,
			   widget,
			   "iconview-drop-indicator",
			   dest_item->x + dest_item->width - 1, dest_item->y,
			   2, dest_item->height);
	case GTK_ICON_VIEW_NO_DROP: ;
	  break;
	}
    }
  
  if (icon_view->priv->doing_rubberband)
    {
      GdkRectangle *rectangles;
      gint n_rectangles;
      
      gdk_region_get_rectangles (expose->region,
				 &rectangles,
				 &n_rectangles);
      
      while (n_rectangles--)
	gtk_icon_view_paint_rubberband (icon_view, cr, &rectangles[n_rectangles]);

      g_free (rectangles);
    }

  cairo_destroy (cr);

  GTK_WIDGET_CLASS (gtk_icon_view_parent_class)->expose_event (widget, expose);

  return TRUE;
}

static gboolean
rubberband_scroll_timeout (gpointer data)
{
  GtkIconView *icon_view;
  gdouble value;

  icon_view = data;

  value = MIN (icon_view->priv->vadjustment->value +
	       icon_view->priv->scroll_value_diff,
	       icon_view->priv->vadjustment->upper -
	       icon_view->priv->vadjustment->page_size);

  gtk_adjustment_set_value (icon_view->priv->vadjustment, value);

  gtk_icon_view_update_rubberband (icon_view);
  
  return TRUE;
}

static gboolean
gtk_icon_view_motion (GtkWidget      *widget,
		      GdkEventMotion *event)
{
  GtkIconView *icon_view;
  gint abs_y;
  
  icon_view = GTK_ICON_VIEW (widget);

  gtk_icon_view_maybe_begin_drag (icon_view, event);

  if (icon_view->priv->doing_rubberband)
    {
      gtk_icon_view_update_rubberband (widget);
      
      abs_y = event->y - icon_view->priv->height *
	(icon_view->priv->vadjustment->value /
	 (icon_view->priv->vadjustment->upper -
	  icon_view->priv->vadjustment->lower));

      if (abs_y < 0 || abs_y > widget->allocation.height)
	{
	  if (abs_y < 0)
	    icon_view->priv->scroll_value_diff = abs_y;
	  else
	    icon_view->priv->scroll_value_diff = abs_y - widget->allocation.height;

	  icon_view->priv->event_last_x = event->x;
	  icon_view->priv->event_last_y = event->y;

	  if (icon_view->priv->scroll_timeout_id == 0)
	    icon_view->priv->scroll_timeout_id = gdk_threads_add_timeout (30, rubberband_scroll_timeout, 
								icon_view);
 	}
      else 
	remove_scroll_timeout (icon_view);
    }
  
  return TRUE;
}

static void
gtk_icon_view_remove (GtkContainer *container,
		      GtkWidget    *widget)
{
  GtkIconView *icon_view;
  GtkIconViewChild *child = NULL;
  GList *tmp_list;

  icon_view = GTK_ICON_VIEW (container);
  
  tmp_list = icon_view->priv->children;
  while (tmp_list)
    {
      child = tmp_list->data;
      if (child->widget == widget)
	{
	  gtk_widget_unparent (widget);

	  icon_view->priv->children = g_list_remove_link (icon_view->priv->children, tmp_list);
	  g_list_free_1 (tmp_list);
	  g_free (child);
	  return;
	}

      tmp_list = tmp_list->next;
    }
}

static void
gtk_icon_view_forall (GtkContainer *container,
		      gboolean      include_internals,
		      GtkCallback   callback,
		      gpointer      callback_data)
{
  GtkIconView *icon_view;
  GtkIconViewChild *child = NULL;
  GList *tmp_list;

  icon_view = GTK_ICON_VIEW (container);

  tmp_list = icon_view->priv->children;
  while (tmp_list)
    {
      child = tmp_list->data;
      tmp_list = tmp_list->next;

      (* callback) (child->widget, callback_data);
    }
}

static void
gtk_icon_view_item_activate_cell (GtkIconView         *icon_view, 
				  GtkIconViewItem     *item, 
				  GtkIconViewCellInfo *info,
				  GdkEvent            *event)
{
  GtkTreePath *path;  
  gchar *path_string;
  GdkRectangle cell_area;
  gboolean visible, mode;

  gtk_icon_view_set_cell_data (icon_view, item);

  g_object_get (info->cell,
		"visible", &visible,
		"mode", &mode,
		NULL);

  if (visible && mode == GTK_CELL_RENDERER_MODE_ACTIVATABLE)
    {
      gtk_icon_view_get_cell_area (icon_view, item, info, &cell_area);

      path = gtk_tree_path_new_from_indices (item->index, -1);
      path_string = gtk_tree_path_to_string (path);
      gtk_tree_path_free (path);

      gtk_cell_renderer_activate (info->cell, 
				  event, 
				  GTK_WIDGET (icon_view),
				  path_string, 
				  &cell_area, 
				  &cell_area, 
				  0);

      g_free (path_string);      
    }
}

static void 
gtk_icon_view_item_selected_changed (GtkIconView      *icon_view,
                                     GtkIconViewItem  *item)
{
}

static void 
gtk_icon_view_put (GtkIconView     *icon_view,
		   GtkWidget       *widget,
		   GtkIconViewItem *item,
		   gint             cell)
{
  GtkIconViewChild *child;
  
  child = g_new (GtkIconViewChild, 1);
  
  child->widget = widget;
  child->item = item;
  child->cell = cell;

  icon_view->priv->children = g_list_append (icon_view->priv->children, child);

  if (gtk_widget_get_realized (GTK_WIDGET (icon_view)))
    gtk_widget_set_parent_window (child->widget, icon_view->priv->bin_window);
  
  gtk_widget_set_parent (widget, GTK_WIDGET (icon_view));
}

static void
gtk_icon_view_remove_widget (GtkCellEditable *editable,
			     GtkIconView     *icon_view)
{
  GList *l;
  GtkIconViewItem *item;

  if (icon_view->priv->edited_item == NULL)
    return;

  item = icon_view->priv->edited_item;
  icon_view->priv->edited_item = NULL;
  icon_view->priv->editable = NULL;
  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = l->data;

      info->editing = FALSE;
    }

  if (gtk_widget_has_focus (GTK_WIDGET (editable)))
    gtk_widget_grab_focus (GTK_WIDGET (icon_view));
  
  g_signal_handlers_disconnect_by_func (editable,
					gtk_icon_view_remove_widget,
					icon_view);

  gtk_container_remove (GTK_CONTAINER (icon_view),
			GTK_WIDGET (editable));  

  gtk_icon_view_queue_draw_item (icon_view, item);
}


static void
gtk_icon_view_start_editing (GtkIconView         *icon_view, 
			     GtkIconViewItem     *item, 
			     GtkIconViewCellInfo *info,
			     GdkEvent            *event)
{
  GtkTreePath *path;  
  gchar *path_string;
  GdkRectangle cell_area;
  gboolean visible, mode;
  GtkCellEditable *editable;

  gtk_icon_view_set_cell_data (icon_view, item);

  g_object_get (info->cell,
		"visible", &visible,
		"mode", &mode,
		NULL);
  if (visible && mode == GTK_CELL_RENDERER_MODE_EDITABLE)
    {
      gtk_icon_view_get_cell_area (icon_view, item, info, &cell_area);

      path = gtk_tree_path_new_from_indices (item->index, -1);
      path_string = gtk_tree_path_to_string (path);
      gtk_tree_path_free (path);

      editable = gtk_cell_renderer_start_editing (info->cell, 
						  event, 
						  GTK_WIDGET (icon_view),
						  path_string, 
						  &cell_area, 
						  &cell_area, 
						  0);
      g_free (path_string);      

      /* the rest corresponds to tree_view_real_start_editing... */
      icon_view->priv->edited_item = item;
      icon_view->priv->editable = editable;
      info->editing = TRUE;

      gtk_icon_view_put (icon_view, GTK_WIDGET (editable), item, 
			 info->position);
      gtk_cell_editable_start_editing (GTK_CELL_EDITABLE (editable), 
				       (GdkEvent *)event);
      gtk_widget_grab_focus (GTK_WIDGET (editable));
      g_signal_connect (editable, "remove-widget",
			G_CALLBACK (gtk_icon_view_remove_widget), 
			icon_view);

    }
}

static void
gtk_icon_view_stop_editing (GtkIconView *icon_view,
			    gboolean     cancel_editing)
{
  GtkCellRenderer *cell = NULL;
  GtkIconViewItem *item;
  GList *l;

  if (icon_view->priv->edited_item == NULL)
    return;

  /*
   * This is very evil. We need to do this, because
   * gtk_cell_editable_editing_done may trigger gtk_icon_view_row_changed
   * later on. If gtk_icon_view_row_changed notices
   * icon_view->priv->edited_item != NULL, it'll call
   * gtk_icon_view_stop_editing again. Bad things will happen then.
   *
   * Please read that again if you intend to modify anything here.
   */

  item = icon_view->priv->edited_item;
  icon_view->priv->edited_item = NULL;

  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = l->data;

      if (info->editing)
	{
	  cell = info->cell;
	  break;
	}
    }

  if (cell == NULL)
    return;

  gtk_cell_renderer_stop_editing (cell, cancel_editing);
  if (!cancel_editing)
    gtk_cell_editable_editing_done (icon_view->priv->editable);

  icon_view->priv->edited_item = item;

  gtk_cell_editable_remove_widget (icon_view->priv->editable);
}

/**
 * gtk_icon_view_set_cursor:
 * @icon_view: A #GtkIconView
 * @path: A #GtkTreePath
 * @cell: (allow-none): One of the cell renderers of @icon_view, or %NULL
 * @start_editing: %TRUE if the specified cell should start being edited.
 *
 * Sets the current keyboard focus to be at @path, and selects it.  This is
 * useful when you want to focus the user's attention on a particular item.
 * If @cell is not %NULL, then focus is given to the cell specified by 
 * it. Additionally, if @start_editing is %TRUE, then editing should be 
 * started in the specified cell.  
 *
 * This function is often followed by <literal>gtk_widget_grab_focus 
 * (icon_view)</literal> in order to give keyboard focus to the widget.  
 * Please note that editing can only happen when the widget is realized.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_set_cursor (GtkIconView     *icon_view,
			  GtkTreePath     *path,
			  GtkCellRenderer *cell,
			  gboolean         start_editing)
{
  GtkIconViewItem *item = NULL;
  GtkIconViewCellInfo *info =  NULL;
  GList *l;
  gint i, cell_pos;

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (path != NULL);
  g_return_if_fail (cell == NULL || GTK_IS_CELL_RENDERER (cell));

  gtk_icon_view_stop_editing (icon_view, TRUE);

  if (gtk_tree_path_get_depth (path) == 1)
    item = g_list_nth_data (icon_view->priv->items,
			    gtk_tree_path_get_indices(path)[0]);
  
  if (!item)
    return;

  cell_pos = -1;
  for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
    {
      info = l->data;
      
      if (info->cell == cell)
	{
	  cell_pos = i;
	  break;
	}
	  
      info = NULL;
    }
  
  g_return_if_fail (cell == NULL || info != NULL);

  gtk_icon_view_set_cursor_item (icon_view, item, cell_pos);
  gtk_icon_view_scroll_to_path (icon_view, path, FALSE, 0.0, 0.0);
  
  if (info && start_editing)
    gtk_icon_view_start_editing (icon_view, item, info, NULL);
}

/**
 * gtk_icon_view_get_cursor:
 * @icon_view: A #GtkIconView
 * @path: (allow-none): Return location for the current cursor path, or %NULL
 * @cell: (allow-none): Return location the current focus cell, or %NULL
 *
 * Fills in @path and @cell with the current cursor path and cell. 
 * If the cursor isn't currently set, then *@path will be %NULL.  
 * If no cell currently has focus, then *@cell will be %NULL.
 *
 * The returned #GtkTreePath must be freed with gtk_tree_path_free().
 *
 * Return value: %TRUE if the cursor is set.
 *
 * Since: 2.8
 **/
gboolean
gtk_icon_view_get_cursor (GtkIconView      *icon_view,
			  GtkTreePath     **path,
			  GtkCellRenderer **cell)
{
  GtkIconViewItem *item;
  GtkIconViewCellInfo *info;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);

  item = icon_view->priv->cursor_item;
  if (icon_view->priv->cursor_cell < 0)
    info = NULL;
  else
    info = g_list_nth_data (icon_view->priv->cell_list, 
			    icon_view->priv->cursor_cell);

  if (path != NULL)
    {
      if (item != NULL)
	*path = gtk_tree_path_new_from_indices (item->index, -1);
      else
	*path = NULL;
    }

  if (cell != NULL)
    {
      if (info != NULL)
	*cell = info->cell;
      else 
	*cell = NULL;
    }

  return (item != NULL);
}

static gboolean
gtk_icon_view_button_press (GtkWidget      *widget,
			    GdkEventButton *event)
{
  GtkIconView *icon_view;
  GtkIconViewItem *item;
  GtkIconViewCellInfo *info = NULL;
  gboolean dirty = FALSE;
  GtkCellRendererMode mode;
  gint cursor_cell = -1;

  icon_view = GTK_ICON_VIEW (widget);

  if (event->window != icon_view->priv->bin_window)
    return FALSE;

  if (!gtk_widget_has_focus (widget))
    gtk_widget_grab_focus (widget);

  if (event->button == 1 && event->type == GDK_BUTTON_PRESS)
    {
      item = gtk_icon_view_get_item_at_coords (icon_view, 
					       event->x, event->y,
					       FALSE,
					       &info);    

      /*
       * We consider only the the cells' area as the item area if the
       * item is not selected, but if it *is* selected, the complete
       * selection rectangle is considered to be part of the item.
       */
      if (item != NULL && (info != NULL || item->selected))
	{
	  if (info != NULL)
	    {
	      g_object_get (info->cell, "mode", &mode, NULL);

	      if (mode == GTK_CELL_RENDERER_MODE_ACTIVATABLE ||
		  mode == GTK_CELL_RENDERER_MODE_EDITABLE)
		cursor_cell = g_list_index (icon_view->priv->cell_list, info);
	    }

	  gtk_icon_view_scroll_to_item (icon_view, item);
	  
	  if (icon_view->priv->selection_mode == GTK_SELECTION_NONE)
	    {
	      gtk_icon_view_set_cursor_item (icon_view, item, cursor_cell);
	    }
	  else if (icon_view->priv->selection_mode == GTK_SELECTION_MULTIPLE &&
		   (event->state & GTK_EXTEND_SELECTION_MOD_MASK))
	    {
	      gtk_icon_view_unselect_all_internal (icon_view);

	      gtk_icon_view_set_cursor_item (icon_view, item, cursor_cell);
	      if (!icon_view->priv->anchor_item)
		icon_view->priv->anchor_item = item;
	      else 
		gtk_icon_view_select_all_between (icon_view,
						  icon_view->priv->anchor_item,
						  item);
	      dirty = TRUE;
	    }
	  else 
	    {
	      if ((icon_view->priv->selection_mode == GTK_SELECTION_MULTIPLE ||
		  ((icon_view->priv->selection_mode == GTK_SELECTION_SINGLE) && item->selected)) &&
		  (event->state & GTK_MODIFY_SELECTION_MOD_MASK))
		{
		  item->selected = !item->selected;
		  gtk_icon_view_queue_draw_item (icon_view, item);
		  dirty = TRUE;
		}
	      else
		{
		  gtk_icon_view_unselect_all_internal (icon_view);

		  item->selected = TRUE;
		  gtk_icon_view_queue_draw_item (icon_view, item);
		  dirty = TRUE;
		}
	      gtk_icon_view_set_cursor_item (icon_view, item, cursor_cell);
	      icon_view->priv->anchor_item = item;
	    }

	  /* Save press to possibly begin a drag */
	  if (icon_view->priv->pressed_button < 0)
	    {
	      icon_view->priv->pressed_button = event->button;
	      icon_view->priv->press_start_x = event->x;
	      icon_view->priv->press_start_y = event->y;
	    }

	  if (!icon_view->priv->last_single_clicked)
	    icon_view->priv->last_single_clicked = item;

	  /* cancel the current editing, if it exists */
	  gtk_icon_view_stop_editing (icon_view, TRUE);

	  if (info != NULL)
	    {
	      if (mode == GTK_CELL_RENDERER_MODE_ACTIVATABLE)
		gtk_icon_view_item_activate_cell (icon_view, item, info, 
					          (GdkEvent *)event);
	      else if (mode == GTK_CELL_RENDERER_MODE_EDITABLE)
		gtk_icon_view_start_editing (icon_view, item, info, 
					     (GdkEvent *)event);
	    }
	}
      else
	{
	  if (icon_view->priv->selection_mode != GTK_SELECTION_BROWSE &&
	      !(event->state & GTK_MODIFY_SELECTION_MOD_MASK))
	    {
	      dirty = gtk_icon_view_unselect_all_internal (icon_view);
	    }
	  
	  if (icon_view->priv->selection_mode == GTK_SELECTION_MULTIPLE)
	    gtk_icon_view_start_rubberbanding (icon_view, event->x, event->y);
	}

      /* don't draw keyboard focus around an clicked-on item */
      icon_view->priv->draw_focus = FALSE;
    }

  if (event->button == 1 && event->type == GDK_2BUTTON_PRESS)
    {
      item = gtk_icon_view_get_item_at_coords (icon_view,
					       event->x, event->y,
					       FALSE,
					       NULL);

      if (item && item == icon_view->priv->last_single_clicked)
	{
	  GtkTreePath *path;

	  path = gtk_tree_path_new_from_indices (item->index, -1);
	  gtk_icon_view_item_activated (icon_view, path);
	  gtk_tree_path_free (path);
	}

      icon_view->priv->last_single_clicked = NULL;
      icon_view->priv->pressed_button = -1;
    }
  
  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);

  return event->button == 1;
}

static gboolean
gtk_icon_view_button_release (GtkWidget      *widget,
			      GdkEventButton *event)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (widget);
  
  if (icon_view->priv->pressed_button == event->button)
    icon_view->priv->pressed_button = -1;

  gtk_icon_view_stop_rubberbanding (icon_view);

  remove_scroll_timeout (icon_view);

  return TRUE;
}

static gboolean
gtk_icon_view_key_press (GtkWidget      *widget,
			 GdkEventKey    *event)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  if (icon_view->priv->doing_rubberband)
    {
      if (event->keyval == GDK_Escape)
	gtk_icon_view_stop_rubberbanding (icon_view);

      return TRUE;
    }

  return GTK_WIDGET_CLASS (gtk_icon_view_parent_class)->key_press_event (widget, event);
}

static gboolean
gtk_icon_view_key_release (GtkWidget      *widget,
			   GdkEventKey    *event)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  if (icon_view->priv->doing_rubberband)
    return TRUE;

  return GTK_WIDGET_CLASS (gtk_icon_view_parent_class)->key_press_event (widget, event);
}

static void
gtk_icon_view_update_rubberband (gpointer data)
{
  GtkIconView *icon_view;
  gint x, y;
  GdkRectangle old_area;
  GdkRectangle new_area;
  GdkRectangle common;
  GdkRegion *invalid_region;
  
  icon_view = GTK_ICON_VIEW (data);

  gdk_window_get_pointer (icon_view->priv->bin_window, &x, &y, NULL);

  x = MAX (x, 0);
  y = MAX (y, 0);

  old_area.x = MIN (icon_view->priv->rubberband_x1,
		    icon_view->priv->rubberband_x2);
  old_area.y = MIN (icon_view->priv->rubberband_y1,
		    icon_view->priv->rubberband_y2);
  old_area.width = ABS (icon_view->priv->rubberband_x2 -
			icon_view->priv->rubberband_x1) + 1;
  old_area.height = ABS (icon_view->priv->rubberband_y2 -
			 icon_view->priv->rubberband_y1) + 1;
  
  new_area.x = MIN (icon_view->priv->rubberband_x1, x);
  new_area.y = MIN (icon_view->priv->rubberband_y1, y);
  new_area.width = ABS (x - icon_view->priv->rubberband_x1) + 1;
  new_area.height = ABS (y - icon_view->priv->rubberband_y1) + 1;

  invalid_region = gdk_region_rectangle (&old_area);
  gdk_region_union_with_rect (invalid_region, &new_area);

  gdk_rectangle_intersect (&old_area, &new_area, &common);
  if (common.width > 2 && common.height > 2)
    {
      GdkRegion *common_region;

      /* make sure the border is invalidated */
      common.x += 1;
      common.y += 1;
      common.width -= 2;
      common.height -= 2;
      
      common_region = gdk_region_rectangle (&common);

      gdk_region_subtract (invalid_region, common_region);
      gdk_region_destroy (common_region);
    }
  
  gdk_window_invalidate_region (icon_view->priv->bin_window, invalid_region, TRUE);
    
  gdk_region_destroy (invalid_region);

  icon_view->priv->rubberband_x2 = x;
  icon_view->priv->rubberband_y2 = y;  

  gtk_icon_view_update_rubberband_selection (icon_view);
}

static void
gtk_icon_view_start_rubberbanding (GtkIconView  *icon_view,
				   gint          x,
				   gint          y)
{
  GList *items;

  g_assert (!icon_view->priv->doing_rubberband);

  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;

      item->selected_before_rubberbanding = item->selected;
    }
  
  icon_view->priv->rubberband_x1 = x;
  icon_view->priv->rubberband_y1 = y;
  icon_view->priv->rubberband_x2 = x;
  icon_view->priv->rubberband_y2 = y;

  icon_view->priv->doing_rubberband = TRUE;

  gtk_grab_add (GTK_WIDGET (icon_view));
}

static void
gtk_icon_view_stop_rubberbanding (GtkIconView *icon_view)
{
  if (!icon_view->priv->doing_rubberband)
    return;

  icon_view->priv->doing_rubberband = FALSE;

  gtk_grab_remove (GTK_WIDGET (icon_view));
  
  gtk_widget_queue_draw (GTK_WIDGET (icon_view));
}

static void
gtk_icon_view_update_rubberband_selection (GtkIconView *icon_view)
{
  GList *items;
  gint x, y, width, height;
  gboolean dirty = FALSE;
  
  x = MIN (icon_view->priv->rubberband_x1,
	   icon_view->priv->rubberband_x2);
  y = MIN (icon_view->priv->rubberband_y1,
	   icon_view->priv->rubberband_y2);
  width = ABS (icon_view->priv->rubberband_x1 - 
	       icon_view->priv->rubberband_x2);
  height = ABS (icon_view->priv->rubberband_y1 - 
		icon_view->priv->rubberband_y2);
  
  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;
      gboolean is_in;
      gboolean selected;
      
      is_in = gtk_icon_view_item_hit_test (icon_view, item, 
					   x, y, width, height);

      selected = is_in ^ item->selected_before_rubberbanding;

      if (item->selected != selected)
	{
	  item->selected = selected;
	  dirty = TRUE;
	  gtk_icon_view_queue_draw_item (icon_view, item);
	}
    }

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

static gboolean
gtk_icon_view_item_hit_test (GtkIconView      *icon_view,
			     GtkIconViewItem  *item,
			     gint              x,
			     gint              y,
			     gint              width,
			     gint              height)
{
  GList *l;
  GdkRectangle box;
 
  if (MIN (x + width, item->x + item->width) - MAX (x, item->x) <= 0 ||
      MIN (y + height, item->y + item->height) - MAX (y, item->y) <= 0)
    return FALSE;

  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;
      
      if (!info->cell->visible)
	continue;
      
      gtk_icon_view_get_cell_box (icon_view, item, info, &box);

      if (MIN (x + width, box.x + box.width) - MAX (x, box.x) > 0 &&
	MIN (y + height, box.y + box.height) - MAX (y, box.y) > 0)
	return TRUE;
    }

  return FALSE;
}

static gboolean
gtk_icon_view_unselect_all_internal (GtkIconView  *icon_view)
{
  gboolean dirty = FALSE;
  GList *items;

  if (icon_view->priv->selection_mode == GTK_SELECTION_NONE)
    return FALSE;

  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;

      if (item->selected)
	{
	  item->selected = FALSE;
	  dirty = TRUE;
	  gtk_icon_view_queue_draw_item (icon_view, item);
	  gtk_icon_view_item_selected_changed (icon_view, item);
	}
    }

  return dirty;
}


/* GtkIconView signals */
static void
gtk_icon_view_set_adjustments (GtkIconView   *icon_view,
			       GtkAdjustment *hadj,
			       GtkAdjustment *vadj)
{
  gboolean need_adjust = FALSE;

  if (hadj)
    g_return_if_fail (GTK_IS_ADJUSTMENT (hadj));
  else
    hadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));
  if (vadj)
    g_return_if_fail (GTK_IS_ADJUSTMENT (vadj));
  else
    vadj = GTK_ADJUSTMENT (gtk_adjustment_new (0.0, 0.0, 0.0, 0.0, 0.0, 0.0));

  if (icon_view->priv->hadjustment && (icon_view->priv->hadjustment != hadj))
    {
      g_signal_handlers_disconnect_matched (icon_view->priv->hadjustment, G_SIGNAL_MATCH_DATA,
					   0, 0, NULL, NULL, icon_view);
      g_object_unref (icon_view->priv->hadjustment);
    }

  if (icon_view->priv->vadjustment && (icon_view->priv->vadjustment != vadj))
    {
      g_signal_handlers_disconnect_matched (icon_view->priv->vadjustment, G_SIGNAL_MATCH_DATA,
					    0, 0, NULL, NULL, icon_view);
      g_object_unref (icon_view->priv->vadjustment);
    }

  if (icon_view->priv->hadjustment != hadj)
    {
      icon_view->priv->hadjustment = hadj;
      g_object_ref_sink (icon_view->priv->hadjustment);

      g_signal_connect (icon_view->priv->hadjustment, "value-changed",
			G_CALLBACK (gtk_icon_view_adjustment_changed),
			icon_view);
      need_adjust = TRUE;
    }

  if (icon_view->priv->vadjustment != vadj)
    {
      icon_view->priv->vadjustment = vadj;
      g_object_ref_sink (icon_view->priv->vadjustment);

      g_signal_connect (icon_view->priv->vadjustment, "value-changed",
			G_CALLBACK (gtk_icon_view_adjustment_changed),
			icon_view);
      need_adjust = TRUE;
    }

  if (need_adjust)
    gtk_icon_view_adjustment_changed (NULL, icon_view);
}

static void
gtk_icon_view_real_select_all (GtkIconView *icon_view)
{
  gtk_icon_view_select_all (icon_view);
}

static void
gtk_icon_view_real_unselect_all (GtkIconView *icon_view)
{
  gtk_icon_view_unselect_all (icon_view);
}

static void
gtk_icon_view_real_select_cursor_item (GtkIconView *icon_view)
{
  gtk_icon_view_unselect_all (icon_view);
  
  if (icon_view->priv->cursor_item != NULL)
    gtk_icon_view_select_item (icon_view, icon_view->priv->cursor_item);
}

static gboolean
gtk_icon_view_real_activate_cursor_item (GtkIconView *icon_view)
{
  GtkTreePath *path;
  GtkCellRendererMode mode;
  GtkIconViewCellInfo *info = NULL;
  
  if (!icon_view->priv->cursor_item)
    return FALSE;

  info = g_list_nth_data (icon_view->priv->cell_list, 
			  icon_view->priv->cursor_cell);

  if (info) 
    {  
      g_object_get (info->cell, "mode", &mode, NULL);

      if (mode == GTK_CELL_RENDERER_MODE_ACTIVATABLE)
	{
	  gtk_icon_view_item_activate_cell (icon_view, 
					    icon_view->priv->cursor_item, 
					    info, NULL);
	  return TRUE;
	}
      else if (mode == GTK_CELL_RENDERER_MODE_EDITABLE)
	{
	  gtk_icon_view_start_editing (icon_view, 
				       icon_view->priv->cursor_item, 
				       info, NULL);
	  return TRUE;
	}
    }
  
  path = gtk_tree_path_new_from_indices (icon_view->priv->cursor_item->index, -1);
  gtk_icon_view_item_activated (icon_view, path);
  gtk_tree_path_free (path);

  return TRUE;
}

static void
gtk_icon_view_real_toggle_cursor_item (GtkIconView *icon_view)
{
  if (!icon_view->priv->cursor_item)
    return;

  switch (icon_view->priv->selection_mode)
    {
    case GTK_SELECTION_NONE:
      break;
    case GTK_SELECTION_BROWSE:
      gtk_icon_view_select_item (icon_view, icon_view->priv->cursor_item);
      break;
    case GTK_SELECTION_SINGLE:
      if (icon_view->priv->cursor_item->selected)
	gtk_icon_view_unselect_item (icon_view, icon_view->priv->cursor_item);
      else
	gtk_icon_view_select_item (icon_view, icon_view->priv->cursor_item);
      break;
    case GTK_SELECTION_MULTIPLE:
      icon_view->priv->cursor_item->selected = !icon_view->priv->cursor_item->selected;
      g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0); 
      
      gtk_icon_view_item_selected_changed (icon_view, icon_view->priv->cursor_item);      
      gtk_icon_view_queue_draw_item (icon_view, icon_view->priv->cursor_item);
      break;
    }
}

/* Internal functions */
static void
gtk_icon_view_adjustment_changed (GtkAdjustment *adjustment,
				  GtkIconView   *icon_view)
{
  if (gtk_widget_get_realized (GTK_WIDGET (icon_view)))
    {
      gdk_window_move (icon_view->priv->bin_window,
		       - icon_view->priv->hadjustment->value,
		       - icon_view->priv->vadjustment->value);

      if (icon_view->priv->doing_rubberband)
	gtk_icon_view_update_rubberband (GTK_WIDGET (icon_view));

      gdk_window_process_updates (icon_view->priv->bin_window, TRUE);
    }
}

static GList *
gtk_icon_view_layout_single_row (GtkIconView *icon_view, 
				 GList       *first_item, 
				 gint         item_width,
				 gint         row,
				 gint        *y, 
				 gint        *maximum_width)
{
  gint focus_width;
  gint x, current_width;
  GList *items, *last_item;
  gint col;
  gint colspan;
  gint *max_height;
  gint i;
  gboolean rtl;

  rtl = gtk_widget_get_direction (GTK_WIDGET (icon_view)) == GTK_TEXT_DIR_RTL;
  max_height = g_new0 (gint, icon_view->priv->n_cells);

  x = 0;
  col = 0;
  items = first_item;
  current_width = 0;

  gtk_widget_style_get (GTK_WIDGET (icon_view),
			"focus-line-width", &focus_width,
			NULL);

  x += icon_view->priv->margin + focus_width;
  current_width += 2 * (icon_view->priv->margin + focus_width);

  items = first_item;
  while (items)
    {
      GtkIconViewItem *item = items->data;

      gtk_icon_view_calculate_item_size (icon_view, item);
      colspan = 1 + (item->width - 1) / (item_width + icon_view->priv->column_spacing);

      item->width = colspan * item_width + (colspan - 1) * icon_view->priv->column_spacing;

      current_width += item->width;

      if (items != first_item)
	{
	  if ((icon_view->priv->columns <= 0 && current_width > GTK_WIDGET (icon_view)->allocation.width) ||
	      (icon_view->priv->columns > 0 && col >= icon_view->priv->columns))
	    break;
	}

      current_width += icon_view->priv->column_spacing + 2 * focus_width;

      item->y = *y + focus_width;
      item->x = x;

      x = current_width - (icon_view->priv->margin + focus_width); 

      for (i = 0; i < icon_view->priv->n_cells; i++)
	max_height[i] = MAX (max_height[i], item->box[i].height);
	      
      if (current_width > *maximum_width)
	*maximum_width = current_width;

      item->row = row;
      item->col = col;

      col += colspan;
      items = items->next;
    }

  last_item = items;

  /* Now go through the row again and align the icons */
  for (items = first_item; items != last_item; items = items->next)
    {
      GtkIconViewItem *item = items->data;

      if (rtl)
	{
	  item->x = *maximum_width - item->width - item->x;
	  item->col = col - 1 - item->col;
	}

      gtk_icon_view_calculate_item_size2 (icon_view, item, max_height);

      /* We may want to readjust the new y coordinate. */
      if (item->y + item->height + focus_width + icon_view->priv->row_spacing > *y)
	*y = item->y + item->height + focus_width + icon_view->priv->row_spacing;
    }

  g_free (max_height);
  
  return last_item;
}

static void
gtk_icon_view_set_adjustment_upper (GtkAdjustment *adj,
				    gdouble        upper)
{
  if (upper != adj->upper)
    {
      gdouble min = MAX (0.0, upper - adj->page_size);
      gboolean value_changed = FALSE;
      
      adj->upper = upper;

      if (adj->value > min)
	{
	  adj->value = min;
	  value_changed = TRUE;
	}
      
      gtk_adjustment_changed (adj);
      
      if (value_changed)
	gtk_adjustment_value_changed (adj);
    }
}

static void
gtk_icon_view_layout (GtkIconView *icon_view)
{
  gint y = 0, maximum_width = 0;
  GList *icons;
  GtkWidget *widget;
  gint row;
  gint item_width;

  if (icon_view->priv->layout_idle_id != 0)
    {
      g_source_remove (icon_view->priv->layout_idle_id);
      icon_view->priv->layout_idle_id = 0;
    }
  
  if (icon_view->priv->model == NULL)
    return;

  widget = GTK_WIDGET (icon_view);

  item_width = icon_view->priv->item_width;

  if (item_width < 0)
    {
      for (icons = icon_view->priv->items; icons; icons = icons->next)
	{
	  GtkIconViewItem *item = icons->data;
	  gtk_icon_view_calculate_item_size (icon_view, item);
	  item_width = MAX (item_width, item->width);
	}
    }


  icons = icon_view->priv->items;
  y += icon_view->priv->margin;
  row = 0;

  if (icons)
    {
      gtk_icon_view_set_cell_data (icon_view, icons->data);
      adjust_wrap_width (icon_view, icons->data);
    }
  
  do
    {
      icons = gtk_icon_view_layout_single_row (icon_view, icons, 
					       item_width, row,
					       &y, &maximum_width);
      row++;
    }
  while (icons != NULL);

  if (maximum_width != icon_view->priv->width)
    icon_view->priv->width = maximum_width;

  y += icon_view->priv->margin;
  
  if (y != icon_view->priv->height)
    icon_view->priv->height = y;

  gtk_icon_view_set_adjustment_upper (icon_view->priv->hadjustment, 
				      icon_view->priv->width);
  gtk_icon_view_set_adjustment_upper (icon_view->priv->vadjustment, 
				      icon_view->priv->height);

  if (icon_view->priv->width != widget->requisition.width ||
      icon_view->priv->height != widget->requisition.height)
    gtk_widget_queue_resize_no_redraw (widget);

  if (gtk_widget_get_realized (GTK_WIDGET (icon_view)))
    gdk_window_resize (icon_view->priv->bin_window,
		       MAX (icon_view->priv->width, widget->allocation.width),
		       MAX (icon_view->priv->height, widget->allocation.height));

  if (icon_view->priv->scroll_to_path)
    {
      GtkTreePath *path;

      path = gtk_tree_row_reference_get_path (icon_view->priv->scroll_to_path);
      gtk_tree_row_reference_free (icon_view->priv->scroll_to_path);
      icon_view->priv->scroll_to_path = NULL;
      
      gtk_icon_view_scroll_to_path (icon_view, path,
				    icon_view->priv->scroll_to_use_align,
				    icon_view->priv->scroll_to_row_align,
				    icon_view->priv->scroll_to_col_align);
      gtk_tree_path_free (path);
    }
  
  gtk_widget_queue_draw (widget);
}

static void 
gtk_icon_view_get_cell_area (GtkIconView         *icon_view,
			     GtkIconViewItem     *item,
			     GtkIconViewCellInfo *info,
			     GdkRectangle        *cell_area)
{
  g_return_if_fail (info->position < item->n_cells);

  if (icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      cell_area->x = item->box[info->position].x - item->before[info->position];
      cell_area->y = item->y + icon_view->priv->item_padding;
      cell_area->width = item->box[info->position].width + 
	item->before[info->position] + item->after[info->position];
      cell_area->height = item->height - icon_view->priv->item_padding * 2;
    }
  else
    {
      cell_area->x = item->x + icon_view->priv->item_padding;
      cell_area->y = item->box[info->position].y - item->before[info->position];
      cell_area->width = item->width - icon_view->priv->item_padding * 2;
      cell_area->height = item->box[info->position].height + 
	item->before[info->position] + item->after[info->position];
    }
}

static void 
gtk_icon_view_get_cell_box (GtkIconView         *icon_view,
			    GtkIconViewItem     *item,
			    GtkIconViewCellInfo *info,
			    GdkRectangle        *box)
{
  g_return_if_fail (info->position < item->n_cells);

  *box = item->box[info->position];
}

/* try to guess a reasonable wrap width for an implicit text cell renderer
 */
static void
adjust_wrap_width (GtkIconView     *icon_view,
		   GtkIconViewItem *item)
{
  GtkIconViewCellInfo *text_info;
  GtkIconViewCellInfo *pixbuf_info;
  gint pixbuf_width, wrap_width;
      
  if (icon_view->priv->text_cell != -1 &&
      icon_view->priv->pixbuf_cell != -1)
    {
      gint item_width;

      text_info = g_list_nth_data (icon_view->priv->cell_list,
				   icon_view->priv->text_cell);
      pixbuf_info = g_list_nth_data (icon_view->priv->cell_list,
				     icon_view->priv->pixbuf_cell);
      
      gtk_cell_renderer_get_size (pixbuf_info->cell, 
				  GTK_WIDGET (icon_view), 
				  NULL, NULL, NULL,
				  &pixbuf_width, 
				  NULL);
	  

      if (icon_view->priv->item_width > 0)
	item_width = icon_view->priv->item_width;
      else
	item_width = item->width;

      if (icon_view->priv->item_orientation == GTK_ORIENTATION_VERTICAL)
        wrap_width = item_width;
      else {
        if (item->width == -1 && item_width <= 0)
          wrap_width = MAX (2 * pixbuf_width, 50);
        else
          wrap_width = item_width - pixbuf_width - icon_view->priv->spacing;
        }

      wrap_width -= icon_view->priv->item_padding * 2;

      g_object_set (text_info->cell, "wrap-width", wrap_width, NULL);
      g_object_set (text_info->cell, "width", wrap_width, NULL);
    }
}

static void
gtk_icon_view_calculate_item_size (GtkIconView     *icon_view,
				   GtkIconViewItem *item)
{
  gint spacing;
  GList *l;

  if (item->width != -1 && item->height != -1) 
    return;

  if (item->n_cells != icon_view->priv->n_cells)
    {
      g_free (item->before);
      g_free (item->after);
      g_free (item->box);
      
      item->before = g_new0 (gint, icon_view->priv->n_cells);
      item->after = g_new0 (gint, icon_view->priv->n_cells);
      item->box = g_new0 (GdkRectangle, icon_view->priv->n_cells);

      item->n_cells = icon_view->priv->n_cells;
    }

  gtk_icon_view_set_cell_data (icon_view, item);

  spacing = icon_view->priv->spacing;

  item->width = 0;
  item->height = 0;
  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;
      
      if (!info->cell->visible)
	continue;
      
      gtk_cell_renderer_get_size (info->cell, GTK_WIDGET (icon_view), 
				  NULL, NULL, NULL,
				  &item->box[info->position].width, 
				  &item->box[info->position].height);

      if (icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
	{
	  item->width += item->box[info->position].width 
	    + (info->position > 0 ? spacing : 0);
	  item->height = MAX (item->height, item->box[info->position].height);
	}
      else
	{
	  item->width = MAX (item->width, item->box[info->position].width);
	  item->height += item->box[info->position].height + (info->position > 0 ? spacing : 0);
	}
    }

  item->width += icon_view->priv->item_padding * 2;
  item->height += icon_view->priv->item_padding * 2;
}

static void
gtk_icon_view_calculate_item_size2 (GtkIconView     *icon_view,
				    GtkIconViewItem *item,
				    gint            *max_height)
{
  GdkRectangle cell_area;
  gint spacing;
  GList *l;
  gint i, k;
  gboolean rtl;

  rtl = gtk_widget_get_direction (GTK_WIDGET (icon_view)) == GTK_TEXT_DIR_RTL;

  gtk_icon_view_set_cell_data (icon_view, item);

  spacing = icon_view->priv->spacing;

  item->height = 0;
  for (i = 0; i < icon_view->priv->n_cells; i++)
    {
      if (icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
	item->height = MAX (item->height, max_height[i]);
      else
	item->height += max_height[i] + (i > 0 ? spacing : 0);
    }

  cell_area.x = item->x + icon_view->priv->item_padding;
  cell_area.y = item->y + icon_view->priv->item_padding;
      
  for (k = 0; k < 2; k++)
    for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
      {
	GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;
	
	if (info->pack == (k ? GTK_PACK_START : GTK_PACK_END))
	  continue;

	if (!info->cell->visible)
	  continue;

	if (icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
	  {
            /* We should not subtract icon_view->priv->item_padding from item->height,
             * because item->height is recalculated above using
             * max_height which does not contain item padding.
             */
	    cell_area.width = item->box[info->position].width;
	    cell_area.height = item->height;
	  }
	else
	  {
            /* item->width is not recalculated and thus needs to be
             * corrected for the padding.
             */
	    cell_area.width = item->width - 2 * icon_view->priv->item_padding;
	    cell_area.height = max_height[i];
	  }
	
	gtk_cell_renderer_get_size (info->cell, GTK_WIDGET (icon_view), 
				    &cell_area,
				    &item->box[info->position].x, &item->box[info->position].y,
				    &item->box[info->position].width, &item->box[info->position].height);
	
	item->box[info->position].x += cell_area.x;
	item->box[info->position].y += cell_area.y;
	if (icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
	  {
	    item->before[info->position] = item->box[info->position].x - cell_area.x;
	    item->after[info->position] = cell_area.width - item->box[info->position].width - item->before[info->position];
	    cell_area.x += cell_area.width + spacing;
	  }
	else
	  {
	    if (item->box[info->position].width > item->width - icon_view->priv->item_padding * 2)
	      {
		item->width = item->box[info->position].width + icon_view->priv->item_padding * 2;
		cell_area.width = item->width;
	      }
	    item->before[info->position] = item->box[info->position].y - cell_area.y;
	    item->after[info->position] = cell_area.height - item->box[info->position].height - item->before[info->position];
	    cell_area.y += cell_area.height + spacing;
	  }
      }
  
  if (rtl && icon_view->priv->item_orientation == GTK_ORIENTATION_HORIZONTAL)
    {
      for (i = 0; i < icon_view->priv->n_cells; i++)
	{
	  item->box[i].x = item->x + item->width - 
	    (item->box[i].x + item->box[i].width - item->x);
	}      
    }

  item->height += icon_view->priv->item_padding * 2;
}

static void
gtk_icon_view_invalidate_sizes (GtkIconView *icon_view)
{
  g_list_foreach (icon_view->priv->items,
		  (GFunc)gtk_icon_view_item_invalidate_size, NULL);
}

static void
gtk_icon_view_item_invalidate_size (GtkIconViewItem *item)
{
  item->width = -1;
  item->height = -1;
}

static void
gtk_icon_view_paint_item (GtkIconView     *icon_view,
			  cairo_t         *cr,
			  GtkIconViewItem *item,
			  GdkRectangle    *area,
			  GdkDrawable     *drawable,
			  gint             x,
			  gint             y,
			  gboolean         draw_focus)
{
  gint focus_width;
  gint padding;
  GdkRectangle cell_area, box;
  GList *l;
  gint i;
  GtkCellRendererState flags;
      
  if (icon_view->priv->model == NULL)
    return;
  
  gtk_icon_view_set_cell_data (icon_view, item);

  gtk_widget_style_get (GTK_WIDGET (icon_view),
			"focus-line-width", &focus_width,
			NULL);
  
  padding = focus_width; 
  
  if (item->selected)
    {
      flags = GTK_CELL_RENDERER_SELECTED;
    }
  else
    {
      flags = 0;
    }
  
#ifdef DEBUG_ICON_VIEW
  gdk_draw_rectangle (drawable,
		      GTK_WIDGET (icon_view)->style->black_gc,
		      FALSE,
		      x, y, 
		      item->width, item->height);
#endif

  if (item->selected)
    {
      gtk_paint_flat_box (GTK_WIDGET (icon_view)->style,
			  (GdkWindow *) drawable,
			  GTK_STATE_SELECTED,
			  GTK_SHADOW_NONE,
			  area,
			  GTK_WIDGET (icon_view),
			  "icon_view_item",
			  x, y,
			  item->width, item->height);
    }
  
  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;
      
      if (!info->cell->visible)
	continue;
      
      gtk_icon_view_get_cell_area (icon_view, item, info, &cell_area);
      
#ifdef DEBUG_ICON_VIEW
      gdk_draw_rectangle (drawable,
			  GTK_WIDGET (icon_view)->style->black_gc,
			  FALSE,
			  x - item->x + cell_area.x, 
			  y - item->y + cell_area.y, 
			  cell_area.width, cell_area.height);

      gtk_icon_view_get_cell_box (icon_view, item, info, &box);
	  
      gdk_draw_rectangle (drawable,
			  GTK_WIDGET (icon_view)->style->black_gc,
			  FALSE,
			  x - item->x + box.x, 
			  y - item->y + box.y, 
			  box.width, box.height);
#endif

      cell_area.x = x - item->x + cell_area.x;
      cell_area.y = y - item->y + cell_area.y;

      gtk_cell_renderer_render (info->cell,
				drawable,
				GTK_WIDGET (icon_view),
				&cell_area, &cell_area, area, flags);
    }

  if (draw_focus &&
      gtk_widget_has_focus (GTK_WIDGET (icon_view)) &&
      item == icon_view->priv->cursor_item)
    {
      for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
        {
          GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;

          if (!info->cell->visible)
            continue;

          /* If found a editable/activatable cell, draw focus on it. */
          if (icon_view->priv->cursor_cell < 0 &&
              info->cell->mode != GTK_CELL_RENDERER_MODE_INERT)
            icon_view->priv->cursor_cell = i;

          gtk_icon_view_get_cell_box (icon_view, item, info, &box);

          if (i == icon_view->priv->cursor_cell)
            {
              gtk_paint_focus (GTK_WIDGET (icon_view)->style,
                               drawable,
                               GTK_STATE_NORMAL,
                               area,
                               GTK_WIDGET (icon_view),
                               "icon_view",
                               x - item->x + box.x - padding,
                               y - item->y + box.y - padding,
                               box.width + 2 * padding,
                               box.height + 2 * padding);
              break;
            }
        }

      /* If there are no editable/activatable cells, draw focus 
       * around the whole item.
       */
      if (icon_view->priv->cursor_cell < 0)
        gtk_paint_focus (GTK_WIDGET (icon_view)->style,
                         drawable,
                         GTK_STATE_NORMAL,
                         area,
                         GTK_WIDGET (icon_view),
                         "icon_view",
                         x - padding,
                         y - padding,
                         item->width + 2 * padding,
                         item->height + 2 * padding);
    }
}

static void
gtk_icon_view_paint_rubberband (GtkIconView     *icon_view,
				cairo_t         *cr,
				GdkRectangle    *area)
{
  GdkRectangle rect;
  GdkRectangle rubber_rect;
  GdkColor *fill_color_gdk;
  guchar fill_color_alpha;

  rubber_rect.x = MIN (icon_view->priv->rubberband_x1, icon_view->priv->rubberband_x2);
  rubber_rect.y = MIN (icon_view->priv->rubberband_y1, icon_view->priv->rubberband_y2);
  rubber_rect.width = ABS (icon_view->priv->rubberband_x1 - icon_view->priv->rubberband_x2) + 1;
  rubber_rect.height = ABS (icon_view->priv->rubberband_y1 - icon_view->priv->rubberband_y2) + 1;

  if (!gdk_rectangle_intersect (&rubber_rect, area, &rect))
    return;

  gtk_widget_style_get (GTK_WIDGET (icon_view),
                        "selection-box-color", &fill_color_gdk,
                        "selection-box-alpha", &fill_color_alpha,
                        NULL);

  if (!fill_color_gdk)
    fill_color_gdk = gdk_color_copy (&GTK_WIDGET (icon_view)->style->base[GTK_STATE_SELECTED]);

  cairo_set_source_rgba (cr,
			 fill_color_gdk->red / 65535.,
			 fill_color_gdk->green / 65535.,
			 fill_color_gdk->blue / 65535.,
			 fill_color_alpha / 255.);

  cairo_save (cr);
  gdk_cairo_rectangle (cr, &rect);
  cairo_clip (cr);
  cairo_paint (cr);

  /* Draw the border without alpha */
  cairo_set_source_rgb (cr,
			fill_color_gdk->red / 65535.,
			fill_color_gdk->green / 65535.,
			fill_color_gdk->blue / 65535.);
  cairo_rectangle (cr, 
		   rubber_rect.x + 0.5, rubber_rect.y + 0.5,
		   rubber_rect.width - 1, rubber_rect.height - 1);
  cairo_stroke (cr);
  cairo_restore (cr);

  gdk_color_free (fill_color_gdk);
}

static void
gtk_icon_view_queue_draw_path (GtkIconView *icon_view,
			       GtkTreePath *path)
{
  GList *l;
  gint index;

  index = gtk_tree_path_get_indices (path)[0];

  for (l = icon_view->priv->items; l; l = l->next) 
    {
      GtkIconViewItem *item = l->data;

      if (item->index == index)
	{
	  gtk_icon_view_queue_draw_item (icon_view, item);
	  break;
	}
    }
}

static void
gtk_icon_view_queue_draw_item (GtkIconView     *icon_view,
			       GtkIconViewItem *item)
{
  gint focus_width;
  GdkRectangle rect;

  gtk_widget_style_get (GTK_WIDGET (icon_view),
			"focus-line-width", &focus_width,
			NULL);

  rect.x = item->x - focus_width;
  rect.y = item->y - focus_width;
  rect.width = item->width + 2 * focus_width;
  rect.height = item->height + 2 * focus_width;

  if (icon_view->priv->bin_window)
    gdk_window_invalidate_rect (icon_view->priv->bin_window, &rect, TRUE);
}

static gboolean
layout_callback (gpointer user_data)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (user_data);
  
  icon_view->priv->layout_idle_id = 0;

  gtk_icon_view_layout (icon_view);
  
  return FALSE;
}

static void
gtk_icon_view_queue_layout (GtkIconView *icon_view)
{
  if (icon_view->priv->layout_idle_id != 0)
    return;

  icon_view->priv->layout_idle_id = gdk_threads_add_idle (layout_callback, icon_view);
}

static void
gtk_icon_view_set_cursor_item (GtkIconView     *icon_view,
			       GtkIconViewItem *item,
			       gint             cursor_cell)
{

  if (icon_view->priv->cursor_item == item &&
      (cursor_cell < 0 || cursor_cell == icon_view->priv->cursor_cell))
    return;

  if (icon_view->priv->cursor_item != NULL)
    {
      gtk_icon_view_queue_draw_item (icon_view, icon_view->priv->cursor_item);
    }
  icon_view->priv->cursor_item = item;
  if (cursor_cell >= 0)
    icon_view->priv->cursor_cell = cursor_cell;

  gtk_icon_view_queue_draw_item (icon_view, item);
}


static GtkIconViewItem *
gtk_icon_view_item_new (void)
{
  GtkIconViewItem *item;

  item = g_new0 (GtkIconViewItem, 1);

  item->width = -1;
  item->height = -1;
  
  return item;
}

static void
gtk_icon_view_item_free (GtkIconViewItem *item)
{
  g_return_if_fail (item != NULL);

  g_free (item->before);
  g_free (item->after);
  g_free (item->box);

  g_free (item);
}


static GtkIconViewItem *
gtk_icon_view_get_item_at_coords (GtkIconView          *icon_view,
				  gint                  x,
				  gint                  y,
				  gboolean              only_in_cell,
				  GtkIconViewCellInfo **cell_at_pos)
{
  GList *items, *l;
  GdkRectangle box;

  if (cell_at_pos)
    *cell_at_pos = NULL;

  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;

      if (x >= item->x - icon_view->priv->column_spacing/2 && x <= item->x + item->width + icon_view->priv->column_spacing/2 &&
	  y >= item->y - icon_view->priv->row_spacing/2 && y <= item->y + item->height + icon_view->priv->row_spacing/2)
	{
	  if (only_in_cell || cell_at_pos)
	    {
	      gtk_icon_view_set_cell_data (icon_view, item);

	      for (l = icon_view->priv->cell_list; l; l = l->next)
		{
		  GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;

		  if (!info->cell->visible)
		    continue;

		  gtk_icon_view_get_cell_box (icon_view, item, info, &box);

		  if ((x >= box.x && x <= box.x + box.width &&
		       y >= box.y && y <= box.y + box.height) ||
		      (x >= box.x  &&
		       x <= box.x + box.width &&
		       y >= box.y &&
		       y <= box.y + box.height))
		    {
		      if (cell_at_pos)
			*cell_at_pos = info;

		      return item;
		    }
		}

	      if (only_in_cell)
		return NULL;
	    }

	  return item;
	}
    }

  return NULL;
}

static void
gtk_icon_view_select_item (GtkIconView      *icon_view,
			   GtkIconViewItem  *item)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (item != NULL);

  if (item->selected)
    return;
  
  if (icon_view->priv->selection_mode == GTK_SELECTION_NONE)
    return;
  else if (icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    gtk_icon_view_unselect_all_internal (icon_view);

  item->selected = TRUE;

  gtk_icon_view_item_selected_changed (icon_view, item);
  g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);

  gtk_icon_view_queue_draw_item (icon_view, item);
}


static void
gtk_icon_view_unselect_item (GtkIconView      *icon_view,
			     GtkIconViewItem  *item)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (item != NULL);

  if (!item->selected)
    return;
  
  if (icon_view->priv->selection_mode == GTK_SELECTION_NONE ||
      icon_view->priv->selection_mode == GTK_SELECTION_BROWSE)
    return;
  
  item->selected = FALSE;

  gtk_icon_view_item_selected_changed (icon_view, item);
  g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);

  gtk_icon_view_queue_draw_item (icon_view, item);
}

static void
verify_items (GtkIconView *icon_view)
{
  GList *items;
  int i = 0;

  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;

      if (item->index != i)
	g_error ("List item does not match its index: "
		 "item index %d and list index %d\n", item->index, i);

      i++;
    }
}

static void
gtk_icon_view_row_changed (GtkTreeModel *model,
			   GtkTreePath  *path,
			   GtkTreeIter  *iter,
			   gpointer      data)
{
  GtkIconViewItem *item;
  gint index;
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (data);

  gtk_icon_view_stop_editing (icon_view, TRUE);
  
  index = gtk_tree_path_get_indices(path)[0];
  item = g_list_nth_data (icon_view->priv->items, index);

  gtk_icon_view_item_invalidate_size (item);
  gtk_icon_view_queue_layout (icon_view);

  verify_items (icon_view);
}

static void
gtk_icon_view_row_inserted (GtkTreeModel *model,
			    GtkTreePath  *path,
			    GtkTreeIter  *iter,
			    gpointer      data)
{
  gint index;
  GtkIconViewItem *item;
  gboolean iters_persist;
  GtkIconView *icon_view;
  GList *list;
  
  icon_view = GTK_ICON_VIEW (data);

  iters_persist = gtk_tree_model_get_flags (icon_view->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST;
  
  index = gtk_tree_path_get_indices(path)[0];

  item = gtk_icon_view_item_new ();

  if (iters_persist)
    item->iter = *iter;

  item->index = index;

  /* FIXME: We can be more efficient here,
     we can store a tail pointer and use that when
     appending (which is a rather common operation)
  */
  icon_view->priv->items = g_list_insert (icon_view->priv->items,
					 item, index);
  
  list = g_list_nth (icon_view->priv->items, index + 1);
  for (; list; list = list->next)
    {
      item = list->data;

      item->index++;
    }
    
  verify_items (icon_view);

  gtk_icon_view_queue_layout (icon_view);
}

static void
gtk_icon_view_row_deleted (GtkTreeModel *model,
			   GtkTreePath  *path,
			   gpointer      data)
{
  gint index;
  GtkIconView *icon_view;
  GtkIconViewItem *item;
  GList *list, *next;
  gboolean emit = FALSE;
  
  icon_view = GTK_ICON_VIEW (data);

  index = gtk_tree_path_get_indices(path)[0];

  list = g_list_nth (icon_view->priv->items, index);
  item = list->data;

  gtk_icon_view_stop_editing (icon_view, TRUE);

  if (item == icon_view->priv->anchor_item)
    icon_view->priv->anchor_item = NULL;

  if (item == icon_view->priv->cursor_item)
    icon_view->priv->cursor_item = NULL;

  if (item->selected)
    emit = TRUE;
  
  gtk_icon_view_item_free (item);

  for (next = list->next; next; next = next->next)
    {
      item = next->data;

      item->index--;
    }
  
  icon_view->priv->items = g_list_delete_link (icon_view->priv->items, list);

  verify_items (icon_view);  
  
  gtk_icon_view_queue_layout (icon_view);

  if (emit)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

static void
gtk_icon_view_rows_reordered (GtkTreeModel *model,
			      GtkTreePath  *parent,
			      GtkTreeIter  *iter,
			      gint         *new_order,
			      gpointer      data)
{
  int i;
  int length;
  GtkIconView *icon_view;
  GList *items = NULL, *list;
  GtkIconViewItem **item_array;
  gint *order;
  
  icon_view = GTK_ICON_VIEW (data);

  gtk_icon_view_stop_editing (icon_view, TRUE);

  length = gtk_tree_model_iter_n_children (model, NULL);

  order = g_new (gint, length);
  for (i = 0; i < length; i++)
    order [new_order[i]] = i;

  item_array = g_new (GtkIconViewItem *, length);
  for (i = 0, list = icon_view->priv->items; list != NULL; list = list->next, i++)
    item_array[order[i]] = list->data;
  g_free (order);

  for (i = length - 1; i >= 0; i--)
    {
      item_array[i]->index = i;
      items = g_list_prepend (items, item_array[i]);
    }
  
  g_free (item_array);
  g_list_free (icon_view->priv->items);
  icon_view->priv->items = items;

  gtk_icon_view_queue_layout (icon_view);

  verify_items (icon_view);  
}

static void
gtk_icon_view_build_items (GtkIconView *icon_view)
{
  GtkTreeIter iter;
  int i;
  gboolean iters_persist;
  GList *items = NULL;

  iters_persist = gtk_tree_model_get_flags (icon_view->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST;
  
  if (!gtk_tree_model_get_iter_first (icon_view->priv->model,
				      &iter))
    return;

  i = 0;
  
  do
    {
      GtkIconViewItem *item = gtk_icon_view_item_new ();

      if (iters_persist)
	item->iter = iter;

      item->index = i;
      
      i++;

      items = g_list_prepend (items, item);
      
    } while (gtk_tree_model_iter_next (icon_view->priv->model, &iter));

  icon_view->priv->items = g_list_reverse (items);
}

static void
gtk_icon_view_add_move_binding (GtkBindingSet  *binding_set,
				guint           keyval,
				guint           modmask,
				GtkMovementStep step,
				gint            count)
{
  
  gtk_binding_entry_add_signal (binding_set, keyval, modmask,
                                I_("move-cursor"), 2,
                                G_TYPE_ENUM, step,
                                G_TYPE_INT, count);

  gtk_binding_entry_add_signal (binding_set, keyval, GDK_SHIFT_MASK,
                                "move-cursor", 2,
                                G_TYPE_ENUM, step,
                                G_TYPE_INT, count);

  if ((modmask & GDK_CONTROL_MASK) == GDK_CONTROL_MASK)
   return;

  gtk_binding_entry_add_signal (binding_set, keyval, GDK_CONTROL_MASK | GDK_SHIFT_MASK,
                                "move-cursor", 2,
                                G_TYPE_ENUM, step,
                                G_TYPE_INT, count);

  gtk_binding_entry_add_signal (binding_set, keyval, GDK_CONTROL_MASK,
                                "move-cursor", 2,
                                G_TYPE_ENUM, step,
                                G_TYPE_INT, count);
}

static gboolean
gtk_icon_view_real_move_cursor (GtkIconView     *icon_view,
				GtkMovementStep  step,
				gint             count)
{
  GdkModifierType state;

  g_return_val_if_fail (GTK_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (step == GTK_MOVEMENT_LOGICAL_POSITIONS ||
			step == GTK_MOVEMENT_VISUAL_POSITIONS ||
			step == GTK_MOVEMENT_DISPLAY_LINES ||
			step == GTK_MOVEMENT_PAGES ||
			step == GTK_MOVEMENT_BUFFER_ENDS, FALSE);

  if (!gtk_widget_has_focus (GTK_WIDGET (icon_view)))
    return FALSE;

  gtk_icon_view_stop_editing (icon_view, FALSE);
  gtk_widget_grab_focus (GTK_WIDGET (icon_view));

  if (gtk_get_current_event_state (&state))
    {
      if ((state & GTK_MODIFY_SELECTION_MOD_MASK) == GTK_MODIFY_SELECTION_MOD_MASK)
        icon_view->priv->modify_selection_pressed = TRUE;
      if ((state & GTK_EXTEND_SELECTION_MOD_MASK) == GTK_EXTEND_SELECTION_MOD_MASK)
        icon_view->priv->extend_selection_pressed = TRUE;
    }
  /* else we assume not pressed */

  switch (step)
    {
    case GTK_MOVEMENT_LOGICAL_POSITIONS:
    case GTK_MOVEMENT_VISUAL_POSITIONS:
      gtk_icon_view_move_cursor_left_right (icon_view, count);
      break;
    case GTK_MOVEMENT_DISPLAY_LINES:
      gtk_icon_view_move_cursor_up_down (icon_view, count);
      break;
    case GTK_MOVEMENT_PAGES:
      gtk_icon_view_move_cursor_page_up_down (icon_view, count);
      break;
    case GTK_MOVEMENT_BUFFER_ENDS:
      gtk_icon_view_move_cursor_start_end (icon_view, count);
      break;
    default:
      g_assert_not_reached ();
    }

  icon_view->priv->modify_selection_pressed = FALSE;
  icon_view->priv->extend_selection_pressed = FALSE;

  icon_view->priv->draw_focus = TRUE;

  return TRUE;
}

static GtkIconViewItem *
find_item (GtkIconView     *icon_view,
	   GtkIconViewItem *current,
	   gint             row_ofs,
	   gint             col_ofs)
{
  gint row, col;
  GList *items;
  GtkIconViewItem *item;

  /* FIXME: this could be more efficient 
   */
  row = current->row + row_ofs;
  col = current->col + col_ofs;

  for (items = icon_view->priv->items; items; items = items->next)
    {
      item = items->data;
      if (item->row == row && item->col == col)
	return item;
    }
  
  return NULL;
}

static gint
find_cell (GtkIconView     *icon_view,
	   GtkIconViewItem *item,
	   gint             cell,
	   GtkOrientation   orientation,
	   gint             step,
	   gint            *count)
{
  gint n_focusable;
  gint *focusable;
  gint current;
  gint i, k;
  GList *l;

  if (icon_view->priv->item_orientation != orientation)
    return cell;

  gtk_icon_view_set_cell_data (icon_view, item);

  focusable = g_new0 (gint, icon_view->priv->n_cells);
  n_focusable = 0;

  current = 0;
  for (k = 0; k < 2; k++)
    for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
      {
	GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;
	
	if (info->pack == (k ? GTK_PACK_START : GTK_PACK_END))
	  continue;
	
	if (!info->cell->visible)
	  continue;

	if (info->cell->mode != GTK_CELL_RENDERER_MODE_INERT)
	  {
	    if (cell == i)
	      current = n_focusable;

	    focusable[n_focusable] = i;

	    n_focusable++;
	  }
      }
  
  if (n_focusable == 0)
    {
      g_free (focusable);
      return -1;
    }

  if (cell < 0)
    {
      current = step > 0 ? 0 : n_focusable - 1;
      cell = focusable[current];
    }

  if (current + *count < 0)
    {
      cell = -1;
      *count = current + *count;
    }
  else if (current + *count > n_focusable - 1)
    {
      cell = -1;
      *count = current + *count - (n_focusable - 1);
    }
  else
    {
      cell = focusable[current + *count];
      *count = 0;
    }
  
  g_free (focusable);
  
  return cell;
}

static GtkIconViewItem *
find_item_page_up_down (GtkIconView     *icon_view,
			GtkIconViewItem *current,
			gint             count)
{
  GList *item, *next;
  gint y, col;
  
  col = current->col;
  y = current->y + count * icon_view->priv->vadjustment->page_size;

  item = g_list_find (icon_view->priv->items, current);
  if (count > 0)
    {
      while (item)
	{
	  for (next = item->next; next; next = next->next)
	    {
	      if (((GtkIconViewItem *)next->data)->col == col)
		break;
	    }
	  if (!next || ((GtkIconViewItem *)next->data)->y > y)
	    break;

	  item = next;
	}
    }
  else 
    {
      while (item)
	{
	  for (next = item->prev; next; next = next->prev)
	    {
	      if (((GtkIconViewItem *)next->data)->col == col)
		break;
	    }
	  if (!next || ((GtkIconViewItem *)next->data)->y < y)
	    break;

	  item = next;
	}
    }

  if (item)
    return item->data;

  return NULL;
}

static gboolean
gtk_icon_view_select_all_between (GtkIconView     *icon_view,
				  GtkIconViewItem *anchor,
				  GtkIconViewItem *cursor)
{
  GList *items;
  GtkIconViewItem *item;
  gint row1, row2, col1, col2;
  gboolean dirty = FALSE;
  
  if (anchor->row < cursor->row)
    {
      row1 = anchor->row;
      row2 = cursor->row;
    }
  else
    {
      row1 = cursor->row;
      row2 = anchor->row;
    }

  if (anchor->col < cursor->col)
    {
      col1 = anchor->col;
      col2 = cursor->col;
    }
  else
    {
      col1 = cursor->col;
      col2 = anchor->col;
    }

  for (items = icon_view->priv->items; items; items = items->next)
    {
      item = items->data;

      if (row1 <= item->row && item->row <= row2 &&
	  col1 <= item->col && item->col <= col2)
	{
	  if (!item->selected)
	    {
	      dirty = TRUE;
	      item->selected = TRUE;
	      gtk_icon_view_item_selected_changed (icon_view, item);
	    }
	  gtk_icon_view_queue_draw_item (icon_view, item);
	}
    }

  return dirty;
}

static void 
gtk_icon_view_move_cursor_up_down (GtkIconView *icon_view,
				   gint         count)
{
  GtkIconViewItem *item;
  gint cell;
  gboolean dirty = FALSE;
  gint step;
  GtkDirectionType direction;
  
  if (!gtk_widget_has_focus (GTK_WIDGET (icon_view)))
    return;

  direction = count < 0 ? GTK_DIR_UP : GTK_DIR_DOWN;

  if (!icon_view->priv->cursor_item)
    {
      GList *list;

      if (count > 0)
	list = icon_view->priv->items;
      else
	list = g_list_last (icon_view->priv->items);

      item = list ? list->data : NULL;
      cell = -1;
    }
  else
    {
      item = icon_view->priv->cursor_item;
      cell = icon_view->priv->cursor_cell;
      step = count > 0 ? 1 : -1;      
      while (item)
	{
	  cell = find_cell (icon_view, item, cell,
			    GTK_ORIENTATION_VERTICAL, 
			    step, &count);
	  if (count == 0)
	    break;

	  item = find_item (icon_view, item, step, 0);
	  count = count - step;
	}
    }

  if (!item)
    {
      if (!gtk_widget_keynav_failed (GTK_WIDGET (icon_view), direction))
        {
          GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (icon_view));
          if (toplevel)
            gtk_widget_child_focus (toplevel,
                                    direction == GTK_DIR_UP ?
                                    GTK_DIR_TAB_BACKWARD :
                                    GTK_DIR_TAB_FORWARD);
        }

      return;
    }

  if (icon_view->priv->modify_selection_pressed ||
      !icon_view->priv->extend_selection_pressed ||
      !icon_view->priv->anchor_item ||
      icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    icon_view->priv->anchor_item = item;

  gtk_icon_view_set_cursor_item (icon_view, item, cell);

  if (!icon_view->priv->modify_selection_pressed &&
      icon_view->priv->selection_mode != GTK_SELECTION_NONE)
    {
      dirty = gtk_icon_view_unselect_all_internal (icon_view);
      dirty = gtk_icon_view_select_all_between (icon_view, 
						icon_view->priv->anchor_item,
						item) || dirty;
    }

  gtk_icon_view_scroll_to_item (icon_view, item);

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

static void 
gtk_icon_view_move_cursor_page_up_down (GtkIconView *icon_view,
					gint         count)
{
  GtkIconViewItem *item;
  gboolean dirty = FALSE;
  
  if (!gtk_widget_has_focus (GTK_WIDGET (icon_view)))
    return;
  
  if (!icon_view->priv->cursor_item)
    {
      GList *list;

      if (count > 0)
	list = icon_view->priv->items;
      else
	list = g_list_last (icon_view->priv->items);

      item = list ? list->data : NULL;
    }
  else
    item = find_item_page_up_down (icon_view, 
				   icon_view->priv->cursor_item,
				   count);

  if (item == icon_view->priv->cursor_item)
    gtk_widget_error_bell (GTK_WIDGET (icon_view));

  if (!item)
    return;

  if (icon_view->priv->modify_selection_pressed ||
      !icon_view->priv->extend_selection_pressed ||
      !icon_view->priv->anchor_item ||
      icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    icon_view->priv->anchor_item = item;

  gtk_icon_view_set_cursor_item (icon_view, item, -1);

  if (!icon_view->priv->modify_selection_pressed &&
      icon_view->priv->selection_mode != GTK_SELECTION_NONE)
    {
      dirty = gtk_icon_view_unselect_all_internal (icon_view);
      dirty = gtk_icon_view_select_all_between (icon_view, 
						icon_view->priv->anchor_item,
						item) || dirty;
    }

  gtk_icon_view_scroll_to_item (icon_view, item);

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);  
}

static void 
gtk_icon_view_move_cursor_left_right (GtkIconView *icon_view,
				      gint         count)
{
  GtkIconViewItem *item;
  gint cell = -1;
  gboolean dirty = FALSE;
  gint step;
  GtkDirectionType direction;

  if (!gtk_widget_has_focus (GTK_WIDGET (icon_view)))
    return;

  direction = count < 0 ? GTK_DIR_LEFT : GTK_DIR_RIGHT;

  if (!icon_view->priv->cursor_item)
    {
      GList *list;

      if (count > 0)
	list = icon_view->priv->items;
      else
	list = g_list_last (icon_view->priv->items);

      item = list ? list->data : NULL;
    }
  else
    {
      item = icon_view->priv->cursor_item;
      cell = icon_view->priv->cursor_cell;
      step = count > 0 ? 1 : -1;
      while (item)
	{
	  cell = find_cell (icon_view, item, cell,
			    GTK_ORIENTATION_HORIZONTAL, 
			    step, &count);
	  if (count == 0)
	    break;
	  
	  item = find_item (icon_view, item, 0, step);
	  count = count - step;
	}
    }

  if (!item)
    {
      if (!gtk_widget_keynav_failed (GTK_WIDGET (icon_view), direction))
        {
          GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (icon_view));
          if (toplevel)
            gtk_widget_child_focus (toplevel,
                                    direction == GTK_DIR_LEFT ?
                                    GTK_DIR_TAB_BACKWARD :
                                    GTK_DIR_TAB_FORWARD);
        }

      return;
    }

  if (icon_view->priv->modify_selection_pressed ||
      !icon_view->priv->extend_selection_pressed ||
      !icon_view->priv->anchor_item ||
      icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    icon_view->priv->anchor_item = item;

  gtk_icon_view_set_cursor_item (icon_view, item, cell);

  if (!icon_view->priv->modify_selection_pressed &&
      icon_view->priv->selection_mode != GTK_SELECTION_NONE)
    {
      dirty = gtk_icon_view_unselect_all_internal (icon_view);
      dirty = gtk_icon_view_select_all_between (icon_view, 
						icon_view->priv->anchor_item,
						item) || dirty;
    }

  gtk_icon_view_scroll_to_item (icon_view, item);

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

static void 
gtk_icon_view_move_cursor_start_end (GtkIconView *icon_view,
				     gint         count)
{
  GtkIconViewItem *item;
  GList *list;
  gboolean dirty = FALSE;
  
  if (!gtk_widget_has_focus (GTK_WIDGET (icon_view)))
    return;
  
  if (count < 0)
    list = icon_view->priv->items;
  else
    list = g_list_last (icon_view->priv->items);
  
  item = list ? list->data : NULL;

  if (item == icon_view->priv->cursor_item)
    gtk_widget_error_bell (GTK_WIDGET (icon_view));

  if (!item)
    return;

  if (icon_view->priv->modify_selection_pressed ||
      !icon_view->priv->extend_selection_pressed ||
      !icon_view->priv->anchor_item ||
      icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    icon_view->priv->anchor_item = item;

  gtk_icon_view_set_cursor_item (icon_view, item, -1);

  if (!icon_view->priv->modify_selection_pressed &&
      icon_view->priv->selection_mode != GTK_SELECTION_NONE)
    {
      dirty = gtk_icon_view_unselect_all_internal (icon_view);
      dirty = gtk_icon_view_select_all_between (icon_view, 
						icon_view->priv->anchor_item,
						item) || dirty;
    }

  gtk_icon_view_scroll_to_item (icon_view, item);

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

/**
 * gtk_icon_view_scroll_to_path:
 * @icon_view: A #GtkIconView.
 * @path: The path of the item to move to.
 * @use_align: whether to use alignment arguments, or %FALSE.
 * @row_align: The vertical alignment of the item specified by @path.
 * @col_align: The horizontal alignment of the item specified by @path.
 *
 * Moves the alignments of @icon_view to the position specified by @path.  
 * @row_align determines where the row is placed, and @col_align determines 
 * where @column is placed.  Both are expected to be between 0.0 and 1.0. 
 * 0.0 means left/top alignment, 1.0 means right/bottom alignment, 0.5 means 
 * center.
 *
 * If @use_align is %FALSE, then the alignment arguments are ignored, and the
 * tree does the minimum amount of work to scroll the item onto the screen.
 * This means that the item will be scrolled to the edge closest to its current
 * position.  If the item is currently visible on the screen, nothing is done.
 *
 * This function only works if the model is set, and @path is a valid row on 
 * the model. If the model changes before the @icon_view is realized, the 
 * centered path will be modified to reflect this change.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_scroll_to_path (GtkIconView *icon_view,
			      GtkTreePath *path,
			      gboolean     use_align,
			      gfloat       row_align,
			      gfloat       col_align)
{
  GtkIconViewItem *item = NULL;

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (path != NULL);
  g_return_if_fail (row_align >= 0.0 && row_align <= 1.0);
  g_return_if_fail (col_align >= 0.0 && col_align <= 1.0);

  if (gtk_tree_path_get_depth (path) > 0)
    item = g_list_nth_data (icon_view->priv->items,
			    gtk_tree_path_get_indices(path)[0]);
  
  if (!item || item->width < 0 ||
      !gtk_widget_get_realized (GTK_WIDGET (icon_view)))
    {
      if (icon_view->priv->scroll_to_path)
	gtk_tree_row_reference_free (icon_view->priv->scroll_to_path);

      icon_view->priv->scroll_to_path = NULL;

      if (path)
	icon_view->priv->scroll_to_path = gtk_tree_row_reference_new_proxy (G_OBJECT (icon_view), icon_view->priv->model, path);

      icon_view->priv->scroll_to_use_align = use_align;
      icon_view->priv->scroll_to_row_align = row_align;
      icon_view->priv->scroll_to_col_align = col_align;

      return;
    }

  if (use_align)
    {
      gint x, y;
      gint focus_width;
      gfloat offset, value;

      gtk_widget_style_get (GTK_WIDGET (icon_view),
			    "focus-line-width", &focus_width,
			    NULL);
      
      gdk_window_get_position (icon_view->priv->bin_window, &x, &y);
      
      offset =  y + item->y - focus_width - 
	row_align * (GTK_WIDGET (icon_view)->allocation.height - item->height);
      value = CLAMP (icon_view->priv->vadjustment->value + offset, 
		     icon_view->priv->vadjustment->lower,
		     icon_view->priv->vadjustment->upper - icon_view->priv->vadjustment->page_size);
      gtk_adjustment_set_value (icon_view->priv->vadjustment, value);

      offset = x + item->x - focus_width - 
	col_align * (GTK_WIDGET (icon_view)->allocation.width - item->width);
      value = CLAMP (icon_view->priv->hadjustment->value + offset, 
		     icon_view->priv->hadjustment->lower,
		     icon_view->priv->hadjustment->upper - icon_view->priv->hadjustment->page_size);
      gtk_adjustment_set_value (icon_view->priv->hadjustment, value);

      gtk_adjustment_changed (icon_view->priv->hadjustment);
      gtk_adjustment_changed (icon_view->priv->vadjustment);
    }
  else
    gtk_icon_view_scroll_to_item (icon_view, item);
}


static void     
gtk_icon_view_scroll_to_item (GtkIconView     *icon_view, 
			      GtkIconViewItem *item)
{
  gint x, y;
  gint focus_width;

  gtk_widget_style_get (GTK_WIDGET (icon_view),
			"focus-line-width", &focus_width,
			NULL);

  gdk_window_get_position (icon_view->priv->bin_window, &x, &y);
  
  if (y + item->y - focus_width < 0)
    gtk_adjustment_set_value (icon_view->priv->vadjustment, 
			      icon_view->priv->vadjustment->value + y + item->y - focus_width);
  else if (y + item->y + item->height + focus_width > GTK_WIDGET (icon_view)->allocation.height)
    gtk_adjustment_set_value (icon_view->priv->vadjustment, 
			      icon_view->priv->vadjustment->value + y + item->y + item->height 
			      + focus_width - GTK_WIDGET (icon_view)->allocation.height);

  if (x + item->x - focus_width < 0)
    gtk_adjustment_set_value (icon_view->priv->hadjustment, 
			      icon_view->priv->hadjustment->value + x + item->x - focus_width);
  else if (x + item->x + item->width + focus_width > GTK_WIDGET (icon_view)->allocation.width)
    gtk_adjustment_set_value (icon_view->priv->hadjustment, 
			      icon_view->priv->hadjustment->value + x + item->x + item->width 
			      + focus_width - GTK_WIDGET (icon_view)->allocation.width);

  gtk_adjustment_changed (icon_view->priv->hadjustment);
  gtk_adjustment_changed (icon_view->priv->vadjustment);
}

/* GtkCellLayout implementation */
static GtkIconViewCellInfo *
gtk_icon_view_get_cell_info (GtkIconView     *icon_view,
                             GtkCellRenderer *renderer)
{
  GList *i;

  for (i = icon_view->priv->cell_list; i; i = i->next)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)i->data;

      if (info->cell == renderer)
        return info;
    }

  return NULL;
}

static void
gtk_icon_view_set_cell_data (GtkIconView     *icon_view, 
			     GtkIconViewItem *item)
{
  GList *i;
  gboolean iters_persist;
  GtkTreeIter iter;
  
  iters_persist = gtk_tree_model_get_flags (icon_view->priv->model) & GTK_TREE_MODEL_ITERS_PERSIST;
  
  if (!iters_persist)
    {
      GtkTreePath *path;

      path = gtk_tree_path_new_from_indices (item->index, -1);
      if (!gtk_tree_model_get_iter (icon_view->priv->model, &iter, path))
        return;
      gtk_tree_path_free (path);
    }
  else
    iter = item->iter;
  
  for (i = icon_view->priv->cell_list; i; i = i->next)
    {
      GSList *j;
      GtkIconViewCellInfo *info = i->data;

      g_object_freeze_notify (G_OBJECT (info->cell));

      for (j = info->attributes; j && j->next; j = j->next->next)
        {
          gchar *property = j->data;
          gint column = GPOINTER_TO_INT (j->next->data);
          GValue value = {0, };

          gtk_tree_model_get_value (icon_view->priv->model, &iter,
                                    column, &value);
          g_object_set_property (G_OBJECT (info->cell),
                                 property, &value);
          g_value_unset (&value);
        }

      if (info->func)
	(* info->func) (GTK_CELL_LAYOUT (icon_view),
			info->cell,
			icon_view->priv->model,
			&iter,
			info->func_data);
      
      g_object_thaw_notify (G_OBJECT (info->cell));
    }  
}

static void 
free_cell_attributes (GtkIconViewCellInfo *info)
{ 
  GSList *list;

  list = info->attributes;
  while (list && list->next)
    {
      g_free (list->data);
      list = list->next->next;
    }
  
  g_slist_free (info->attributes);
  info->attributes = NULL;
}

static void
free_cell_info (GtkIconViewCellInfo *info)
{
  free_cell_attributes (info);

  g_object_unref (info->cell);
  
  if (info->destroy)
    (* info->destroy) (info->func_data);

  g_free (info);
}

static void
gtk_icon_view_cell_layout_pack_start (GtkCellLayout   *layout,
                                      GtkCellRenderer *renderer,
                                      gboolean         expand)
{
  GtkIconViewCellInfo *info;
  GtkIconView *icon_view = GTK_ICON_VIEW (layout);

  g_return_if_fail (GTK_IS_CELL_RENDERER (renderer));
  g_return_if_fail (!gtk_icon_view_get_cell_info (icon_view, renderer));

  g_object_ref_sink (renderer);

  info = g_new0 (GtkIconViewCellInfo, 1);
  info->cell = renderer;
  info->expand = expand ? TRUE : FALSE;
  info->pack = GTK_PACK_START;
  info->position = icon_view->priv->n_cells;
  
  icon_view->priv->cell_list = g_list_append (icon_view->priv->cell_list, info);
  icon_view->priv->n_cells++;
}

static void
gtk_icon_view_cell_layout_pack_end (GtkCellLayout   *layout,
                                    GtkCellRenderer *renderer,
                                    gboolean         expand)
{
  GtkIconViewCellInfo *info;
  GtkIconView *icon_view = GTK_ICON_VIEW (layout);

  g_return_if_fail (GTK_IS_CELL_RENDERER (renderer));
  g_return_if_fail (!gtk_icon_view_get_cell_info (icon_view, renderer));

  g_object_ref_sink (renderer);

  info = g_new0 (GtkIconViewCellInfo, 1);
  info->cell = renderer;
  info->expand = expand ? TRUE : FALSE;
  info->pack = GTK_PACK_END;
  info->position = icon_view->priv->n_cells;

  icon_view->priv->cell_list = g_list_append (icon_view->priv->cell_list, info);
  icon_view->priv->n_cells++;
}

static void
gtk_icon_view_cell_layout_add_attribute (GtkCellLayout   *layout,
                                         GtkCellRenderer *renderer,
                                         const gchar     *attribute,
                                         gint             column)
{
  GtkIconViewCellInfo *info;
  GtkIconView *icon_view = GTK_ICON_VIEW (layout);

  info = gtk_icon_view_get_cell_info (icon_view, renderer);
  g_return_if_fail (info != NULL);

  info->attributes = g_slist_prepend (info->attributes,
                                      GINT_TO_POINTER (column));
  info->attributes = g_slist_prepend (info->attributes,
                                      g_strdup (attribute));
}

static void
gtk_icon_view_cell_layout_clear (GtkCellLayout *layout)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (layout);

  while (icon_view->priv->cell_list)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)icon_view->priv->cell_list->data;
      free_cell_info (info);
      icon_view->priv->cell_list = g_list_delete_link (icon_view->priv->cell_list, 
						       icon_view->priv->cell_list);
    }

  icon_view->priv->n_cells = 0;
}

static void
gtk_icon_view_cell_layout_set_cell_data_func (GtkCellLayout         *layout,
                                              GtkCellRenderer       *cell,
                                              GtkCellLayoutDataFunc  func,
                                              gpointer               func_data,
                                              GDestroyNotify         destroy)
{
  GtkIconViewCellInfo *info;
  GtkIconView *icon_view = GTK_ICON_VIEW (layout);

  info = gtk_icon_view_get_cell_info (icon_view, cell);
  g_return_if_fail (info != NULL);

  if (info->destroy)
    {
      GDestroyNotify d = info->destroy;

      info->destroy = NULL;
      d (info->func_data);
    }

  info->func = func;
  info->func_data = func_data;
  info->destroy = destroy;
}

static void
gtk_icon_view_cell_layout_clear_attributes (GtkCellLayout   *layout,
                                            GtkCellRenderer *renderer)
{
  GtkIconViewCellInfo *info;

  info = gtk_icon_view_get_cell_info (GTK_ICON_VIEW (layout), renderer);
  if (info != NULL)
    free_cell_attributes (info);
}

static void
gtk_icon_view_cell_layout_reorder (GtkCellLayout   *layout,
                                   GtkCellRenderer *cell,
                                   gint             position)
{
  GtkIconView *icon_view;
  GList *link, *l;
  GtkIconViewCellInfo *info;
  gint i;

  icon_view = GTK_ICON_VIEW (layout);

  g_return_if_fail (GTK_IS_CELL_RENDERER (cell));

  info = gtk_icon_view_get_cell_info (icon_view, cell);

  g_return_if_fail (info != NULL);
  g_return_if_fail (position >= 0);

  link = g_list_find (icon_view->priv->cell_list, info);

  g_return_if_fail (link != NULL);

  icon_view->priv->cell_list = g_list_delete_link (icon_view->priv->cell_list,
                                                   link);
  icon_view->priv->cell_list = g_list_insert (icon_view->priv->cell_list,
                                             info, position);

  for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
    {
      info = l->data;

      info->position = i;
    }

  gtk_widget_queue_draw (GTK_WIDGET (icon_view));
}

static GList *
gtk_icon_view_cell_layout_get_cells (GtkCellLayout *layout)
{
  GtkIconView *icon_view = (GtkIconView *)layout;
  GList *retval = NULL, *l;

  for (l = icon_view->priv->cell_list; l; l = l->next)
    {
      GtkIconViewCellInfo *info = (GtkIconViewCellInfo *)l->data;

      retval = g_list_prepend (retval, info->cell);
    }

  return g_list_reverse (retval);
}

/* Public API */


/**
 * gtk_icon_view_new:
 * 
 * Creates a new #GtkIconView widget
 * 
 * Return value: A newly created #GtkIconView widget
 *
 * Since: 2.6
 **/
GtkWidget *
gtk_icon_view_new (void)
{
  return g_object_new (GTK_TYPE_ICON_VIEW, NULL);
}

/**
 * gtk_icon_view_new_with_model:
 * @model: The model.
 * 
 * Creates a new #GtkIconView widget with the model @model.
 * 
 * Return value: A newly created #GtkIconView widget.
 *
 * Since: 2.6 
 **/
GtkWidget *
gtk_icon_view_new_with_model (GtkTreeModel *model)
{
  return g_object_new (GTK_TYPE_ICON_VIEW, "model", model, NULL);
}

/**
 * gtk_icon_view_convert_widget_to_bin_window_coords:
 * @icon_view: a #GtkIconView 
 * @wx: X coordinate relative to the widget
 * @wy: Y coordinate relative to the widget
 * @bx: (out): return location for bin_window X coordinate
 * @by: (out): return location for bin_window Y coordinate
 * 
 * Converts widget coordinates to coordinates for the bin_window,
 * as expected by e.g. gtk_icon_view_get_path_at_pos(). 
 *
 * Since: 2.12
 */
void
gtk_icon_view_convert_widget_to_bin_window_coords (GtkIconView *icon_view,
                                                   gint         wx,
                                                   gint         wy, 
                                                   gint        *bx,
                                                   gint        *by)
{
  gint x, y;

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->bin_window) 
    gdk_window_get_position (icon_view->priv->bin_window, &x, &y);
  else
    x = y = 0;
 
  if (bx)
    *bx = wx - x;
  if (by)
    *by = wy - y;
}

/**
 * gtk_icon_view_get_path_at_pos:
 * @icon_view: A #GtkIconView.
 * @x: The x position to be identified
 * @y: The y position to be identified
 * 
 * Finds the path at the point (@x, @y), relative to bin_window coordinates.
 * See gtk_icon_view_get_item_at_pos(), if you are also interested in
 * the cell at the specified position. 
 * See gtk_icon_view_convert_widget_to_bin_window_coords() for converting
 * widget coordinates to bin_window coordinates.
 * 
 * Return value: The #GtkTreePath corresponding to the icon or %NULL
 * if no icon exists at that position.
 *
 * Since: 2.6 
 **/
GtkTreePath *
gtk_icon_view_get_path_at_pos (GtkIconView *icon_view,
			       gint         x,
			       gint         y)
{
  GtkIconViewItem *item;
  GtkTreePath *path;
  
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), NULL);

  item = gtk_icon_view_get_item_at_coords (icon_view, x, y, TRUE, NULL);

  if (!item)
    return NULL;

  path = gtk_tree_path_new_from_indices (item->index, -1);

  return path;
}

/**
 * gtk_icon_view_get_item_at_pos:
 * @icon_view: A #GtkIconView.
 * @x: The x position to be identified
 * @y: The y position to be identified
 * @path: (allow-none): Return location for the path, or %NULL
 * @cell: Return location for the renderer responsible for the cell
 *   at (@x, @y), or %NULL
 * 
 * Finds the path at the point (@x, @y), relative to bin_window coordinates.
 * In contrast to gtk_icon_view_get_path_at_pos(), this function also 
 * obtains the cell at the specified position. The returned path should
 * be freed with gtk_tree_path_free().
 * See gtk_icon_view_convert_widget_to_bin_window_coords() for converting
 * widget coordinates to bin_window coordinates.
 * 
 * Return value: %TRUE if an item exists at the specified position
 *
 * Since: 2.8
 **/
gboolean 
gtk_icon_view_get_item_at_pos (GtkIconView      *icon_view,
			       gint              x,
			       gint              y,
			       GtkTreePath     **path,
			       GtkCellRenderer **cell)
{
  GtkIconViewItem *item;
  GtkIconViewCellInfo *info;
  
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);

  item = gtk_icon_view_get_item_at_coords (icon_view, x, y, TRUE, &info);

  if (path != NULL)
    {
      if (item != NULL)
	*path = gtk_tree_path_new_from_indices (item->index, -1);
      else
	*path = NULL;
    }

  if (cell != NULL)
    {
      if (info != NULL)
	*cell = info->cell;
      else 
	*cell = NULL;
    }

  return (item != NULL);
}

/**
 * gtk_icon_view_set_tooltip_item:
 * @icon_view: a #GtkIconView
 * @tooltip: a #GtkTooltip
 * @path: a #GtkTreePath
 * 
 * Sets the tip area of @tooltip to be the area covered by the item at @path.
 * See also gtk_icon_view_set_tooltip_column() for a simpler alternative.
 * See also gtk_tooltip_set_tip_area().
 * 
 * Since: 2.12
 */
void 
gtk_icon_view_set_tooltip_item (GtkIconView     *icon_view,
                                GtkTooltip      *tooltip,
                                GtkTreePath     *path)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (GTK_IS_TOOLTIP (tooltip));

  gtk_icon_view_set_tooltip_cell (icon_view, tooltip, path, NULL);
}

/**
 * gtk_icon_view_set_tooltip_cell:
 * @icon_view: a #GtkIconView
 * @tooltip: a #GtkTooltip
 * @path: a #GtkTreePath
 * @cell: (allow-none): a #GtkCellRenderer or %NULL
 *
 * Sets the tip area of @tooltip to the area which @cell occupies in
 * the item pointed to by @path. See also gtk_tooltip_set_tip_area().
 *
 * See also gtk_icon_view_set_tooltip_column() for a simpler alternative.
 *
 * Since: 2.12
 */
void
gtk_icon_view_set_tooltip_cell (GtkIconView     *icon_view,
                                GtkTooltip      *tooltip,
                                GtkTreePath     *path,
                                GtkCellRenderer *cell)
{
  GdkRectangle rect;
  GtkIconViewItem *item = NULL;
  GtkIconViewCellInfo *info = NULL;
  gint x, y;
 
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (GTK_IS_TOOLTIP (tooltip));
  g_return_if_fail (cell == NULL || GTK_IS_CELL_RENDERER (cell));

  if (gtk_tree_path_get_depth (path) > 0)
    item = g_list_nth_data (icon_view->priv->items,
                            gtk_tree_path_get_indices(path)[0]);
 
  if (!item)
    return;

  if (cell)
    {
      info = gtk_icon_view_get_cell_info (icon_view, cell);
      gtk_icon_view_get_cell_area (icon_view, item, info, &rect);
    }
  else
    {
      rect.x = item->x;
      rect.y = item->y;
      rect.width = item->width;
      rect.height = item->height;
    }
  
  if (icon_view->priv->bin_window)
    {
      gdk_window_get_position (icon_view->priv->bin_window, &x, &y);
      rect.x += x;
      rect.y += y; 
    }

  gtk_tooltip_set_tip_area (tooltip, &rect); 
}


/**
 * gtk_icon_view_get_tooltip_context:
 * @icon_view: an #GtkIconView
 * @x: (inout): the x coordinate (relative to widget coordinates)
 * @y: (inout): the y coordinate (relative to widget coordinates)
 * @keyboard_tip: whether this is a keyboard tooltip or not
 * @model: (out) (allow-none): a pointer to receive a #GtkTreeModel or %NULL
 * @path: (out) (allow-none): a pointer to receive a #GtkTreePath or %NULL
 * @iter: (out) (allow-none): a pointer to receive a #GtkTreeIter or %NULL
 *
 * This function is supposed to be used in a #GtkWidget::query-tooltip
 * signal handler for #GtkIconView.  The @x, @y and @keyboard_tip values
 * which are received in the signal handler, should be passed to this
 * function without modification.
 *
 * The return value indicates whether there is an icon view item at the given
 * coordinates (%TRUE) or not (%FALSE) for mouse tooltips. For keyboard
 * tooltips the item returned will be the cursor item. When %TRUE, then any of
 * @model, @path and @iter which have been provided will be set to point to
 * that row and the corresponding model. @x and @y will always be converted
 * to be relative to @icon_view's bin_window if @keyboard_tooltip is %FALSE.
 *
 * Return value: whether or not the given tooltip context points to a item
 *
 * Since: 2.12
 */
gboolean
gtk_icon_view_get_tooltip_context (GtkIconView   *icon_view,
                                   gint          *x,
                                   gint          *y,
                                   gboolean       keyboard_tip,
                                   GtkTreeModel **model,
                                   GtkTreePath  **path,
                                   GtkTreeIter   *iter)
{
  GtkTreePath *tmppath = NULL;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (x != NULL, FALSE);
  g_return_val_if_fail (y != NULL, FALSE);

  if (keyboard_tip)
    {
      gtk_icon_view_get_cursor (icon_view, &tmppath, NULL);

      if (!tmppath)
        return FALSE;
    }
  else
    {
      gtk_icon_view_convert_widget_to_bin_window_coords (icon_view, *x, *y,
                                                         x, y);

      if (!gtk_icon_view_get_item_at_pos (icon_view, *x, *y, &tmppath, NULL))
        return FALSE;
    }

  if (model)
    *model = gtk_icon_view_get_model (icon_view);

  if (iter)
    gtk_tree_model_get_iter (gtk_icon_view_get_model (icon_view),
                             iter, tmppath);

  if (path)
    *path = tmppath;
  else
    gtk_tree_path_free (tmppath);

  return TRUE;
}

static gboolean
gtk_icon_view_set_tooltip_query_cb (GtkWidget  *widget,
                                    gint        x,
                                    gint        y,
                                    gboolean    keyboard_tip,
                                    GtkTooltip *tooltip,
                                    gpointer    data)
{
  gchar *str;
  GtkTreeIter iter;
  GtkTreePath *path;
  GtkTreeModel *model;
  GtkIconView *icon_view = GTK_ICON_VIEW (widget);

  if (!gtk_icon_view_get_tooltip_context (GTK_ICON_VIEW (widget),
                                          &x, &y,
                                          keyboard_tip,
                                          &model, &path, &iter))
    return FALSE;

  gtk_tree_model_get (model, &iter, icon_view->priv->tooltip_column, &str, -1);

  if (!str)
    {
      gtk_tree_path_free (path);
      return FALSE;
    }

  gtk_tooltip_set_markup (tooltip, str);
  gtk_icon_view_set_tooltip_item (icon_view, tooltip, path);

  gtk_tree_path_free (path);
  g_free (str);

  return TRUE;
}


/**
 * gtk_icon_view_set_tooltip_column:
 * @icon_view: a #GtkIconView
 * @column: an integer, which is a valid column number for @icon_view's model
 *
 * If you only plan to have simple (text-only) tooltips on full items, you
 * can use this function to have #GtkIconView handle these automatically
 * for you. @column should be set to the column in @icon_view's model
 * containing the tooltip texts, or -1 to disable this feature.
 *
 * When enabled, #GtkWidget::has-tooltip will be set to %TRUE and
 * @icon_view will connect a #GtkWidget::query-tooltip signal handler.
 *
 * Since: 2.12
 */
void
gtk_icon_view_set_tooltip_column (GtkIconView *icon_view,
                                  gint         column)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (column == icon_view->priv->tooltip_column)
    return;

  if (column == -1)
    {
      g_signal_handlers_disconnect_by_func (icon_view,
                                            gtk_icon_view_set_tooltip_query_cb,
                                            NULL);
      gtk_widget_set_has_tooltip (GTK_WIDGET (icon_view), FALSE);
    }
  else
    {
      if (icon_view->priv->tooltip_column == -1)
        {
          g_signal_connect (icon_view, "query-tooltip",
                            G_CALLBACK (gtk_icon_view_set_tooltip_query_cb), NULL);
          gtk_widget_set_has_tooltip (GTK_WIDGET (icon_view), TRUE);
        }
    }

  icon_view->priv->tooltip_column = column;
  g_object_notify (G_OBJECT (icon_view), "tooltip-column");
}

/** 
 * gtk_icon_view_get_tooltip_column:
 * @icon_view: a #GtkIconView
 *
 * Returns the column of @icon_view's model which is being used for
 * displaying tooltips on @icon_view's rows.
 *
 * Return value: the index of the tooltip column that is currently being
 * used, or -1 if this is disabled.
 *
 * Since: 2.12
 */
gint
gtk_icon_view_get_tooltip_column (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), 0);

  return icon_view->priv->tooltip_column;
}

/**
 * gtk_icon_view_get_visible_range:
 * @icon_view: A #GtkIconView
 * @start_path: (allow-none): Return location for start of region, or %NULL
 * @end_path: (allow-none): Return location for end of region, or %NULL
 * 
 * Sets @start_path and @end_path to be the first and last visible path. 
 * Note that there may be invisible paths in between.
 * 
 * Both paths should be freed with gtk_tree_path_free() after use.
 * 
 * Return value: %TRUE, if valid paths were placed in @start_path and @end_path
 *
 * Since: 2.8
 **/
gboolean
gtk_icon_view_get_visible_range (GtkIconView  *icon_view,
				 GtkTreePath **start_path,
				 GtkTreePath **end_path)
{
  gint start_index = -1;
  gint end_index = -1;
  GList *icons;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);

  if (icon_view->priv->hadjustment == NULL ||
      icon_view->priv->vadjustment == NULL)
    return FALSE;

  if (start_path == NULL && end_path == NULL)
    return FALSE;
  
  for (icons = icon_view->priv->items; icons; icons = icons->next) 
    {
      GtkIconViewItem *item = icons->data;

      if ((item->x + item->width >= (int)icon_view->priv->hadjustment->value) &&
	  (item->y + item->height >= (int)icon_view->priv->vadjustment->value) &&
	  (item->x <= (int) (icon_view->priv->hadjustment->value + icon_view->priv->hadjustment->page_size)) &&
	  (item->y <= (int) (icon_view->priv->vadjustment->value + icon_view->priv->vadjustment->page_size)))
	{
	  if (start_index == -1)
	    start_index = item->index;
	  end_index = item->index;
	}
    }

  if (start_path && start_index != -1)
    *start_path = gtk_tree_path_new_from_indices (start_index, -1);
  if (end_path && end_index != -1)
    *end_path = gtk_tree_path_new_from_indices (end_index, -1);
  
  return start_index != -1;
}

/**
 * gtk_icon_view_selected_foreach:
 * @icon_view: A #GtkIconView.
 * @func: (scope call): The function to call for each selected icon.
 * @data: User data to pass to the function.
 * 
 * Calls a function for each selected icon. Note that the model or
 * selection cannot be modified from within this function.
 *
 * Since: 2.6 
 **/
void
gtk_icon_view_selected_foreach (GtkIconView           *icon_view,
				GtkIconViewForeachFunc func,
				gpointer               data)
{
  GList *list;
  
  for (list = icon_view->priv->items; list; list = list->next)
    {
      GtkIconViewItem *item = list->data;
      GtkTreePath *path = gtk_tree_path_new_from_indices (item->index, -1);

      if (item->selected)
	(* func) (icon_view, path, data);

      gtk_tree_path_free (path);
    }
}

/**
 * gtk_icon_view_set_selection_mode:
 * @icon_view: A #GtkIconView.
 * @mode: The selection mode
 * 
 * Sets the selection mode of the @icon_view.
 *
 * Since: 2.6 
 **/
void
gtk_icon_view_set_selection_mode (GtkIconView      *icon_view,
				  GtkSelectionMode  mode)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (mode == icon_view->priv->selection_mode)
    return;
  
  if (mode == GTK_SELECTION_NONE ||
      icon_view->priv->selection_mode == GTK_SELECTION_MULTIPLE)
    gtk_icon_view_unselect_all (icon_view);
  
  icon_view->priv->selection_mode = mode;

  g_object_notify (G_OBJECT (icon_view), "selection-mode");
}

/**
 * gtk_icon_view_get_selection_mode:
 * @icon_view: A #GtkIconView.
 * 
 * Gets the selection mode of the @icon_view.
 *
 * Return value: the current selection mode
 *
 * Since: 2.6 
 **/
GtkSelectionMode
gtk_icon_view_get_selection_mode (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), GTK_SELECTION_SINGLE);

  return icon_view->priv->selection_mode;
}

/**
 * gtk_icon_view_set_model:
 * @icon_view: A #GtkIconView.
 * @model: (allow-none): The model.
 *
 * Sets the model for a #GtkIconView.
 * If the @icon_view already has a model set, it will remove
 * it before setting the new model.  If @model is %NULL, then
 * it will unset the old model.
 *
 * Since: 2.6 
 **/
void
gtk_icon_view_set_model (GtkIconView *icon_view,
			 GtkTreeModel *model)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (model == NULL || GTK_IS_TREE_MODEL (model));
  
  if (icon_view->priv->model == model)
    return;

  if (icon_view->priv->scroll_to_path)
    {
      gtk_tree_row_reference_free (icon_view->priv->scroll_to_path);
      icon_view->priv->scroll_to_path = NULL;
    }

  gtk_icon_view_stop_editing (icon_view, TRUE);

  if (model)
    {
      GType column_type;
      
      g_return_if_fail (gtk_tree_model_get_flags (model) & GTK_TREE_MODEL_LIST_ONLY);

      if (icon_view->priv->pixbuf_column != -1)
	{
	  column_type = gtk_tree_model_get_column_type (model,
							icon_view->priv->pixbuf_column);	  

	  g_return_if_fail (column_type == GDK_TYPE_PIXBUF);
	}

      if (icon_view->priv->text_column != -1)
	{
	  column_type = gtk_tree_model_get_column_type (model,
							icon_view->priv->text_column);	  

	  g_return_if_fail (column_type == G_TYPE_STRING);
	}

      if (icon_view->priv->markup_column != -1)
	{
	  column_type = gtk_tree_model_get_column_type (model,
							icon_view->priv->markup_column);	  

	  g_return_if_fail (column_type == G_TYPE_STRING);
	}
      
    }
  
  if (icon_view->priv->model)
    {
      g_signal_handlers_disconnect_by_func (icon_view->priv->model,
					    gtk_icon_view_row_changed,
					    icon_view);
      g_signal_handlers_disconnect_by_func (icon_view->priv->model,
					    gtk_icon_view_row_inserted,
					    icon_view);
      g_signal_handlers_disconnect_by_func (icon_view->priv->model,
					    gtk_icon_view_row_deleted,
					    icon_view);
      g_signal_handlers_disconnect_by_func (icon_view->priv->model,
					    gtk_icon_view_rows_reordered,
					    icon_view);

      g_object_unref (icon_view->priv->model);
      
      g_list_foreach (icon_view->priv->items, (GFunc)gtk_icon_view_item_free, NULL);
      g_list_free (icon_view->priv->items);
      icon_view->priv->items = NULL;
      icon_view->priv->anchor_item = NULL;
      icon_view->priv->cursor_item = NULL;
      icon_view->priv->last_single_clicked = NULL;
      icon_view->priv->width = 0;
      icon_view->priv->height = 0;
    }

  icon_view->priv->model = model;

  if (icon_view->priv->model)
    {
      g_object_ref (icon_view->priv->model);
      g_signal_connect (icon_view->priv->model,
			"row-changed",
			G_CALLBACK (gtk_icon_view_row_changed),
			icon_view);
      g_signal_connect (icon_view->priv->model,
			"row-inserted",
			G_CALLBACK (gtk_icon_view_row_inserted),
			icon_view);
      g_signal_connect (icon_view->priv->model,
			"row-deleted",
			G_CALLBACK (gtk_icon_view_row_deleted),
			icon_view);
      g_signal_connect (icon_view->priv->model,
			"rows-reordered",
			G_CALLBACK (gtk_icon_view_rows_reordered),
			icon_view);

      gtk_icon_view_build_items (icon_view);

      gtk_icon_view_queue_layout (icon_view);
    }

  g_object_notify (G_OBJECT (icon_view), "model");  

  if (gtk_widget_get_realized (GTK_WIDGET (icon_view)))
    gtk_widget_queue_resize (GTK_WIDGET (icon_view));
}

/**
 * gtk_icon_view_get_model:
 * @icon_view: a #GtkIconView
 *
 * Returns the model the #GtkIconView is based on.  Returns %NULL if the
 * model is unset.
 *
 * Return value: (transfer none): A #GtkTreeModel, or %NULL if none is
 *     currently being used.
 *
 * Since: 2.6 
 **/
GtkTreeModel *
gtk_icon_view_get_model (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), NULL);

  return icon_view->priv->model;
}

static void
update_text_cell (GtkIconView *icon_view)
{
  GtkIconViewCellInfo *info;
  GList *l;
  gint i;
	  
  if (icon_view->priv->text_column == -1 &&
      icon_view->priv->markup_column == -1)
    {
      if (icon_view->priv->text_cell != -1)
	{
	  if (icon_view->priv->pixbuf_cell > icon_view->priv->text_cell)
	    icon_view->priv->pixbuf_cell--;

	  info = g_list_nth_data (icon_view->priv->cell_list, 
				  icon_view->priv->text_cell);
	  
	  icon_view->priv->cell_list = g_list_remove (icon_view->priv->cell_list, info);
	  
	  free_cell_info (info);
	  
	  icon_view->priv->n_cells--;
	  icon_view->priv->text_cell = -1;
	}
    }
  else 
    {
      if (icon_view->priv->text_cell == -1)
	{
	  GtkCellRenderer *cell = gtk_cell_renderer_text_new ();
	  gtk_cell_layout_pack_end (GTK_CELL_LAYOUT (icon_view), cell, FALSE);
	  for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
	    {
	      info = l->data;
	      if (info->cell == cell)
		{
		  icon_view->priv->text_cell = i;
		  break;
		}
	    }
	}
      
      info = g_list_nth_data (icon_view->priv->cell_list,
			      icon_view->priv->text_cell);

      if (icon_view->priv->markup_column != -1)
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (icon_view),
					info->cell, 
					"markup", icon_view->priv->markup_column, 
					NULL);
      else
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (icon_view),
					info->cell, 
					"text", icon_view->priv->text_column, 
					NULL);

      if (icon_view->priv->item_orientation == GTK_ORIENTATION_VERTICAL)
	g_object_set (info->cell,
                      "alignment", PANGO_ALIGN_CENTER,
		      "wrap-mode", PANGO_WRAP_WORD_CHAR,
		      "xalign", 0.0,
		      "yalign", 0.0,
		      NULL);
      else
	g_object_set (info->cell,
                      "alignment", PANGO_ALIGN_LEFT,
		      "wrap-mode", PANGO_WRAP_WORD_CHAR,
		      "xalign", 0.0,
		      "yalign", 0.0,
		      NULL);
    }
}

static void
update_pixbuf_cell (GtkIconView *icon_view)
{
  GtkIconViewCellInfo *info;
  GList *l;
  gint i;

  if (icon_view->priv->pixbuf_column == -1)
    {
      if (icon_view->priv->pixbuf_cell != -1)
	{
	  if (icon_view->priv->text_cell > icon_view->priv->pixbuf_cell)
	    icon_view->priv->text_cell--;

	  info = g_list_nth_data (icon_view->priv->cell_list, 
				  icon_view->priv->pixbuf_cell);
	  
	  icon_view->priv->cell_list = g_list_remove (icon_view->priv->cell_list, info);
	  
	  free_cell_info (info);
	  
	  icon_view->priv->n_cells--;
	  icon_view->priv->pixbuf_cell = -1;
	}
    }
  else 
    {
      if (icon_view->priv->pixbuf_cell == -1)
	{
	  GtkCellRenderer *cell = gtk_cell_renderer_pixbuf_new ();
	  
	  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (icon_view), cell, FALSE);
	  for (l = icon_view->priv->cell_list, i = 0; l; l = l->next, i++)
	    {
	      info = l->data;
	      if (info->cell == cell)
		{
		  icon_view->priv->pixbuf_cell = i;
		  break;
		}
	    }
	}
      
	info = g_list_nth_data (icon_view->priv->cell_list, 
				icon_view->priv->pixbuf_cell);
	
	gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (icon_view),
					info->cell, 
					"pixbuf", icon_view->priv->pixbuf_column, 
					NULL);

	if (icon_view->priv->item_orientation == GTK_ORIENTATION_VERTICAL)
	  g_object_set (info->cell,
			"xalign", 0.5,
			"yalign", 1.0,
			NULL);
	else
	  g_object_set (info->cell,
			"xalign", 0.0,
			"yalign", 0.0,
			NULL);
    }
}

/**
 * gtk_icon_view_set_text_column:
 * @icon_view: A #GtkIconView.
 * @column: A column in the currently used model, or -1 to display no text
 * 
 * Sets the column with text for @icon_view to be @column. The text
 * column must be of type #G_TYPE_STRING.
 *
 * Since: 2.6 
 **/
void
gtk_icon_view_set_text_column (GtkIconView *icon_view,
			       gint          column)
{
  if (column == icon_view->priv->text_column)
    return;
  
  if (column == -1)
    icon_view->priv->text_column = -1;
  else
    {
      if (icon_view->priv->model != NULL)
	{
	  GType column_type;
	  
	  column_type = gtk_tree_model_get_column_type (icon_view->priv->model, column);

	  g_return_if_fail (column_type == G_TYPE_STRING);
	}
      
      icon_view->priv->text_column = column;
    }

  gtk_icon_view_stop_editing (icon_view, TRUE);

  update_text_cell (icon_view);

  gtk_icon_view_invalidate_sizes (icon_view);
  gtk_icon_view_queue_layout (icon_view);
  
  g_object_notify (G_OBJECT (icon_view), "text-column");
}

/**
 * gtk_icon_view_get_text_column:
 * @icon_view: A #GtkIconView.
 *
 * Returns the column with text for @icon_view.
 *
 * Returns: the text column, or -1 if it's unset.
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_text_column (GtkIconView  *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->text_column;
}

/**
 * gtk_icon_view_set_markup_column:
 * @icon_view: A #GtkIconView.
 * @column: A column in the currently used model, or -1 to display no text
 * 
 * Sets the column with markup information for @icon_view to be
 * @column. The markup column must be of type #G_TYPE_STRING.
 * If the markup column is set to something, it overrides
 * the text column set by gtk_icon_view_set_text_column().
 *
 * Since: 2.6
 **/
void
gtk_icon_view_set_markup_column (GtkIconView *icon_view,
				 gint         column)
{
  if (column == icon_view->priv->markup_column)
    return;
  
  if (column == -1)
    icon_view->priv->markup_column = -1;
  else
    {
      if (icon_view->priv->model != NULL)
	{
	  GType column_type;
	  
	  column_type = gtk_tree_model_get_column_type (icon_view->priv->model, column);

	  g_return_if_fail (column_type == G_TYPE_STRING);
	}
      
      icon_view->priv->markup_column = column;
    }

  gtk_icon_view_stop_editing (icon_view, TRUE);

  update_text_cell (icon_view);

  gtk_icon_view_invalidate_sizes (icon_view);
  gtk_icon_view_queue_layout (icon_view);
  
  g_object_notify (G_OBJECT (icon_view), "markup-column");
}

/**
 * gtk_icon_view_get_markup_column:
 * @icon_view: A #GtkIconView.
 *
 * Returns the column with markup text for @icon_view.
 *
 * Returns: the markup column, or -1 if it's unset.
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_markup_column (GtkIconView  *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->markup_column;
}

/**
 * gtk_icon_view_set_pixbuf_column:
 * @icon_view: A #GtkIconView.
 * @column: A column in the currently used model, or -1 to disable
 * 
 * Sets the column with pixbufs for @icon_view to be @column. The pixbuf
 * column must be of type #GDK_TYPE_PIXBUF
 *
 * Since: 2.6 
 **/
void
gtk_icon_view_set_pixbuf_column (GtkIconView *icon_view,
				 gint         column)
{
  if (column == icon_view->priv->pixbuf_column)
    return;
  
  if (column == -1)
    icon_view->priv->pixbuf_column = -1;
  else
    {
      if (icon_view->priv->model != NULL)
	{
	  GType column_type;
	  
	  column_type = gtk_tree_model_get_column_type (icon_view->priv->model, column);

	  g_return_if_fail (column_type == GDK_TYPE_PIXBUF);
	}
      
      icon_view->priv->pixbuf_column = column;
    }

  gtk_icon_view_stop_editing (icon_view, TRUE);

  update_pixbuf_cell (icon_view);

  gtk_icon_view_invalidate_sizes (icon_view);
  gtk_icon_view_queue_layout (icon_view);
  
  g_object_notify (G_OBJECT (icon_view), "pixbuf-column");
  
}

/**
 * gtk_icon_view_get_pixbuf_column:
 * @icon_view: A #GtkIconView.
 *
 * Returns the column with pixbufs for @icon_view.
 *
 * Returns: the pixbuf column, or -1 if it's unset.
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_pixbuf_column (GtkIconView  *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->pixbuf_column;
}

/**
 * gtk_icon_view_select_path:
 * @icon_view: A #GtkIconView.
 * @path: The #GtkTreePath to be selected.
 * 
 * Selects the row at @path.
 *
 * Since: 2.6
 **/
void
gtk_icon_view_select_path (GtkIconView *icon_view,
			   GtkTreePath *path)
{
  GtkIconViewItem *item = NULL;

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (icon_view->priv->model != NULL);
  g_return_if_fail (path != NULL);

  if (gtk_tree_path_get_depth (path) > 0)
    item = g_list_nth_data (icon_view->priv->items,
			    gtk_tree_path_get_indices(path)[0]);

  if (item)
    gtk_icon_view_select_item (icon_view, item);
}

/**
 * gtk_icon_view_unselect_path:
 * @icon_view: A #GtkIconView.
 * @path: The #GtkTreePath to be unselected.
 * 
 * Unselects the row at @path.
 *
 * Since: 2.6
 **/
void
gtk_icon_view_unselect_path (GtkIconView *icon_view,
			     GtkTreePath *path)
{
  GtkIconViewItem *item;
  
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (icon_view->priv->model != NULL);
  g_return_if_fail (path != NULL);

  item = g_list_nth_data (icon_view->priv->items,
			  gtk_tree_path_get_indices(path)[0]);

  if (!item)
    return;
  
  gtk_icon_view_unselect_item (icon_view, item);
}

/**
 * gtk_icon_view_get_selected_items:
 * @icon_view: A #GtkIconView.
 *
 * Creates a list of paths of all selected items. Additionally, if you are
 * planning on modifying the model after calling this function, you may
 * want to convert the returned list into a list of #GtkTreeRowReference<!-- -->s.
 * To do this, you can use gtk_tree_row_reference_new().
 *
 * To free the return value, use:
 * |[
 * g_list_foreach (list, (GFunc)gtk_tree_path_free, NULL);
 * g_list_free (list);
 * ]|
 *
 * Return value: (element-type GtkTreePath) (transfer full): A #GList containing a #GtkTreePath for each selected row.
 *
 * Since: 2.6
 **/
GList *
gtk_icon_view_get_selected_items (GtkIconView *icon_view)
{
  GList *list;
  GList *selected = NULL;
  
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), NULL);
  
  for (list = icon_view->priv->items; list != NULL; list = list->next)
    {
      GtkIconViewItem *item = list->data;

      if (item->selected)
	{
	  GtkTreePath *path = gtk_tree_path_new_from_indices (item->index, -1);

	  selected = g_list_prepend (selected, path);
	}
    }

  return selected;
}

/**
 * gtk_icon_view_select_all:
 * @icon_view: A #GtkIconView.
 * 
 * Selects all the icons. @icon_view must has its selection mode set
 * to #GTK_SELECTION_MULTIPLE.
 *
 * Since: 2.6
 **/
void
gtk_icon_view_select_all (GtkIconView *icon_view)
{
  GList *items;
  gboolean dirty = FALSE;
  
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->selection_mode != GTK_SELECTION_MULTIPLE)
    return;

  for (items = icon_view->priv->items; items; items = items->next)
    {
      GtkIconViewItem *item = items->data;
      
      if (!item->selected)
	{
	  dirty = TRUE;
	  item->selected = TRUE;
	  gtk_icon_view_queue_draw_item (icon_view, item);
	}
    }

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

/**
 * gtk_icon_view_unselect_all:
 * @icon_view: A #GtkIconView.
 * 
 * Unselects all the icons.
 *
 * Since: 2.6
 **/
void
gtk_icon_view_unselect_all (GtkIconView *icon_view)
{
  gboolean dirty = FALSE;
  
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->selection_mode == GTK_SELECTION_BROWSE)
    return;

  dirty = gtk_icon_view_unselect_all_internal (icon_view);

  if (dirty)
    g_signal_emit (icon_view, icon_view_signals[SELECTION_CHANGED], 0);
}

/**
 * gtk_icon_view_path_is_selected:
 * @icon_view: A #GtkIconView.
 * @path: A #GtkTreePath to check selection on.
 * 
 * Returns %TRUE if the icon pointed to by @path is currently
 * selected. If @path does not point to a valid location, %FALSE is returned.
 * 
 * Return value: %TRUE if @path is selected.
 *
 * Since: 2.6
 **/
gboolean
gtk_icon_view_path_is_selected (GtkIconView *icon_view,
				GtkTreePath *path)
{
  GtkIconViewItem *item;
  
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (icon_view->priv->model != NULL, FALSE);
  g_return_val_if_fail (path != NULL, FALSE);
  
  item = g_list_nth_data (icon_view->priv->items,
			  gtk_tree_path_get_indices(path)[0]);

  if (!item)
    return FALSE;
  
  return item->selected;
}

/**
 * gtk_icon_view_get_item_row:
 * @icon_view: a #GtkIconView
 * @path: the #GtkTreePath of the item
 *
 * Gets the row in which the item @path is currently
 * displayed. Row numbers start at 0.
 *
 * Returns: The row in which the item is displayed
 *
 * Since: 2.22
 */
gint
gtk_icon_view_get_item_row (GtkIconView *icon_view,
                            GtkTreePath *path)
{
  GtkIconViewItem *item;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (icon_view->priv->model != NULL, FALSE);
  g_return_val_if_fail (path != NULL, FALSE);

  item = g_list_nth_data (icon_view->priv->items,
                          gtk_tree_path_get_indices(path)[0]);

  if (!item)
    return -1;

  return item->row;
}

/**
 * gtk_icon_view_get_item_column:
 * @icon_view: a #GtkIconView
 * @path: the #GtkTreePath of the item
 *
 * Gets the column in which the item @path is currently
 * displayed. Column numbers start at 0.
 *
 * Returns: The column in which the item is displayed
 *
 * Since: 2.22
 */
gint
gtk_icon_view_get_item_column (GtkIconView *icon_view,
                               GtkTreePath *path)
{
  GtkIconViewItem *item;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (icon_view->priv->model != NULL, FALSE);
  g_return_val_if_fail (path != NULL, FALSE);

  item = g_list_nth_data (icon_view->priv->items,
                          gtk_tree_path_get_indices(path)[0]);

  if (!item)
    return -1;

  return item->col;
}

/**
 * gtk_icon_view_item_activated:
 * @icon_view: A #GtkIconView
 * @path: The #GtkTreePath to be activated
 * 
 * Activates the item determined by @path.
 *
 * Since: 2.6
 **/
void
gtk_icon_view_item_activated (GtkIconView      *icon_view,
			      GtkTreePath      *path)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  g_return_if_fail (path != NULL);
  
  g_signal_emit (icon_view, icon_view_signals[ITEM_ACTIVATED], 0, path);
}

/**
 * gtk_icon_view_set_item_orientation:
 * @icon_view: a #GtkIconView
 * @orientation: the relative position of texts and icons 
 * 
 * Sets the ::item-orientation property which determines whether
 * the labels are drawn beside the icons instead of below.
 *
 * Since: 2.22
 **/
void 
gtk_icon_view_set_item_orientation (GtkIconView    *icon_view,
                                    GtkOrientation  orientation)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->item_orientation != orientation)
    {
      icon_view->priv->item_orientation = orientation;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);

      update_text_cell (icon_view);
      update_pixbuf_cell (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "item-orientation");
      g_object_notify (G_OBJECT (icon_view), "orientation");
    }
}

/**
 * gtk_icon_view_set_orientation:
 * @icon_view: a #GtkIconView
 * @orientation: the relative position of texts and icons 
 * 
 * Sets the ::orientation property which determines whether the labels 
 * are drawn beside the icons instead of below.
 *
 * Since: 2.6
 *
 * Deprecated: 2.22: Use gtk_icon_view_set_item_orientation()
 **/
void 
gtk_icon_view_set_orientation (GtkIconView    *icon_view,
                               GtkOrientation  orientation)
{
  gtk_icon_view_set_item_orientation (icon_view, orientation);
}

/**
 * gtk_icon_view_get_item_orientation:
 * @icon_view: a #GtkIconView
 *
 * Returns the value of the ::item-orientation property which determines
 * whether the labels are drawn beside the icons instead of below.
 *
 * Return value: the relative position of texts and icons
 *
 * Since: 2.22
 */
GtkOrientation
gtk_icon_view_get_item_orientation (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view),
			GTK_ORIENTATION_VERTICAL);

  return icon_view->priv->item_orientation;
}

/**
 * gtk_icon_view_get_orientation:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::orientation property which determines 
 * whether the labels are drawn beside the icons instead of below. 
 * 
 * Return value: the relative position of texts and icons 
 *
 * Since: 2.6
 *
 * Deprecated: 2.22: Use gtk_icon_view_get_item_orientation()
 **/
GtkOrientation
gtk_icon_view_get_orientation (GtkIconView *icon_view)
{
  return gtk_icon_view_get_item_orientation (icon_view);
}

/**
 * gtk_icon_view_set_columns:
 * @icon_view: a #GtkIconView
 * @columns: the number of columns
 * 
 * Sets the ::columns property which determines in how
 * many columns the icons are arranged. If @columns is
 * -1, the number of columns will be chosen automatically 
 * to fill the available area. 
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_columns (GtkIconView *icon_view,
			   gint         columns)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->columns != columns)
    {
      icon_view->priv->columns = columns;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "columns");
    }  
}

/**
 * gtk_icon_view_get_columns:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::columns property.
 * 
 * Return value: the number of columns, or -1
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_columns (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->columns;
}

/**
 * gtk_icon_view_set_item_width:
 * @icon_view: a #GtkIconView
 * @item_width: the width for each item
 * 
 * Sets the ::item-width property which specifies the width 
 * to use for each item. If it is set to -1, the icon view will 
 * automatically determine a suitable item size.
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_item_width (GtkIconView *icon_view,
			      gint         item_width)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->item_width != item_width)
    {
      icon_view->priv->item_width = item_width;
      
      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      update_text_cell (icon_view);

      g_object_notify (G_OBJECT (icon_view), "item-width");
    }  
}

/**
 * gtk_icon_view_get_item_width:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::item-width property.
 * 
 * Return value: the width of a single item, or -1
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_item_width (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->item_width;
}


/**
 * gtk_icon_view_set_spacing:
 * @icon_view: a #GtkIconView
 * @spacing: the spacing
 * 
 * Sets the ::spacing property which specifies the space 
 * which is inserted between the cells (i.e. the icon and 
 * the text) of an item.
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_spacing (GtkIconView *icon_view,
			   gint         spacing)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->spacing != spacing)
    {
      icon_view->priv->spacing = spacing;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "spacing");
    }  
}

/**
 * gtk_icon_view_get_spacing:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::spacing property.
 * 
 * Return value: the space between cells 
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_spacing (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->spacing;
}

/**
 * gtk_icon_view_set_row_spacing:
 * @icon_view: a #GtkIconView
 * @row_spacing: the row spacing
 * 
 * Sets the ::row-spacing property which specifies the space 
 * which is inserted between the rows of the icon view.
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_row_spacing (GtkIconView *icon_view,
			       gint         row_spacing)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->row_spacing != row_spacing)
    {
      icon_view->priv->row_spacing = row_spacing;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "row-spacing");
    }  
}

/**
 * gtk_icon_view_get_row_spacing:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::row-spacing property.
 * 
 * Return value: the space between rows
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_row_spacing (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->row_spacing;
}

/**
 * gtk_icon_view_set_column_spacing:
 * @icon_view: a #GtkIconView
 * @column_spacing: the column spacing
 * 
 * Sets the ::column-spacing property which specifies the space 
 * which is inserted between the columns of the icon view.
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_column_spacing (GtkIconView *icon_view,
				  gint         column_spacing)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->column_spacing != column_spacing)
    {
      icon_view->priv->column_spacing = column_spacing;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "column-spacing");
    }  
}

/**
 * gtk_icon_view_get_column_spacing:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::column-spacing property.
 * 
 * Return value: the space between columns
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_column_spacing (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->column_spacing;
}

/**
 * gtk_icon_view_set_margin:
 * @icon_view: a #GtkIconView
 * @margin: the margin
 * 
 * Sets the ::margin property which specifies the space 
 * which is inserted at the top, bottom, left and right 
 * of the icon view.
 *
 * Since: 2.6
 */
void 
gtk_icon_view_set_margin (GtkIconView *icon_view,
			  gint         margin)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->margin != margin)
    {
      icon_view->priv->margin = margin;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "margin");
    }  
}

/**
 * gtk_icon_view_get_margin:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::margin property.
 * 
 * Return value: the space at the borders 
 *
 * Since: 2.6
 */
gint
gtk_icon_view_get_margin (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->margin;
}

/**
 * gtk_icon_view_set_item_padding:
 * @icon_view: a #GtkIconView
 * @item_padding: the item padding
 *
 * Sets the #GtkIconView:item-padding property which specifies the padding
 * around each of the icon view's items.
 *
 * Since: 2.18
 */
void
gtk_icon_view_set_item_padding (GtkIconView *icon_view,
				gint         item_padding)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));
  
  if (icon_view->priv->item_padding != item_padding)
    {
      icon_view->priv->item_padding = item_padding;

      gtk_icon_view_stop_editing (icon_view, TRUE);
      gtk_icon_view_invalidate_sizes (icon_view);
      gtk_icon_view_queue_layout (icon_view);
      
      g_object_notify (G_OBJECT (icon_view), "item-padding");
    }  
}

/**
 * gtk_icon_view_get_item_padding:
 * @icon_view: a #GtkIconView
 * 
 * Returns the value of the ::item-padding property.
 * 
 * Return value: the padding around items
 *
 * Since: 2.18
 */
gint
gtk_icon_view_get_item_padding (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), -1);

  return icon_view->priv->item_padding;
}

/* Get/set whether drag_motion requested the drag data and
 * drag_data_received should thus not actually insert the data,
 * since the data doesn't result from a drop.
 */
static void
set_status_pending (GdkDragContext *context,
                    GdkDragAction   suggested_action)
{
  g_object_set_data (G_OBJECT (context),
                     I_("gtk-icon-view-status-pending"),
                     GINT_TO_POINTER (suggested_action));
}

static GdkDragAction
get_status_pending (GdkDragContext *context)
{
  return GPOINTER_TO_INT (g_object_get_data (G_OBJECT (context),
                                             "gtk-icon-view-status-pending"));
}

static void
unset_reorderable (GtkIconView *icon_view)
{
  if (icon_view->priv->reorderable)
    {
      icon_view->priv->reorderable = FALSE;
      g_object_notify (G_OBJECT (icon_view), "reorderable");
    }
}

static void
set_source_row (GdkDragContext *context,
                GtkTreeModel   *model,
                GtkTreePath    *source_row)
{
  if (source_row)
    g_object_set_data_full (G_OBJECT (context),
			    I_("gtk-icon-view-source-row"),
			    gtk_tree_row_reference_new (model, source_row),
			    (GDestroyNotify) gtk_tree_row_reference_free);
  else
    g_object_set_data_full (G_OBJECT (context),
			    I_("gtk-icon-view-source-row"),
			    NULL, NULL);
}

static GtkTreePath*
get_source_row (GdkDragContext *context)
{
  GtkTreeRowReference *ref;

  ref = g_object_get_data (G_OBJECT (context), "gtk-icon-view-source-row");

  if (ref)
    return gtk_tree_row_reference_get_path (ref);
  else
    return NULL;
}

typedef struct
{
  GtkTreeRowReference *dest_row;
  gboolean             empty_view_drop;
  gboolean             drop_append_mode;
} DestRow;

static void
dest_row_free (gpointer data)
{
  DestRow *dr = (DestRow *)data;

  gtk_tree_row_reference_free (dr->dest_row);
  g_free (dr);
}

static void
set_dest_row (GdkDragContext *context,
	      GtkTreeModel   *model,
	      GtkTreePath    *dest_row,
	      gboolean        empty_view_drop,
	      gboolean        drop_append_mode)
{
  DestRow *dr;

  if (!dest_row)
    {
      g_object_set_data_full (G_OBJECT (context),
			      I_("gtk-icon-view-dest-row"),
			      NULL, NULL);
      return;
    }
  
  dr = g_new0 (DestRow, 1);
     
  dr->dest_row = gtk_tree_row_reference_new (model, dest_row);
  dr->empty_view_drop = empty_view_drop;
  dr->drop_append_mode = drop_append_mode;
  g_object_set_data_full (G_OBJECT (context),
                          I_("gtk-icon-view-dest-row"),
                          dr, (GDestroyNotify) dest_row_free);
}

static GtkTreePath*
get_dest_row (GdkDragContext *context)
{
  DestRow *dr;

  dr = g_object_get_data (G_OBJECT (context), "gtk-icon-view-dest-row");

  if (dr)
    {
      GtkTreePath *path = NULL;
      
      if (dr->dest_row)
	path = gtk_tree_row_reference_get_path (dr->dest_row);
      else if (dr->empty_view_drop)
	path = gtk_tree_path_new_from_indices (0, -1);
      else
	path = NULL;

      if (path && dr->drop_append_mode)
	gtk_tree_path_next (path);

      return path;
    }
  else
    return NULL;
}

static gboolean
check_model_dnd (GtkTreeModel *model,
                 GType         required_iface,
                 const gchar  *signal)
{
  if (model == NULL || !G_TYPE_CHECK_INSTANCE_TYPE ((model), required_iface))
    {
      g_warning ("You must override the default '%s' handler "
                 "on GtkIconView when using models that don't support "
                 "the %s interface and enabling drag-and-drop. The simplest way to do this "
                 "is to connect to '%s' and call "
                 "g_signal_stop_emission_by_name() in your signal handler to prevent "
                 "the default handler from running. Look at the source code "
                 "for the default handler in gtkiconview.c to get an idea what "
                 "your handler should do. (gtkiconview.c is in the GTK+ source "
                 "code.) If you're using GTK+ from a language other than C, "
                 "there may be a more natural way to override default handlers, e.g. via derivation.",
                 signal, g_type_name (required_iface), signal);
      return FALSE;
    }
  else
    return TRUE;
}

static void
remove_scroll_timeout (GtkIconView *icon_view)
{
  if (icon_view->priv->scroll_timeout_id != 0)
    {
      g_source_remove (icon_view->priv->scroll_timeout_id);

      icon_view->priv->scroll_timeout_id = 0;
    }
}

static void
gtk_icon_view_autoscroll (GtkIconView *icon_view)
{
  gint px, py, width, height;
  gint hoffset, voffset;
  gfloat value;

  gdk_window_get_pointer (GTK_WIDGET (icon_view)->window, &px, &py, NULL);
  gdk_window_get_geometry (GTK_WIDGET (icon_view)->window, NULL, NULL, &width, &height, NULL);

  /* see if we are near the edge. */
  voffset = py - 2 * SCROLL_EDGE_SIZE;
  if (voffset > 0)
    voffset = MAX (py - (height - 2 * SCROLL_EDGE_SIZE), 0);

  hoffset = px - 2 * SCROLL_EDGE_SIZE;
  if (hoffset > 0)
    hoffset = MAX (px - (width - 2 * SCROLL_EDGE_SIZE), 0);

  if (voffset != 0)
    {
      value = CLAMP (icon_view->priv->vadjustment->value + voffset, 
		     icon_view->priv->vadjustment->lower,
		     icon_view->priv->vadjustment->upper - icon_view->priv->vadjustment->page_size);
      gtk_adjustment_set_value (icon_view->priv->vadjustment, value);
    }
  if (hoffset != 0)
    {
      value = CLAMP (icon_view->priv->hadjustment->value + hoffset, 
		     icon_view->priv->hadjustment->lower,
		     icon_view->priv->hadjustment->upper - icon_view->priv->hadjustment->page_size);
      gtk_adjustment_set_value (icon_view->priv->hadjustment, value);
    }
}


static gboolean
drag_scroll_timeout (gpointer data)
{
  GtkIconView *icon_view = GTK_ICON_VIEW (data);

  gtk_icon_view_autoscroll (icon_view);

  return TRUE;
}


static gboolean
set_destination (GtkIconView    *icon_view,
		 GdkDragContext *context,
		 gint            x,
		 gint            y,
		 GdkDragAction  *suggested_action,
		 GdkAtom        *target)
{
  GtkWidget *widget;
  GtkTreePath *path = NULL;
  GtkIconViewDropPosition pos;
  GtkIconViewDropPosition old_pos;
  GtkTreePath *old_dest_path = NULL;
  gboolean can_drop = FALSE;

  widget = GTK_WIDGET (icon_view);

  *suggested_action = 0;
  *target = GDK_NONE;

  if (!icon_view->priv->dest_set)
    {
      /* someone unset us as a drag dest, note that if
       * we return FALSE drag_leave isn't called
       */

      gtk_icon_view_set_drag_dest_item (icon_view,
					NULL,
					GTK_ICON_VIEW_DROP_LEFT);

      remove_scroll_timeout (GTK_ICON_VIEW (widget));

      return FALSE; /* no longer a drop site */
    }

  *target = gtk_drag_dest_find_target (widget, context,
                                       gtk_drag_dest_get_target_list (widget));
  if (*target == GDK_NONE)
    return FALSE;

  if (!gtk_icon_view_get_dest_item_at_pos (icon_view, x, y, &path, &pos)) 
    {
      gint n_children;
      GtkTreeModel *model;
      
      /* the row got dropped on empty space, let's setup a special case
       */

      if (path)
	gtk_tree_path_free (path);

      model = gtk_icon_view_get_model (icon_view);

      n_children = gtk_tree_model_iter_n_children (model, NULL);
      if (n_children)
        {
          pos = GTK_ICON_VIEW_DROP_BELOW;
          path = gtk_tree_path_new_from_indices (n_children - 1, -1);
        }
      else
        {
          pos = GTK_ICON_VIEW_DROP_ABOVE;
          path = gtk_tree_path_new_from_indices (0, -1);
        }

      can_drop = TRUE;

      goto out;
    }

  g_assert (path);

  gtk_icon_view_get_drag_dest_item (icon_view,
				    &old_dest_path,
				    &old_pos);
  
  if (old_dest_path)
    gtk_tree_path_free (old_dest_path);
  
  if (TRUE /* FIXME if the location droppable predicate */)
    {
      can_drop = TRUE;
    }

out:
  if (can_drop)
    {
      GtkWidget *source_widget;

      *suggested_action = gdk_drag_context_get_suggested_action (context);
      source_widget = gtk_drag_get_source_widget (context);

      if (source_widget == widget)
        {
          /* Default to MOVE, unless the user has
           * pressed ctrl or shift to affect available actions
           */
          if ((gdk_drag_context_get_actions (context) & GDK_ACTION_MOVE) != 0)
            *suggested_action = GDK_ACTION_MOVE;
        }

      gtk_icon_view_set_drag_dest_item (GTK_ICON_VIEW (widget),
					path, pos);
    }
  else
    {
      /* can't drop here */
      gtk_icon_view_set_drag_dest_item (GTK_ICON_VIEW (widget),
					NULL,
					GTK_ICON_VIEW_DROP_LEFT);
    }
  
  if (path)
    gtk_tree_path_free (path);
  
  return TRUE;
}

static GtkTreePath*
get_logical_destination (GtkIconView *icon_view,
			 gboolean    *drop_append_mode)
{
  /* adjust path to point to the row the drop goes in front of */
  GtkTreePath *path = NULL;
  GtkIconViewDropPosition pos;
  
  *drop_append_mode = FALSE;

  gtk_icon_view_get_drag_dest_item (icon_view, &path, &pos);

  if (path == NULL)
    return NULL;

  if (pos == GTK_ICON_VIEW_DROP_RIGHT || 
      pos == GTK_ICON_VIEW_DROP_BELOW)
    {
      GtkTreeIter iter;
      GtkTreeModel *model = icon_view->priv->model;

      if (!gtk_tree_model_get_iter (model, &iter, path) ||
          !gtk_tree_model_iter_next (model, &iter))
        *drop_append_mode = TRUE;
      else
        {
          *drop_append_mode = FALSE;
          gtk_tree_path_next (path);
        }      
    }

  return path;
}

static gboolean
gtk_icon_view_maybe_begin_drag (GtkIconView    *icon_view,
				GdkEventMotion *event)
{
  GtkWidget *widget = GTK_WIDGET (icon_view);
  GdkDragContext *context;
  GtkTreePath *path = NULL;
  gint button;
  GtkTreeModel *model;
  gboolean retval = FALSE;

  if (!icon_view->priv->source_set)
    goto out;

  if (icon_view->priv->pressed_button < 0)
    goto out;

  if (!gtk_drag_check_threshold (GTK_WIDGET (icon_view),
                                 icon_view->priv->press_start_x,
                                 icon_view->priv->press_start_y,
                                 event->x, event->y))
    goto out;

  model = gtk_icon_view_get_model (icon_view);

  if (model == NULL)
    goto out;

  button = icon_view->priv->pressed_button;
  icon_view->priv->pressed_button = -1;

  path = gtk_icon_view_get_path_at_pos (icon_view,
					icon_view->priv->press_start_x,
					icon_view->priv->press_start_y);

  if (path == NULL)
    goto out;

  if (!GTK_IS_TREE_DRAG_SOURCE (model) ||
      !gtk_tree_drag_source_row_draggable (GTK_TREE_DRAG_SOURCE (model),
					   path))
    goto out;

  /* FIXME Check whether we're a start button, if not return FALSE and
   * free path
   */

  /* Now we can begin the drag */
  
  retval = TRUE;

  context = gtk_drag_begin (widget,
                            gtk_drag_source_get_target_list (widget),
                            icon_view->priv->source_actions,
                            button,
                            (GdkEvent*)event);

  set_source_row (context, model, path);
  
 out:
  if (path)
    gtk_tree_path_free (path);

  return retval;
}

/* Source side drag signals */
static void 
gtk_icon_view_drag_begin (GtkWidget      *widget,
			  GdkDragContext *context)
{
  GtkIconView *icon_view;
  GtkIconViewItem *item;
  GdkPixmap *icon;
  gint x, y;
  GtkTreePath *path;

  icon_view = GTK_ICON_VIEW (widget);

  /* if the user uses a custom DnD impl, we don't set the icon here */
  if (!icon_view->priv->dest_set && !icon_view->priv->source_set)
    return;

  item = gtk_icon_view_get_item_at_coords (icon_view,
					   icon_view->priv->press_start_x,
					   icon_view->priv->press_start_y,
					   TRUE,
					   NULL);

  g_return_if_fail (item != NULL);

  x = icon_view->priv->press_start_x - item->x + 1;
  y = icon_view->priv->press_start_y - item->y + 1;
  
  path = gtk_tree_path_new_from_indices (item->index, -1);
  icon = gtk_icon_view_create_drag_icon (icon_view, path);
  gtk_tree_path_free (path);

  gtk_drag_set_icon_pixmap (context, 
			    gdk_drawable_get_colormap (icon),
			    icon, 
			    NULL, 
			    x, y);

  g_object_unref (icon);
}

static void 
gtk_icon_view_drag_end (GtkWidget      *widget,
			GdkDragContext *context)
{
  /* do nothing */
}

static void 
gtk_icon_view_drag_data_get (GtkWidget        *widget,
			     GdkDragContext   *context,
			     GtkSelectionData *selection_data,
			     guint             info,
			     guint             time)
{
  GtkIconView *icon_view;
  GtkTreeModel *model;
  GtkTreePath *source_row;

  icon_view = GTK_ICON_VIEW (widget);
  model = gtk_icon_view_get_model (icon_view);

  if (model == NULL)
    return;

  if (!icon_view->priv->source_set)
    return;

  source_row = get_source_row (context);

  if (source_row == NULL)
    return;

  /* We can implement the GTK_TREE_MODEL_ROW target generically for
   * any model; for DragSource models there are some other targets
   * we also support.
   */

  if (GTK_IS_TREE_DRAG_SOURCE (model) &&
      gtk_tree_drag_source_drag_data_get (GTK_TREE_DRAG_SOURCE (model),
                                          source_row,
                                          selection_data))
    goto done;

  /* If drag_data_get does nothing, try providing row data. */
  if (selection_data->target == gdk_atom_intern_static_string ("GTK_TREE_MODEL_ROW"))
    gtk_tree_set_row_drag_data (selection_data,
				model,
				source_row);

 done:
  gtk_tree_path_free (source_row);
}

static void 
gtk_icon_view_drag_data_delete (GtkWidget      *widget,
				GdkDragContext *context)
{
  GtkTreeModel *model;
  GtkIconView *icon_view;
  GtkTreePath *source_row;

  icon_view = GTK_ICON_VIEW (widget);
  model = gtk_icon_view_get_model (icon_view);

  if (!check_model_dnd (model, GTK_TYPE_TREE_DRAG_SOURCE, "drag-data-delete"))
    return;

  if (!icon_view->priv->source_set)
    return;

  source_row = get_source_row (context);

  if (source_row == NULL)
    return;

  gtk_tree_drag_source_drag_data_delete (GTK_TREE_DRAG_SOURCE (model),
                                         source_row);

  gtk_tree_path_free (source_row);

  set_source_row (context, NULL, NULL);
}

/* Target side drag signals */
static void
gtk_icon_view_drag_leave (GtkWidget      *widget,
			  GdkDragContext *context,
			  guint           time)
{
  GtkIconView *icon_view;

  icon_view = GTK_ICON_VIEW (widget);

  /* unset any highlight row */
  gtk_icon_view_set_drag_dest_item (icon_view,
				    NULL,
				    GTK_ICON_VIEW_DROP_LEFT);

  remove_scroll_timeout (icon_view);
}

static gboolean 
gtk_icon_view_drag_motion (GtkWidget      *widget,
			   GdkDragContext *context,
			   gint            x,
			   gint            y,
			   guint           time)
{
  GtkTreePath *path = NULL;
  GtkIconViewDropPosition pos;
  GtkIconView *icon_view;
  GdkDragAction suggested_action = 0;
  GdkAtom target;
  gboolean empty;

  icon_view = GTK_ICON_VIEW (widget);

  if (!set_destination (icon_view, context, x, y, &suggested_action, &target))
    return FALSE;

  gtk_icon_view_get_drag_dest_item (icon_view, &path, &pos);

  /* we only know this *after* set_desination_row */
  empty = icon_view->priv->empty_view_drop;

  if (path == NULL && !empty)
    {
      /* Can't drop here. */
      gdk_drag_status (context, 0, time);
    }
  else
    {
      if (icon_view->priv->scroll_timeout_id == 0)
	{
	  icon_view->priv->scroll_timeout_id =
	    gdk_threads_add_timeout (50, drag_scroll_timeout, icon_view);
	}

      if (target == gdk_atom_intern_static_string ("GTK_TREE_MODEL_ROW"))
        {
          /* Request data so we can use the source row when
           * determining whether to accept the drop
           */
          set_status_pending (context, suggested_action);
          gtk_drag_get_data (widget, context, target, time);
        }
      else
        {
          set_status_pending (context, 0);
          gdk_drag_status (context, suggested_action, time);
        }
    }

  if (path)
    gtk_tree_path_free (path);

  return TRUE;
}

static gboolean 
gtk_icon_view_drag_drop (GtkWidget      *widget,
			 GdkDragContext *context,
			 gint            x,
			 gint            y,
			 guint           time)
{
  GtkIconView *icon_view;
  GtkTreePath *path;
  GdkDragAction suggested_action = 0;
  GdkAtom target = GDK_NONE;
  GtkTreeModel *model;
  gboolean drop_append_mode;

  icon_view = GTK_ICON_VIEW (widget);
  model = gtk_icon_view_get_model (icon_view);

  remove_scroll_timeout (GTK_ICON_VIEW (widget));

  if (!icon_view->priv->dest_set)
    return FALSE;

  if (!check_model_dnd (model, GTK_TYPE_TREE_DRAG_DEST, "drag-drop"))
    return FALSE;

  if (!set_destination (icon_view, context, x, y, &suggested_action, &target))
    return FALSE;
  
  path = get_logical_destination (icon_view, &drop_append_mode);

  if (target != GDK_NONE && path != NULL)
    {
      /* in case a motion had requested drag data, change things so we
       * treat drag data receives as a drop.
       */
      set_status_pending (context, 0);
      set_dest_row (context, model, path, 
		    icon_view->priv->empty_view_drop, drop_append_mode);
    }

  if (path)
    gtk_tree_path_free (path);

  /* Unset this thing */
  gtk_icon_view_set_drag_dest_item (icon_view, NULL, GTK_ICON_VIEW_DROP_LEFT);

  if (target != GDK_NONE)
    {
      gtk_drag_get_data (widget, context, target, time);
      return TRUE;
    }
  else
    return FALSE;
}

static void
gtk_icon_view_drag_data_received (GtkWidget        *widget,
				  GdkDragContext   *context,
				  gint              x,
				  gint              y,
				  GtkSelectionData *selection_data,
				  guint             info,
				  guint             time)
{
  GtkTreePath *path;
  gboolean accepted = FALSE;
  GtkTreeModel *model;
  GtkIconView *icon_view;
  GtkTreePath *dest_row;
  GdkDragAction suggested_action;
  gboolean drop_append_mode;
  
  icon_view = GTK_ICON_VIEW (widget);  
  model = gtk_icon_view_get_model (icon_view);

  if (!check_model_dnd (model, GTK_TYPE_TREE_DRAG_DEST, "drag-data-received"))
    return;

  if (!icon_view->priv->dest_set)
    return;

  suggested_action = get_status_pending (context);

  if (suggested_action)
    {
      /* We are getting this data due to a request in drag_motion,
       * rather than due to a request in drag_drop, so we are just
       * supposed to call drag_status, not actually paste in the
       * data.
       */
      path = get_logical_destination (icon_view, &drop_append_mode);

      if (path == NULL)
        suggested_action = 0;

      if (suggested_action)
        {
	  if (!gtk_tree_drag_dest_row_drop_possible (GTK_TREE_DRAG_DEST (model),
						     path,
						     selection_data))
	    suggested_action = 0;
        }

      gdk_drag_status (context, suggested_action, time);

      if (path)
        gtk_tree_path_free (path);

      /* If you can't drop, remove user drop indicator until the next motion */
      if (suggested_action == 0)
        gtk_icon_view_set_drag_dest_item (icon_view,
					  NULL,
					  GTK_ICON_VIEW_DROP_LEFT);
      return;
    }
  

  dest_row = get_dest_row (context);

  if (dest_row == NULL)
    return;

  if (selection_data->length >= 0)
    {
      if (gtk_tree_drag_dest_drag_data_received (GTK_TREE_DRAG_DEST (model),
                                                 dest_row,
                                                 selection_data))
        accepted = TRUE;
    }

  gtk_drag_finish (context,
                   accepted,
                   (gdk_drag_context_get_selected_action (context) == GDK_ACTION_MOVE),
                   time);

  gtk_tree_path_free (dest_row);

  /* drop dest_row */
  set_dest_row (context, NULL, NULL, FALSE, FALSE);
}

/* Drag-and-Drop support */
/**
 * gtk_icon_view_enable_model_drag_source:
 * @icon_view: a #GtkIconTreeView
 * @start_button_mask: Mask of allowed buttons to start drag
 * @targets: the table of targets that the drag will support
 * @n_targets: the number of items in @targets
 * @actions: the bitmask of possible actions for a drag from this
 *    widget
 *
 * Turns @icon_view into a drag source for automatic DND. Calling this
 * method sets #GtkIconView:reorderable to %FALSE.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_enable_model_drag_source (GtkIconView              *icon_view,
					GdkModifierType           start_button_mask,
					const GtkTargetEntry     *targets,
					gint                      n_targets,
					GdkDragAction             actions)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  gtk_drag_source_set (GTK_WIDGET (icon_view), 0, targets, n_targets, actions);

  icon_view->priv->start_button_mask = start_button_mask;
  icon_view->priv->source_actions = actions;

  icon_view->priv->source_set = TRUE;

  unset_reorderable (icon_view);
}

/**
 * gtk_icon_view_enable_model_drag_dest:
 * @icon_view: a #GtkIconView
 * @targets: the table of targets that the drag will support
 * @n_targets: the number of items in @targets
 * @actions: the bitmask of possible actions for a drag to this
 *    widget
 *
 * Turns @icon_view into a drop destination for automatic DND. Calling this
 * method sets #GtkIconView:reorderable to %FALSE.
 *
 * Since: 2.8
 **/
void 
gtk_icon_view_enable_model_drag_dest (GtkIconView          *icon_view,
				      const GtkTargetEntry *targets,
				      gint                  n_targets,
				      GdkDragAction         actions)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  gtk_drag_dest_set (GTK_WIDGET (icon_view), 0, targets, n_targets, actions);

  icon_view->priv->dest_actions = actions;

  icon_view->priv->dest_set = TRUE;

  unset_reorderable (icon_view);  
}

/**
 * gtk_icon_view_unset_model_drag_source:
 * @icon_view: a #GtkIconView
 * 
 * Undoes the effect of gtk_icon_view_enable_model_drag_source(). Calling this
 * method sets #GtkIconView:reorderable to %FALSE.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_unset_model_drag_source (GtkIconView *icon_view)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->source_set)
    {
      gtk_drag_source_unset (GTK_WIDGET (icon_view));
      icon_view->priv->source_set = FALSE;
    }

  unset_reorderable (icon_view);
}

/**
 * gtk_icon_view_unset_model_drag_dest:
 * @icon_view: a #GtkIconView
 * 
 * Undoes the effect of gtk_icon_view_enable_model_drag_dest(). Calling this
 * method sets #GtkIconView:reorderable to %FALSE.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_unset_model_drag_dest (GtkIconView *icon_view)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->dest_set)
    {
      gtk_drag_dest_unset (GTK_WIDGET (icon_view));
      icon_view->priv->dest_set = FALSE;
    }

  unset_reorderable (icon_view);
}

/* These are useful to implement your own custom stuff. */
/**
 * gtk_icon_view_set_drag_dest_item:
 * @icon_view: a #GtkIconView
 * @path: (allow-none): The path of the item to highlight, or %NULL.
 * @pos: Specifies where to drop, relative to the item
 *
 * Sets the item that is highlighted for feedback.
 *
 * Since: 2.8
 */
void
gtk_icon_view_set_drag_dest_item (GtkIconView              *icon_view,
				  GtkTreePath              *path,
				  GtkIconViewDropPosition   pos)
{
  /* Note; this function is exported to allow a custom DND
   * implementation, so it can't touch TreeViewDragInfo
   */

  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (icon_view->priv->dest_item)
    {
      GtkTreePath *current_path;
      current_path = gtk_tree_row_reference_get_path (icon_view->priv->dest_item);
      gtk_tree_row_reference_free (icon_view->priv->dest_item);
      icon_view->priv->dest_item = NULL;      

      gtk_icon_view_queue_draw_path (icon_view, current_path);
      gtk_tree_path_free (current_path);
    }
  
  /* special case a drop on an empty model */
  icon_view->priv->empty_view_drop = FALSE;
  if (pos == GTK_ICON_VIEW_DROP_ABOVE && path
      && gtk_tree_path_get_depth (path) == 1
      && gtk_tree_path_get_indices (path)[0] == 0)
    {
      gint n_children;

      n_children = gtk_tree_model_iter_n_children (icon_view->priv->model,
                                                   NULL);

      if (n_children == 0)
        icon_view->priv->empty_view_drop = TRUE;
    }

  icon_view->priv->dest_pos = pos;

  if (path)
    {
      icon_view->priv->dest_item =
        gtk_tree_row_reference_new_proxy (G_OBJECT (icon_view), 
					  icon_view->priv->model, path);
      
      gtk_icon_view_queue_draw_path (icon_view, path);
    }
}

/**
 * gtk_icon_view_get_drag_dest_item:
 * @icon_view: a #GtkIconView
 * @path: (allow-none): Return location for the path of the highlighted item, or %NULL.
 * @pos: (allow-none): Return location for the drop position, or %NULL
 * 
 * Gets information about the item that is highlighted for feedback.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_get_drag_dest_item (GtkIconView              *icon_view,
				  GtkTreePath             **path,
				  GtkIconViewDropPosition  *pos)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  if (path)
    {
      if (icon_view->priv->dest_item)
        *path = gtk_tree_row_reference_get_path (icon_view->priv->dest_item);
      else
	*path = NULL;
    }

  if (pos)
    *pos = icon_view->priv->dest_pos;
}

/**
 * gtk_icon_view_get_dest_item_at_pos:
 * @icon_view: a #GtkIconView
 * @drag_x: the position to determine the destination item for
 * @drag_y: the position to determine the destination item for
 * @path: (allow-none): Return location for the path of the item, or %NULL.
 * @pos: (allow-none): Return location for the drop position, or %NULL
 * 
 * Determines the destination item for a given position.
 * 
 * Return value: whether there is an item at the given position.
 *
 * Since: 2.8
 **/
gboolean
gtk_icon_view_get_dest_item_at_pos (GtkIconView              *icon_view,
				    gint                      drag_x,
				    gint                      drag_y,
				    GtkTreePath             **path,
				    GtkIconViewDropPosition  *pos)
{
  GtkIconViewItem *item;

  /* Note; this function is exported to allow a custom DND
   * implementation, so it can't touch TreeViewDragInfo
   */

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);
  g_return_val_if_fail (drag_x >= 0, FALSE);
  g_return_val_if_fail (drag_y >= 0, FALSE);
  g_return_val_if_fail (icon_view->priv->bin_window != NULL, FALSE);


  if (path)
    *path = NULL;

  item = gtk_icon_view_get_item_at_coords (icon_view, 
					   drag_x + icon_view->priv->hadjustment->value, 
					   drag_y + icon_view->priv->vadjustment->value,
					   FALSE, NULL);

  if (item == NULL)
    return FALSE;

  if (path)
    *path = gtk_tree_path_new_from_indices (item->index, -1);

  if (pos)
    {
      if (drag_x < item->x + item->width / 4)
	*pos = GTK_ICON_VIEW_DROP_LEFT;
      else if (drag_x > item->x + item->width * 3 / 4)
	*pos = GTK_ICON_VIEW_DROP_RIGHT;
      else if (drag_y < item->y + item->height / 4)
	*pos = GTK_ICON_VIEW_DROP_ABOVE;
      else if (drag_y > item->y + item->height * 3 / 4)
	*pos = GTK_ICON_VIEW_DROP_BELOW;
      else
	*pos = GTK_ICON_VIEW_DROP_INTO;
    }

  return TRUE;
}

/**
 * gtk_icon_view_create_drag_icon:
 * @icon_view: a #GtkIconView
 * @path: a #GtkTreePath in @icon_view
 *
 * Creates a #GdkPixmap representation of the item at @path.
 * This image is used for a drag icon.
 *
 * Return value: (transfer full): a newly-allocated pixmap of the drag icon.
 * 
 * Since: 2.8
 **/
GdkPixmap *
gtk_icon_view_create_drag_icon (GtkIconView *icon_view,
				GtkTreePath *path)
{
  GtkWidget *widget;
  cairo_t *cr;
  GdkPixmap *drawable;
  GList *l;
  gint index;
  GdkRectangle area;

  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), NULL);
  g_return_val_if_fail (path != NULL, NULL);

  widget = GTK_WIDGET (icon_view);

  if (!gtk_widget_get_realized (widget))
    return NULL;

  index = gtk_tree_path_get_indices (path)[0];

  for (l = icon_view->priv->items; l; l = l->next) 
    {
      GtkIconViewItem *item = l->data;
      
      if (index == item->index)
	{
	  drawable = gdk_pixmap_new (icon_view->priv->bin_window,
				     item->width + 2,
				     item->height + 2,
				     -1);

	  cr = gdk_cairo_create (drawable);
	  cairo_set_line_width (cr, 1.);

	  gdk_cairo_set_source_color
	    (cr, &widget->style->base[gtk_widget_get_state (widget)]);
	  cairo_rectangle (cr, 0, 0, item->width + 2, item->height + 2);
	  cairo_fill (cr);

	  area.x = 0;
	  area.y = 0;
	  area.width = item->width;
	  area.height = item->height;

	  gtk_icon_view_paint_item (icon_view, cr, item, &area, 
 				    drawable, 1, 1, FALSE); 

	  cairo_set_source_rgb (cr, 0.0, 0.0, 0.0); /* black */
	  cairo_rectangle (cr, 0.5, 0.5, item->width + 1, item->height + 1);
	  cairo_stroke (cr);

	  cairo_destroy (cr);

	  return drawable;
	}
    }
  
  return NULL;
}

/**
 * gtk_icon_view_get_reorderable:
 * @icon_view: a #GtkIconView
 *
 * Retrieves whether the user can reorder the list via drag-and-drop. 
 * See gtk_icon_view_set_reorderable().
 *
 * Return value: %TRUE if the list can be reordered.
 *
 * Since: 2.8
 **/
gboolean
gtk_icon_view_get_reorderable (GtkIconView *icon_view)
{
  g_return_val_if_fail (GTK_IS_ICON_VIEW (icon_view), FALSE);

  return icon_view->priv->reorderable;
}

static const GtkTargetEntry item_targets[] = {
  { "GTK_TREE_MODEL_ROW", GTK_TARGET_SAME_WIDGET, 0 }
};


/**
 * gtk_icon_view_set_reorderable:
 * @icon_view: A #GtkIconView.
 * @reorderable: %TRUE, if the list of items can be reordered.
 *
 * This function is a convenience function to allow you to reorder models that
 * support the #GtkTreeDragSourceIface and the #GtkTreeDragDestIface.  Both
 * #GtkTreeStore and #GtkListStore support these.  If @reorderable is %TRUE, then
 * the user can reorder the model by dragging and dropping rows.  The
 * developer can listen to these changes by connecting to the model's
 * row_inserted and row_deleted signals. The reordering is implemented by setting up
 * the icon view as a drag source and destination. Therefore, drag and
 * drop can not be used in a reorderable view for any other purpose.
 *
 * This function does not give you any degree of control over the order -- any
 * reordering is allowed.  If more control is needed, you should probably
 * handle drag and drop manually.
 *
 * Since: 2.8
 **/
void
gtk_icon_view_set_reorderable (GtkIconView *icon_view,
			       gboolean     reorderable)
{
  g_return_if_fail (GTK_IS_ICON_VIEW (icon_view));

  reorderable = reorderable != FALSE;

  if (icon_view->priv->reorderable == reorderable)
    return;

  if (reorderable)
    {
      gtk_icon_view_enable_model_drag_source (icon_view,
					      GDK_BUTTON1_MASK,
					      item_targets,
					      G_N_ELEMENTS (item_targets),
					      GDK_ACTION_MOVE);
      gtk_icon_view_enable_model_drag_dest (icon_view,
					    item_targets,
					    G_N_ELEMENTS (item_targets),
					    GDK_ACTION_MOVE);
    }
  else
    {
      gtk_icon_view_unset_model_drag_source (icon_view);
      gtk_icon_view_unset_model_drag_dest (icon_view);
    }

  icon_view->priv->reorderable = reorderable;

  g_object_notify (G_OBJECT (icon_view), "reorderable");
}


#define GTK_ICON_VIEW_ITEM_ACCESSIBLE(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_ICON_VIEW_ITEM_ACCESSIBLE, GtkIconViewItemAccessible))
#define GTK_IS_ICON_VIEW_ITEM_ACCESSIBLE(obj)   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_ICON_VIEW_ITEM_ACCESSIBLE))


enum {
    ACTION_ACTIVATE,
    LAST_ACTION
};

typedef struct
{
  void* parent;

  GtkIconViewItem *item;

  GtkWidget *widget;

  void *state_set;

  gchar *text;

  GtkTextBuffer *text_buffer;

  gchar *action_descriptions[LAST_ACTION];
  gchar *image_description;
  guint action_idle_handler;
} GtkIconViewItemAccessible;

typedef struct _GtkIconViewItemAccessibleClass
{
  void* parent_class;

} GtkIconViewItemAccessibleClass;

#define GTK_ICON_VIEW_ACCESSIBLE(obj)      (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_ICON_VIEW_ACCESSIBLE, GtkIconViewAccessible))
#define GTK_IS_ICON_VIEW_ACCESSIBLE(obj)   (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_ICON_VIEW_ACCESSIBLE))

typedef struct
{
   void* parent;
} GtkIconViewAccessible;

typedef struct
{
  void *item;
  gint       index;
} GtkIconViewItemAccessibleInfo;

typedef struct
{
  GList *items;

  GtkAdjustment *old_hadj;
  GtkAdjustment *old_vadj;

  GtkTreeModel *model;

} GtkIconViewAccessiblePrivate;

static gboolean
gtk_icon_view_buildable_custom_tag_start (GtkBuildable  *buildable,
					  GtkBuilder    *builder,
					  GObject       *child,
					  const gchar   *tagname,
					  GMarkupParser *parser,
					  gpointer      *data)
{
  if (parent_buildable_iface->custom_tag_start (buildable, builder, child,
						tagname, parser, data))
    return TRUE;

  return _gtk_cell_layout_buildable_custom_tag_start (buildable, builder, child,
						      tagname, parser, data);
}

static void
gtk_icon_view_buildable_custom_tag_end (GtkBuildable *buildable,
					GtkBuilder   *builder,
					GObject      *child,
					const gchar  *tagname,
					gpointer     *data)
{
  if (strcmp (tagname, "attributes") == 0)
    _gtk_cell_layout_buildable_custom_tag_end (buildable, builder, child, tagname,
					       data);
  else
    parent_buildable_iface->custom_tag_end (buildable, builder, child, tagname,
					    data);
}



#define __GTK_ICON_VIEW_C__

