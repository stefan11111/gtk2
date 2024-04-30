/* GTK - The GIMP Toolkit
 * gtkprintoperation.c: Print Operation
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

#include "gtkprintoperation-private.h"

GType
gtk_print_operation_get_type (void)
{
  return 0;
}

/**
 * gtk_print_error_quark:
 *
 * Registers an error quark for #GtkPrintOperation if necessary.
 * 
 * Return value: The error quark used for #GtkPrintOperation errors.
 *
 * Since: 2.10
 **/
GQuark     
gtk_print_error_quark (void)
{
  return 0;
}

GtkPrintOperation *
gtk_print_operation_new (void)
{
  return NULL;
}

void
gtk_print_operation_set_default_page_setup (GtkPrintOperation *op,
					    GtkPageSetup      *default_page_setup)
{
}

GtkPageSetup *
gtk_print_operation_get_default_page_setup (GtkPrintOperation *op)
{
  return NULL;
}

void
gtk_print_operation_set_print_settings (GtkPrintOperation *op,
					GtkPrintSettings  *print_settings)
{
}

GtkPrintSettings *
gtk_print_operation_get_print_settings (GtkPrintOperation *op)
{
  return NULL;
}

void
gtk_print_operation_set_job_name (GtkPrintOperation *op,
				  const gchar       *job_name)
{
}

void
gtk_print_operation_set_n_pages (GtkPrintOperation *op,
				 gint               n_pages)
{
}

void
gtk_print_operation_set_current_page (GtkPrintOperation *op,
				      gint               current_page)
{
}

void
gtk_print_operation_set_use_full_page (GtkPrintOperation *op,
				       gboolean           full_page)
{
}

void
gtk_print_operation_set_unit (GtkPrintOperation *op,
			      GtkUnit            unit)
{
}

void
gtk_print_operation_set_track_print_status (GtkPrintOperation  *op,
					    gboolean            track_status)
{
}

void
_gtk_print_operation_set_status (GtkPrintOperation *op,
				 GtkPrintStatus     status,
				 const gchar       *string)
{
}

GtkPrintStatus
gtk_print_operation_get_status (GtkPrintOperation *op)
{
  return GTK_PRINT_STATUS_FINISHED_ABORTED;
}

const gchar *
gtk_print_operation_get_status_string (GtkPrintOperation *op)
{
  return "";
}

gboolean
gtk_print_operation_is_finished (GtkPrintOperation *op)
{
  return 1;
}

void
gtk_print_operation_set_show_progress (GtkPrintOperation  *op,
				       gboolean            show_progress)
{
}

void
gtk_print_operation_set_allow_async (GtkPrintOperation  *op,
				     gboolean            allow_async)
{
}

void
gtk_print_operation_set_custom_tab_label (GtkPrintOperation  *op,
					  const gchar        *label)
{
}

void
gtk_print_operation_set_export_filename (GtkPrintOperation *op,
					 const gchar       *filename)
{
}

void
gtk_print_operation_set_defer_drawing (GtkPrintOperation *op)
{
}

void
gtk_print_operation_set_embed_page_setup (GtkPrintOperation  *op,
                                          gboolean            embed)
{
}

gboolean
gtk_print_operation_get_embed_page_setup (GtkPrintOperation *op)
{
  return 0;
}

void
gtk_print_operation_draw_page_finish (GtkPrintOperation *op)
{
}

void
gtk_print_operation_get_error (GtkPrintOperation  *op,
			       GError            **error)
{
}

GtkPrintOperationResult
gtk_print_operation_run (GtkPrintOperation        *op,
			 GtkPrintOperationAction   action,
			 GtkWindow                *parent,
			 GError                  **error)
{
  return GTK_PRINT_OPERATION_RESULT_CANCEL;
}

void
gtk_print_operation_cancel (GtkPrintOperation *op)
{
}

void
gtk_print_operation_set_support_selection (GtkPrintOperation  *op,
                                           gboolean            support_selection)
{
}

gboolean
gtk_print_operation_get_support_selection (GtkPrintOperation *op)
{
  return 0;
}

void
gtk_print_operation_set_has_selection (GtkPrintOperation  *op,
                                       gboolean            has_selection)
{
}

gboolean
gtk_print_operation_get_has_selection (GtkPrintOperation *op)
{
  return 0;
}

gint
gtk_print_operation_get_n_pages_to_print (GtkPrintOperation *op)
{
  return 0;
}

#define __GTK_PRINT_OPERATION_C__

