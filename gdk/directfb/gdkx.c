#include <gdk/gdkprivate.h>
#include <X11/Xlib.h>

Display *gdk_x11_drawable_get_xdisplay    (){return 0;}
XID      gdk_x11_drawable_get_xid         (){return 0;}
GdkDrawable *gdk_x11_window_get_drawable_impl (){return 0;}
GdkDrawable *gdk_x11_pixmap_get_drawable_impl (){return 0;}
Display *gdk_x11_image_get_xdisplay       (){return 0;}
XImage  *gdk_x11_image_get_ximage         (){return 0;}
Display *gdk_x11_colormap_get_xdisplay    (){return 0;}
Colormap gdk_x11_colormap_get_xcolormap   (){return 0;}
Display *gdk_x11_cursor_get_xdisplay      (){return 0;}
Cursor   gdk_x11_cursor_get_xcursor       (){return 0;}
Display *gdk_x11_display_get_xdisplay     (){return 0;}
Visual * gdk_x11_visual_get_xvisual       (){return 0;}
#if !defined(GDK_DISABLE_DEPRECATED) || defined(GDK_COMPILATION)
Display *gdk_x11_gc_get_xdisplay          (){return 0;}
GC       gdk_x11_gc_get_xgc               (){return 0;}
#endif
Screen * gdk_x11_screen_get_xscreen       (){return 0;}
int      gdk_x11_screen_get_screen_number (){return 0;}
void     gdk_x11_window_set_user_time     (){}
void     gdk_x11_window_move_to_current_desktop (){}
const char* gdk_x11_screen_get_window_manager_name (){return 0;}

#ifndef GDK_MULTIHEAD_SAFE
Window   gdk_x11_get_default_root_xwindow (){return 0;}
Display *gdk_x11_get_default_xdisplay     (){return 0;}
gint     gdk_x11_get_default_screen       (){return 0;}
#endif

GdkVisual* gdk_x11_screen_lookup_visual (){return 0;}

#ifndef GDK_DISABLE_DEPRECATED
#ifndef GDK_MULTIHEAD_SAFE
GdkVisual* gdkx_visual_get            (){return 0;}
#endif
#endif

#ifdef GDK_ENABLE_BROKEN
GdkColormap* gdkx_colormap_get (){return 0;}
#endif

GdkColormap *gdk_x11_colormap_foreign_new (){return 0;}

#if !defined (GDK_DISABLE_DEPRECATED) || defined (GDK_COMPILATION)
gpointer      gdk_xid_table_lookup_for_display (){return 0;}
#endif
guint32       gdk_x11_get_server_time  (){return 0;}
guint32       gdk_x11_display_get_user_time (){return 0;}

const gchar * gdk_x11_display_get_startup_notification_id (){return 0;}

void          gdk_x11_display_set_cursor_theme (){}

void gdk_x11_display_broadcast_startup_message (){}

gboolean gdk_x11_screen_supports_net_wm_hint (){return 0;}

XID      gdk_x11_screen_get_monitor_output   (){return 0;}

#ifndef GDK_MULTIHEAD_SAFE
#ifndef GDK_DISABLE_DEPRECATED
gpointer      gdk_xid_table_lookup   (){return 0;}
#endif
void          gdk_x11_grab_server    (){}
void          gdk_x11_ungrab_server  (){}
#endif

GdkDisplay   *gdk_x11_lookup_xdisplay (){return 0;}

Atom                  gdk_x11_atom_to_xatom_for_display (){return 0;}
GdkAtom               gdk_x11_xatom_to_atom_for_display (){return 0;}
Atom                  gdk_x11_get_xatom_by_name_for_display (){return 0;}
const gchar *         gdk_x11_get_xatom_name_for_display (){return 0;}

#ifndef GDK_MULTIHEAD_SAFE
Atom                  gdk_x11_atom_to_xatom     (){return 0;}
GdkAtom               gdk_x11_xatom_to_atom     (){return 0;}
Atom                  gdk_x11_get_xatom_by_name (){return 0;}
const gchar *         gdk_x11_get_xatom_name    (){return 0;}
#endif

void        gdk_x11_display_grab              (){}
void        gdk_x11_display_ungrab            (){}
void        gdk_x11_register_standard_event_type (){}

#if !defined(GDK_DISABLE_DEPRECATED) || defined(GDK_COMPILATION)
gpointer             gdk_x11_font_get_xfont    (){return 0;}
#endif

#ifndef GDK_DISABLE_DEPRECATED
Display *            gdk_x11_font_get_xdisplay (){return 0;}
const char *         gdk_x11_font_get_name     (){return 0;}
#endif

void        gdk_x11_set_sm_client_id (){}

GdkWindow  *gdk_x11_window_foreign_new_for_display (){return 0;}
GdkWindow  *gdk_x11_window_lookup_for_display      (){return 0;}
gint     gdk_x11_display_text_property_to_text_list (){return 0;}
void     gdk_x11_free_text_list                     (){}
gint     gdk_x11_display_string_to_compound_text    (){return 0;}
gboolean gdk_x11_display_utf8_to_compound_text      (){return 0;}
void     gdk_x11_free_compound_text                 (){}
