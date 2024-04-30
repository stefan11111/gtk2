/* GTK - The GIMP Toolkit
 * gtkprintoperationpreview.c: Abstract print preview interface
 * Copyright (C) 2006, Red Hat, Inc.
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

#include "gtkprintoperationpreview.h"

static void gtk_print_operation_preview_base_init (gpointer g_iface);

GType
gtk_print_operation_preview_get_type (void)
{
  return 0;
}

void    
gtk_print_operation_preview_render_page (GtkPrintOperationPreview *preview,
					 gint			   page_nr)
{
}

void
gtk_print_operation_preview_end_preview (GtkPrintOperationPreview *preview)
{
}

gboolean
gtk_print_operation_preview_is_selected (GtkPrintOperationPreview *preview,
					 gint                      page_nr)
{
  return 0;
}


#define __GTK_PRINT_OPERATION_PREVIEW_C__

