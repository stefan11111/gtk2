/* GTK - The GIMP Toolkit
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
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

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#include "config.h"

#include "gtkorientable.h"
#include "gtkvbox.h"


/**
 * SECTION:gtkvbox
 * @Short_description: A vertical container box
 * @Title: GtkVBox
 * @See_also: #GtkHBox
 *
 * A #GtkVBox is a container that organizes child widgets into a single column.
 *
 * Use the #GtkBox packing interface to determine the arrangement,
 * spacing, height, and alignment of #GtkVBox children.
 *
 * All children are allocated the same width.
 */

G_DEFINE_TYPE (GtkVBox, gtk_vbox, GTK_TYPE_BOX)

static void
gtk_vbox_class_init (GtkVBoxClass *class)
{
}

static void
gtk_vbox_init (GtkVBox *vbox)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (vbox),
                                  GTK_ORIENTATION_VERTICAL);

  _gtk_box_set_old_defaults (GTK_BOX (vbox));
}

/**
 * gtk_vbox_new:
 * @homogeneous: %TRUE if all children are to be given equal space allotments.
 * @spacing: the number of pixels to place by default between children.
 *
 * Creates a new #GtkVBox.
 *
 * Returns: a new #GtkVBox.
 */
GtkWidget *
gtk_vbox_new (gboolean homogeneous,
	      gint     spacing)
{
  return g_object_new (GTK_TYPE_VBOX,
                       "spacing",     spacing,
                       "homogeneous", homogeneous ? TRUE : FALSE,
                       NULL);
}

#define __GTK_VBOX_C__

