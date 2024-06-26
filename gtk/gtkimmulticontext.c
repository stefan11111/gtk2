/* GTK - The GIMP Toolkit
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"

#include <string.h>
#include <locale.h>

#include "gtkimmulticontext.h"
#include "gtkmain.h"
#include "gtkradiomenuitem.h"
#include "gtkintl.h"
#include "gtkprivate.h" /* To get redefinition of GTK_LOCALE_DIR on Win32 */


#define NONE_ID "gtk-im-context-none"

struct _GtkIMMulticontextPrivate
{
  GdkWindow *client_window;
  GdkRectangle cursor_location;
  gchar *context_id;

  guint use_preedit : 1;
  guint have_cursor_location : 1;
  guint focus_in : 1;
};

static void     gtk_im_multicontext_finalize           (GObject                 *object);

static void     gtk_im_multicontext_set_slave          (GtkIMMulticontext       *multicontext,
							GtkIMContext            *slave,
							gboolean                 finalizing);

static void     gtk_im_multicontext_set_client_window  (GtkIMContext            *context,
							GdkWindow               *window);
static void     gtk_im_multicontext_get_preedit_string (GtkIMContext            *context,
							gchar                  **str,
							PangoAttrList          **attrs,
							gint                   *cursor_pos);
static gboolean gtk_im_multicontext_filter_keypress    (GtkIMContext            *context,
							GdkEventKey             *event);
static void     gtk_im_multicontext_focus_in           (GtkIMContext            *context);
static void     gtk_im_multicontext_focus_out          (GtkIMContext            *context);
static void     gtk_im_multicontext_reset              (GtkIMContext            *context);
static void     gtk_im_multicontext_set_cursor_location (GtkIMContext            *context,
							GdkRectangle		*area);
static void     gtk_im_multicontext_set_use_preedit    (GtkIMContext            *context,
							gboolean                 use_preedit);
static gboolean gtk_im_multicontext_get_surrounding    (GtkIMContext            *context,
							gchar                  **text,
							gint                    *cursor_index);
static void     gtk_im_multicontext_set_surrounding    (GtkIMContext            *context,
							const char              *text,
							gint                     len,
							gint                     cursor_index);

static void     gtk_im_multicontext_preedit_start_cb        (GtkIMContext      *slave,
							     GtkIMMulticontext *multicontext);
static void     gtk_im_multicontext_preedit_end_cb          (GtkIMContext      *slave,
							     GtkIMMulticontext *multicontext);
static void     gtk_im_multicontext_preedit_changed_cb      (GtkIMContext      *slave,
							     GtkIMMulticontext *multicontext);
static void     gtk_im_multicontext_commit_cb               (GtkIMContext      *slave,
							     const gchar       *str,
							     GtkIMMulticontext *multicontext);
static gboolean gtk_im_multicontext_retrieve_surrounding_cb (GtkIMContext      *slave,
							     GtkIMMulticontext *multicontext);
static gboolean gtk_im_multicontext_delete_surrounding_cb   (GtkIMContext      *slave,
							     gint               offset,
							     gint               n_chars,
							     GtkIMMulticontext *multicontext);

static const gchar *global_context_id = NULL;

G_DEFINE_TYPE_WITH_PRIVATE (GtkIMMulticontext, gtk_im_multicontext, GTK_TYPE_IM_CONTEXT)

static void
gtk_im_multicontext_class_init (GtkIMMulticontextClass *class)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (class);
  GtkIMContextClass *im_context_class = GTK_IM_CONTEXT_CLASS (class);
  
  im_context_class->set_client_window = gtk_im_multicontext_set_client_window;
  im_context_class->get_preedit_string = gtk_im_multicontext_get_preedit_string;
  im_context_class->filter_keypress = gtk_im_multicontext_filter_keypress;
  im_context_class->focus_in = gtk_im_multicontext_focus_in;
  im_context_class->focus_out = gtk_im_multicontext_focus_out;
  im_context_class->reset = gtk_im_multicontext_reset;
  im_context_class->set_cursor_location = gtk_im_multicontext_set_cursor_location;
  im_context_class->set_use_preedit = gtk_im_multicontext_set_use_preedit;
  im_context_class->set_surrounding = gtk_im_multicontext_set_surrounding;
  im_context_class->get_surrounding = gtk_im_multicontext_get_surrounding;

  gobject_class->finalize = gtk_im_multicontext_finalize;
}

