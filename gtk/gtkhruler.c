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

#undef GTK_DISABLE_DEPRECATED

#include "gtkhruler.h"
#include "gtkorientable.h"


G_DEFINE_TYPE (GtkHRuler, gtk_hruler, GTK_TYPE_RULER)

static void
gtk_hruler_class_init (GtkHRulerClass *klass)
{
}

static void
gtk_hruler_init (GtkHRuler *hruler)
{
  gtk_orientable_set_orientation (GTK_ORIENTABLE (hruler),
                                  GTK_ORIENTATION_HORIZONTAL);
}

GtkWidget *
gtk_hruler_new (void)
{
  return g_object_new (GTK_TYPE_HRULER, NULL);
}

#define __GTK_HRULER_C__

