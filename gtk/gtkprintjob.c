/* GtkPrintJob
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

#include "gtkprintjob.h"

GtkPrintJob *
gtk_print_job_new (const gchar      *title,
		   GtkPrinter       *printer,
		   GtkPrintSettings *settings,
		   GtkPageSetup     *page_setup)
{
  return NULL;
}

GtkPrintSettings *
gtk_print_job_get_settings (GtkPrintJob *job)
{
  return NULL;
}

GtkPrinter *
gtk_print_job_get_printer (GtkPrintJob *job)
{
  return NULL;
}

const gchar *
gtk_print_job_get_title (GtkPrintJob *job)
{
  return NULL;
}

GtkPrintStatus
gtk_print_job_get_status (GtkPrintJob *job)
{
  return GTK_PRINT_STATUS_FINISHED;
}

void
gtk_print_job_set_status (GtkPrintJob   *job,
			  GtkPrintStatus status)
{
}

gboolean
gtk_print_job_set_source_file (GtkPrintJob *job,
			       const gchar *filename,
			       GError     **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

cairo_surface_t *
gtk_print_job_get_surface (GtkPrintJob  *job,
			   GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

void
gtk_print_job_set_track_print_status (GtkPrintJob *job,
				      gboolean     track_status)
{
}

gboolean
gtk_print_job_get_track_print_status (GtkPrintJob *job)
{
  return 1;
}

void
gtk_print_job_send (GtkPrintJob             *job,
                    GtkPrintJobCompleteFunc  callback,
                    gpointer                 user_data,
		    GDestroyNotify           dnotify)
{
}


#define __GTK_PRINT_JOB_C__