static void
gtk_im_multicontext_init (GtkIMMulticontext *multicontext)
{
  multicontext->slave = NULL;
  
  multicontext->priv = gtk_im_multicontext_get_instance_private (multicontext);
  multicontext->priv->use_preedit = TRUE;
  multicontext->priv->have_cursor_location = FALSE;
  multicontext->priv->focus_in = FALSE;
}

/**
 * gtk_im_multicontext_new:
 *
 * Creates a new #GtkIMMulticontext.
 *
 * Returns: a new #GtkIMMulticontext.
 **/
GtkIMContext *
gtk_im_multicontext_new (void)
{
  return g_object_new (GTK_TYPE_IM_MULTICONTEXT, NULL);
}

static void
gtk_im_multicontext_finalize (GObject *object)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (object);
  
  gtk_im_multicontext_set_slave (multicontext, NULL, TRUE);
  g_free (multicontext->context_id);
  g_free (multicontext->priv->context_id);

  G_OBJECT_CLASS (gtk_im_multicontext_parent_class)->finalize (object);
}

static void
gtk_im_multicontext_set_slave (GtkIMMulticontext *multicontext,
			       GtkIMContext      *slave,
			       gboolean           finalizing)
{
  GtkIMMulticontextPrivate *priv = multicontext->priv;
  gboolean need_preedit_changed = FALSE;
  
  if (multicontext->slave)
    {
      if (!finalizing)
	gtk_im_context_reset (multicontext->slave);
      
      g_signal_handlers_disconnect_by_func (multicontext->slave,
					    gtk_im_multicontext_preedit_start_cb,
					    multicontext);
      g_signal_handlers_disconnect_by_func (multicontext->slave,
					    gtk_im_multicontext_preedit_end_cb,
					    multicontext);
      g_signal_handlers_disconnect_by_func (multicontext->slave,
					    gtk_im_multicontext_preedit_changed_cb,
					    multicontext);
      g_signal_handlers_disconnect_by_func (multicontext->slave,
					    gtk_im_multicontext_commit_cb,
					    multicontext);

      g_object_unref (multicontext->slave);
      multicontext->slave = NULL;

      if (!finalizing)
	need_preedit_changed = TRUE;
    }
  
  multicontext->slave = slave;

  if (multicontext->slave)
    {
      g_object_ref (multicontext->slave);

      g_signal_connect (multicontext->slave, "preedit-start",
			G_CALLBACK (gtk_im_multicontext_preedit_start_cb),
			multicontext);
      g_signal_connect (multicontext->slave, "preedit-end",
			G_CALLBACK (gtk_im_multicontext_preedit_end_cb),
			multicontext);
      g_signal_connect (multicontext->slave, "preedit-changed",
			G_CALLBACK (gtk_im_multicontext_preedit_changed_cb),
			multicontext);
      g_signal_connect (multicontext->slave, "commit",
			G_CALLBACK (gtk_im_multicontext_commit_cb),
			multicontext);
      g_signal_connect (multicontext->slave, "retrieve-surrounding",
			G_CALLBACK (gtk_im_multicontext_retrieve_surrounding_cb),
			multicontext);
      g_signal_connect (multicontext->slave, "delete-surrounding",
			G_CALLBACK (gtk_im_multicontext_delete_surrounding_cb),
			multicontext);
      
      if (!priv->use_preedit)	/* Default is TRUE */
	gtk_im_context_set_use_preedit (slave, FALSE);
      if (priv->client_window)
	gtk_im_context_set_client_window (slave, priv->client_window);
      if (priv->have_cursor_location)
	gtk_im_context_set_cursor_location (slave, &priv->cursor_location);
      if (priv->focus_in)
	gtk_im_context_focus_in (slave);
    }

  if (need_preedit_changed)
    g_signal_emit_by_name (multicontext, "preedit-changed");
}

