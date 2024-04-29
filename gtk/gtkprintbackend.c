/* GTK - The GIMP Toolkit
 * gtkprintbackend.h: Abstract printer backend interfaces
 * Copyright (C) 2003, Red Hat, Inc.
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


#include "gtkprintbackend.h"


GQuark
gtk_print_backend_error_quark (void)
{
  return NULL;
}

/*****************************************
 *     GtkPrintBackendModule modules     *
 *****************************************/

/**
 * gtk_printer_backend_load_modules:
 *
 * Return value: (element-type GtkPrintBackend) (transfer container):
 */
GList *
gtk_print_backend_load_modules (void)
{
  return NULL;
}

/*****************************************
 *             GtkPrintBackend           *
 *****************************************/

void
gtk_print_backend_add_printer (GtkPrintBackend *backend,
			       GtkPrinter      *printer)
{
}

void
gtk_print_backend_remove_printer (GtkPrintBackend *backend,
				  GtkPrinter      *printer)
{
}

void
gtk_print_backend_set_list_done (GtkPrintBackend *backend)
{
}

/**
 * gtk_print_backend_get_printer_list:
 *
 * Return value: (element-type GtkPrinter) (transfer container):
 */
GList *
gtk_print_backend_get_printer_list (GtkPrintBackend *backend)
{
  return NULL;
}

gboolean
gtk_print_backend_printer_list_is_done (GtkPrintBackend *print_backend)
{
  return 1;
}

GtkPrinter *
gtk_print_backend_find_printer (GtkPrintBackend *backend,
                                const gchar     *printer_name)
{
  return NULL;
}

void
gtk_print_backend_print_stream (GtkPrintBackend        *backend,
                                GtkPrintJob            *job,
                                GIOChannel             *data_io,
                                GtkPrintJobCompleteFunc callback,
                                gpointer                user_data,
				GDestroyNotify          dnotify)
{
}

void 
gtk_print_backend_set_password (GtkPrintBackend  *backend,
                                gchar           **auth_info_required,
                                gchar           **auth_info)
{
}


void
gtk_print_backend_destroy (GtkPrintBackend *print_backend)
{
}


#define __GTK_PRINT_BACKEND_C__

