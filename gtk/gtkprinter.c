/* GtkPrinter
 * Copyright (C) 2006 John (J5) Palmieri  <johnp@redhat.com>
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

#include "gtkprinter.h"
#include "gtkprinter-private.h"

GType
gtk_printer_get_type (void)
{
  return 0;
}

GtkPrinter *
gtk_printer_new (const gchar     *name,
		 GtkPrintBackend *backend,
		 gboolean         virtual_)
{
  return NULL;
}

GtkPrintBackend *
gtk_printer_get_backend (GtkPrinter *printer)
{
  return NULL;
}

const gchar *
gtk_printer_get_name (GtkPrinter *printer)
{
  return NULL;
}

const gchar *
gtk_printer_get_description (GtkPrinter *printer)
{
  return NULL;
}

gboolean
gtk_printer_set_description (GtkPrinter  *printer,
			     const gchar *description)
{
  return 0;
}

const gchar *
gtk_printer_get_state_message (GtkPrinter *printer)
{
  return NULL;
}

gboolean
gtk_printer_set_state_message (GtkPrinter  *printer,
			       const gchar *message)
{
  return 0;
}

const gchar *
gtk_printer_get_location (GtkPrinter *printer)
{
  return NULL;
}

gboolean
gtk_printer_set_location (GtkPrinter  *printer,
			  const gchar *location)
{
  return 0;
}

const gchar *
gtk_printer_get_icon_name (GtkPrinter *printer)
{
  return NULL;
}

void
gtk_printer_set_icon_name (GtkPrinter  *printer,
			   const gchar *icon)
{
}

gint 
gtk_printer_get_job_count (GtkPrinter *printer)
{
  return 0;
}

gboolean
gtk_printer_set_job_count (GtkPrinter *printer,
			   gint        count)
{
  return 0;
}

gboolean
gtk_printer_has_details (GtkPrinter *printer)
{
  return 0;
}

void
gtk_printer_set_has_details (GtkPrinter *printer,
			     gboolean val)
{
}

gboolean
gtk_printer_is_active (GtkPrinter *printer)
{
  return 1;
}

void
gtk_printer_set_is_active (GtkPrinter *printer,
			   gboolean val)
{
}

gboolean
gtk_printer_is_paused (GtkPrinter *printer)
{
  return 1;
}

gboolean
gtk_printer_set_is_paused (GtkPrinter *printer,
			   gboolean    val)
{
  return 0;
}

gboolean
gtk_printer_is_accepting_jobs (GtkPrinter *printer)
{
  return 1;
}

gboolean
gtk_printer_set_is_accepting_jobs (GtkPrinter *printer,
				   gboolean val)
{
  return 0;
}

gboolean
gtk_printer_is_virtual (GtkPrinter *printer)
{
  return 1;
}

gboolean 
gtk_printer_accepts_pdf (GtkPrinter *printer)
{ 
  return 1;
}

void
gtk_printer_set_accepts_pdf (GtkPrinter *printer,
			     gboolean val)
{
}

gboolean 
gtk_printer_accepts_ps (GtkPrinter *printer)
{ 
  return 1;
}

void
gtk_printer_set_accepts_ps (GtkPrinter *printer,
			    gboolean val)
{
}

gboolean
gtk_printer_is_new (GtkPrinter *printer)
{
  return 0;
}

void
gtk_printer_set_is_new (GtkPrinter *printer,
			gboolean val)
{
}

gboolean
gtk_printer_is_default (GtkPrinter *printer)
{
  return 0;
}

void
gtk_printer_set_is_default (GtkPrinter *printer,
			    gboolean    val)
{
}

void
gtk_printer_request_details (GtkPrinter *printer)
{
}

GtkPrinterOptionSet *
_gtk_printer_get_options (GtkPrinter           *printer,
			  GtkPrintSettings     *settings,
			  GtkPageSetup         *page_setup,
			  GtkPrintCapabilities  capabilities)
{
  return NULL;
}

gboolean
_gtk_printer_mark_conflicts (GtkPrinter          *printer,
			     GtkPrinterOptionSet *options)
{
  return 0;
}
  
void
_gtk_printer_get_settings_from_options (GtkPrinter          *printer,
					GtkPrinterOptionSet *options,
					GtkPrintSettings    *settings)
{
}

void
_gtk_printer_prepare_for_print (GtkPrinter       *printer,
				GtkPrintJob      *print_job,
				GtkPrintSettings *settings,
				GtkPageSetup     *page_setup)
{
}

cairo_surface_t *
_gtk_printer_create_cairo_surface (GtkPrinter       *printer,
				   GtkPrintSettings *settings,
				   gdouble           width, 
				   gdouble           height,
				   GIOChannel       *cache_io)
{
  return NULL;
}

GList  *
gtk_printer_list_papers (GtkPrinter *printer)
{
  return NULL;
}

GtkPageSetup  *
gtk_printer_get_default_page_size (GtkPrinter *printer)
{
  return NULL;
}

gboolean
gtk_printer_get_hard_margins (GtkPrinter *printer,
			      gdouble    *top,
			      gdouble    *bottom,
			      gdouble    *left,
			      gdouble    *right)
{
  return NULL;
}

GtkPrintCapabilities
gtk_printer_get_capabilities (GtkPrinter *printer)
{
  return 0;
}

gint
gtk_printer_compare (GtkPrinter *a, 
                     GtkPrinter *b)
{
  return 0;
}

void
gtk_enumerate_printers (GtkPrinterFunc func,
			gpointer       data,
			GDestroyNotify destroy,
			gboolean       wait)
{
}

GType
gtk_print_capabilities_get_type (void)
{
  return 0;
}


#define __GTK_PRINTER_C__