static const gchar *
get_effective_context_id (GtkIMMulticontext *multicontext)
{
  if (multicontext->priv->context_id)
    return multicontext->priv->context_id;

  if (!global_context_id)
    return "";

  return global_context_id;
}

static GtkIMContext *
gtk_im_multicontext_get_slave (GtkIMMulticontext *multicontext)
{
  if (g_strcmp0 (multicontext->context_id, get_effective_context_id (multicontext)) != 0)
    gtk_im_multicontext_set_slave (multicontext, NULL, FALSE);

  if (!multicontext->slave)
    {
      g_free (multicontext->context_id);

      multicontext->context_id = g_strdup (get_effective_context_id (multicontext));

      if (g_strcmp0 (multicontext->context_id, NONE_ID) == 0)
        return NULL;
    }

  return multicontext->slave;
}

static void
im_module_setting_changed (GtkSettings *settings, 
                           gpointer     data)
{
  global_context_id = NULL;
}


static void
gtk_im_multicontext_set_client_window (GtkIMContext *context,
				       GdkWindow    *window)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave;
  GdkScreen *screen;
  GtkSettings *settings;
  gboolean connected;

  multicontext->priv->client_window = window;

  if (window)
    {
      screen = gdk_window_get_screen (window);
      settings = gtk_settings_get_for_screen (screen);

      connected = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (settings),
                                                      "gtk-im-module-connected"));
      if (!connected)
        {
          g_signal_connect (settings, "notify::gtk-im-module",
                            G_CALLBACK (im_module_setting_changed), NULL);
          g_object_set_data (G_OBJECT (settings), "gtk-im-module-connected",
                             GINT_TO_POINTER (TRUE));

          global_context_id = NULL;
        }
    }

  slave = gtk_im_multicontext_get_slave (multicontext);
  if (slave)
    gtk_im_context_set_client_window (slave, window);
}

static void
gtk_im_multicontext_get_preedit_string (GtkIMContext   *context,
					gchar         **str,
					PangoAttrList **attrs,
					gint           *cursor_pos)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  if (slave)
    gtk_im_context_get_preedit_string (slave, str, attrs, cursor_pos);
  else
    {
      if (str)
	*str = g_strdup ("");
      if (attrs)
	*attrs = pango_attr_list_new ();
    }
}

static gboolean
gtk_im_multicontext_filter_keypress (GtkIMContext *context,
				     GdkEventKey  *event)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  if (slave)
    return gtk_im_context_filter_keypress (slave, event);
  else if (event->type == GDK_KEY_PRESS &&
           (event->state & GTK_NO_TEXT_INPUT_MOD_MASK) == 0)
    {
      gunichar ch;

      ch = gdk_keyval_to_unicode (event->keyval);
      if (ch != 0)
        {
          gint len;
          gchar buf[10];

          len = g_unichar_to_utf8 (ch, buf);
          buf[len] = '\0';

          g_signal_emit_by_name (multicontext, "commit", buf);

          return TRUE;
        }
    }

  return FALSE;
}

static void
gtk_im_multicontext_focus_in (GtkIMContext   *context)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  multicontext->priv->focus_in = TRUE;

  if (slave)
    gtk_im_context_focus_in (slave);
}

static void
gtk_im_multicontext_focus_out (GtkIMContext   *context)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  multicontext->priv->focus_in = FALSE;
  
  if (slave)
    gtk_im_context_focus_out (slave);
}

static void
gtk_im_multicontext_reset (GtkIMContext   *context)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  if (slave)
    gtk_im_context_reset (slave);
}

static void
gtk_im_multicontext_set_cursor_location (GtkIMContext   *context,
					 GdkRectangle   *area)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  multicontext->priv->have_cursor_location = TRUE;
  multicontext->priv->cursor_location = *area;

  if (slave)
    gtk_im_context_set_cursor_location (slave, area);
}

