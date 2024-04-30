/* GTK - The GIMP Toolkit
 * gtkprintoperation-unix.c: Print Operation Details for Unix 
 *                           and Unix-like platforms
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

void
_gtk_print_operation_platform_backend_launch_preview (GtkPrintOperation *op,
						      cairo_surface_t   *surface,
						      GtkWindow         *parent,
						      const gchar       *filename)
{
}

void
_gtk_print_operation_platform_backend_run_dialog_async (GtkPrintOperation          *op,
							gboolean                    show_dialog,
                                                        GtkWindow                  *parent,
							GtkPrintOperationPrintFunc  print_cb)
{
}

cairo_surface_t *
_gtk_print_operation_platform_backend_create_preview_surface (GtkPrintOperation *op,
							      GtkPageSetup      *page_setup,
							      gdouble           *dpi_x,
							      gdouble           *dpi_y,
							      gchar            **target)
{
  return NULL;
}

void
_gtk_print_operation_platform_backend_preview_start_page (GtkPrintOperation *op,
							  cairo_surface_t   *surface,
							  cairo_t           *cr)
{
}

void
_gtk_print_operation_platform_backend_preview_end_page (GtkPrintOperation *op,
							cairo_surface_t   *surface,
							cairo_t           *cr)
{
}

void
_gtk_print_operation_platform_backend_resize_preview_surface (GtkPrintOperation *op,
							      GtkPageSetup      *page_setup,
							      cairo_surface_t   *surface)
{
}


GtkPrintOperationResult
_gtk_print_operation_platform_backend_run_dialog (GtkPrintOperation *op,
						  gboolean           show_dialog,
						  GtkWindow         *parent,
						  gboolean          *do_print)
{
  return GTK_PRINT_OPERATION_RESULT_CANCEL;
}

GtkPageSetup *
gtk_print_run_page_setup_dialog (GtkWindow        *parent,
				 GtkPageSetup     *page_setup,
				 GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_run_page_setup_dialog_async (GtkWindow            *parent,
				       GtkPageSetup         *page_setup,
				       GtkPrintSettings     *settings,
				       GtkPageSetupDoneFunc  done_cb,
				       gpointer              data)
{
}

#define __GTK_PRINT_OPERATION_UNIX_C__

