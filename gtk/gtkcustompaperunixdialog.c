/* GtkCustomPaperUnixDialog
 * Copyright (C) 2006 Alexander Larsson <alexl@redhat.com>
 * Copyright © 2006, 2007, 2008 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gtkprivate.h"

#include "gtkliststore.h"

GtkUnit
_gtk_print_get_default_user_units (void)
{
  return GTK_UNIT_MM;
}

GList *
_gtk_load_custom_papers (void)
{
  return NULL;
}

void
_gtk_print_load_custom_papers (GtkListStore *store)
{
}

void
_gtk_print_save_custom_papers (GtkListStore *store)
{
}

GtkWidget *
_gtk_custom_paper_unix_dialog_new (GtkWindow   *parent,
				  const gchar *title)
{
  return NULL;
}

#define __GTK_CUSTOM_PAPER_UNIX_DIALOG_C__