static void
gtk_im_multicontext_set_use_preedit (GtkIMContext   *context,
				     gboolean	    use_preedit)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  use_preedit = use_preedit != FALSE;

  multicontext->priv->use_preedit = use_preedit;

  if (slave)
    gtk_im_context_set_use_preedit (slave, use_preedit);
}

static gboolean
gtk_im_multicontext_get_surrounding (GtkIMContext  *context,
				     gchar        **text,
				     gint          *cursor_index)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  if (slave)
    return gtk_im_context_get_surrounding (slave, text, cursor_index);
  else
    {
      if (text)
	*text = NULL;
      if (cursor_index)
	*cursor_index = 0;

      return FALSE;
    }
}

static void
gtk_im_multicontext_set_surrounding (GtkIMContext *context,
				     const char   *text,
				     gint          len,
				     gint          cursor_index)
{
  GtkIMMulticontext *multicontext = GTK_IM_MULTICONTEXT (context);
  GtkIMContext *slave = gtk_im_multicontext_get_slave (multicontext);

  if (slave)
    gtk_im_context_set_surrounding (slave, text, len, cursor_index);
}

static void
gtk_im_multicontext_preedit_start_cb   (GtkIMContext      *slave,
					GtkIMMulticontext *multicontext)
{
  g_signal_emit_by_name (multicontext, "preedit-start");
}

static void
gtk_im_multicontext_preedit_end_cb (GtkIMContext      *slave,
				    GtkIMMulticontext *multicontext)
{
  g_signal_emit_by_name (multicontext, "preedit-end");
}

static void
gtk_im_multicontext_preedit_changed_cb (GtkIMContext      *slave,
					GtkIMMulticontext *multicontext)
{
  g_signal_emit_by_name (multicontext, "preedit-changed");
}

static void
gtk_im_multicontext_commit_cb (GtkIMContext      *slave,
			       const gchar       *str,
			       GtkIMMulticontext *multicontext)
{
  g_signal_emit_by_name (multicontext, "commit", str);
}

static gboolean
gtk_im_multicontext_retrieve_surrounding_cb (GtkIMContext      *slave,
					     GtkIMMulticontext *multicontext)
{
  gboolean result;
  
  g_signal_emit_by_name (multicontext, "retrieve-surrounding", &result);

  return result;
}

static gboolean
gtk_im_multicontext_delete_surrounding_cb (GtkIMContext      *slave,
					   gint               offset,
					   gint               n_chars,
					   GtkIMMulticontext *multicontext)
{
  gboolean result;
  
  g_signal_emit_by_name (multicontext, "delete-surrounding",
			 offset, n_chars, &result);

  return result;
}

/**
 * gtk_im_multicontext_append_menuitems:
 * @context: a #GtkIMMulticontext
 * @menushell: a #GtkMenuShell
 * 
 * Add menuitems for various available input methods to a menu;
 * the menuitems, when selected, will switch the input method
 * for the context and the global default input method.
 **/
void
gtk_im_multicontext_append_menuitems (GtkIMMulticontext *context,
				      GtkMenuShell      *menushell)
{
}

/**
 * gtk_im_multicontext_get_context_id:
 * @context: a #GtkIMMulticontext
 *
 * Gets the id of the currently active slave of the @context.
 *
 * Returns: the id of the currently active slave
 *
 * Since: 2.16
 */
const char *
gtk_im_multicontext_get_context_id (GtkIMMulticontext *context)
{
  return context->context_id;
}

/**
 * gtk_im_multicontext_set_context_id:
 * @context: a #GtkIMMulticontext
 * @context_id: the id to use 
 *
 * Sets the context id for @context.
 *
 * This causes the currently active slave of @context to be
 * replaced by the slave corresponding to the new context id.
 *
 * Since: 2.16
 */
void
gtk_im_multicontext_set_context_id (GtkIMMulticontext *context,
                                    const char        *context_id)
{
  gtk_im_context_reset (GTK_IM_CONTEXT (context));
  g_free (context->priv->context_id);
  context->priv->context_id = g_strdup (context_id);
  gtk_im_multicontext_set_slave (context, NULL, FALSE);
}


#define __GTK_IM_MULTICONTEXT_C__

