#include <gtk/gtk.h>

#ifdef GDK_WINDOWING_DIRECTFB
#define gdk_display gdk_display_directfb
#include <gdk/directfb/gdkdirectfb.h>
#undef gdk_display
#undef GDK_DISPLAY
#undef GDK_ROOT_WINDOW
#endif

#ifdef GDK_WINDOWING_X11
#define gdk_display gdk_display_x11
#include <gdk/x11/gdkx.h>
#undef gdk_display
#undef GDK_DISPLAY
#undef GDK_ROOT_WINDOW
#endif

int main() { return 0; }
