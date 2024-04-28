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

#include <stdlib.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include "gtkprivate.h"
#include "gtkaccelgroup.h"
#include "gtkimcontextsimple.h"
#include "gtksettings.h"
#include "gtkwidget.h"
#include "gtkintl.h"


#ifdef GDK_WINDOWING_WIN32
#include <win32/gdkwin32keys.h>
#endif

typedef struct _GtkComposeTable GtkComposeTable;
typedef struct _GtkComposeTableCompact GtkComposeTableCompact;

struct _GtkComposeTable 
{
  const guint16 *data;
  gint max_seq_len;
  gint n_seqs;
};

struct _GtkComposeTableCompact
{
  const guint16 *data;
  gint max_seq_len;
  gint n_index_size;
  gint n_index_stride;
};

G_DEFINE_TYPE (GtkIMContextSimple, gtk_im_context_simple, GTK_TYPE_IM_CONTEXT)

static void
gtk_im_context_simple_class_init (GtkIMContextSimpleClass *class)
{
}

static void
gtk_im_context_simple_init (GtkIMContextSimple *im_context_simple)
{
}

/** 
 * gtk_im_context_simple_new:
 * 
 * Creates a new #GtkIMContextSimple.
 *
 * Returns: a new #GtkIMContextSimple.
 **/
GtkIMContext *
gtk_im_context_simple_new (void)
{
  return g_object_new (GTK_TYPE_IM_CONTEXT_SIMPLE, NULL);
}

/* Checks if a keysym is a dead key. Dead key keysym values are defined in
 * ../gdk/gdkkeysyms.h and the first is GDK_dead_grave. As X.Org is updated,
 * more dead keys are added and we need to update the upper limit.
 * Currently, the upper limit is GDK_dead_dasia+1. The +1 has to do with 
 * a temporary issue in the X.Org header files. 
 * In future versions it will be just the keysym (no +1).
 */
#define IS_DEAD_KEY(k) \
    ((k) >= GDK_dead_grave && (k) <= (GDK_dead_dasia+1))


/**
 * gtk_im_context_simple_add_table:
 * @context_simple: A #GtkIMContextSimple
 * @data: the table 
 * @max_seq_len: Maximum length of a sequence in the table
 *               (cannot be greater than #GTK_MAX_COMPOSE_LEN)
 * @n_seqs: number of sequences in the table
 * 
 * Adds an additional table to search to the input context.
 * Each row of the table consists of @max_seq_len key symbols
 * followed by two #guint16 interpreted as the high and low
 * words of a #gunicode value. Tables are searched starting
 * from the last added.
 *
 * The table must be sorted in dictionary order on the
 * numeric value of the key symbol fields. (Values beyond
 * the length of the sequence should be zero.)
 **/
void
gtk_im_context_simple_add_table (GtkIMContextSimple *context_simple,
				 guint16            *data,
				 gint                max_seq_len,
				 gint                n_seqs)
{
  GtkComposeTable *table;

  g_return_if_fail (GTK_IS_IM_CONTEXT_SIMPLE (context_simple));
  g_return_if_fail (data != NULL);
  g_return_if_fail (max_seq_len <= GTK_MAX_COMPOSE_LEN);
  
  table = g_new (GtkComposeTable, 1);
  table->data = data;
  table->max_seq_len = max_seq_len;
  table->n_seqs = n_seqs;

  context_simple->tables = g_slist_prepend (context_simple->tables, table);
}

#define __GTK_IM_CONTEXT_SIMPLE_C__

