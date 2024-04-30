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
#include "gtkprinter.h"
#include "gtkprinter-private.h"
#include "gtkprintbackend.h"

/**
 * gtk_print_job_new:
 * @title: the job title
 * @printer: a #GtkPrinter
 * @settings: a #GtkPrintSettings
 * @page_setup: a #GtkPageSetup
 *
 * Creates a new #GtkPrintJob.
 *
 * Return value: a new #GtkPrintJob
 *
 * Since: 2.10
 **/
GtkPrintJob *
gtk_print_job_new (const gchar      *title,
		   GtkPrinter       *printer,
		   GtkPrintSettings *settings,
		   GtkPageSetup     *page_setup)
{
  return NULL;
}

/**
 * gtk_print_job_get_settings:
 * @job: a #GtkPrintJob
 * 
 * Gets the #GtkPrintSettings of the print job.
 * 
 * Return value: (transfer none): the settings of @job
 *
 * Since: 2.10
 */
GtkPrintSettings *
gtk_print_job_get_settings (GtkPrintJob *job)
{
  return NULL;
}

/**
 * gtk_print_job_get_printer:
 * @job: a #GtkPrintJob
 * 
 * Gets the #GtkPrinter of the print job.
 * 
 * Return value: (transfer none): the printer of @job
 *
 * Since: 2.10
 */
GtkPrinter *
gtk_print_job_get_printer (GtkPrintJob *job)
{
  return NULL;
}

/**
 * gtk_print_job_get_title:
 * @job: a #GtkPrintJob
 * 
 * Gets the job title.
 * 
 * Return value: the title of @job
 *
 * Since: 2.10
 */
const gchar *
gtk_print_job_get_title (GtkPrintJob *job)
{
  return NULL;
}

/**
 * gtk_print_job_get_status:
 * @job: a #GtkPrintJob
 * 
 * Gets the status of the print job.
 * 
 * Return value: the status of @job
 *
 * Since: 2.10
 */
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

/**
 * gtk_print_job_set_source_file:
 * @job: a #GtkPrintJob
 * @filename: the file to be printed
 * @error: return location for errors
 * 
 * Make the #GtkPrintJob send an existing document to the 
 * printing system. The file can be in any format understood
 * by the platforms printing system (typically PostScript,
 * but on many platforms PDF may work too). See 
 * gtk_printer_accepts_pdf() and gtk_printer_accepts_ps().
 * 
 * Returns: %FALSE if an error occurred
 *
 * Since: 2.10
 **/
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

/**
 * gtk_print_job_get_surface:
 * @job: a #GtkPrintJob
 * @error: (allow-none): return location for errors, or %NULL
 * 
 * Gets a cairo surface onto which the pages of
 * the print job should be rendered.
 * 
 * Return value: (transfer none): the cairo surface of @job
 *
 * Since: 2.10
 **/
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

/**
 * gtk_print_job_set_track_print_status:
 * @job: a #GtkPrintJob
 * @track_status: %TRUE to track status after printing
 * 
 * If track_status is %TRUE, the print job will try to continue report
 * on the status of the print job in the printer queues and printer. This
 * can allow your application to show things like "out of paper" issues,
 * and when the print job actually reaches the printer.
 * 
 * This function is often implemented using some form of polling, so it should
 * not be enabled unless needed.
 *
 * Since: 2.10
 */
void
gtk_print_job_set_track_print_status (GtkPrintJob *job,
				      gboolean     track_status)
{
}

/**
 * gtk_print_job_get_track_print_status:
 * @job: a #GtkPrintJob
 *
 * Returns wheter jobs will be tracked after printing.
 * For details, see gtk_print_job_set_track_print_status().
 *
 * Return value: %TRUE if print job status will be reported after printing
 *
 * Since: 2.10
 */
gboolean
gtk_print_job_get_track_print_status (GtkPrintJob *job)
{
  return 1;
}

/**
 * gtk_print_job_send:
 * @job: a GtkPrintJob
 * @callback: function to call when the job completes or an error occurs
 * @user_data: user data that gets passed to @callback
 * @dnotify: destroy notify for @user_data
 * 
 * Sends the print job off to the printer.  
 * 
 * Since: 2.10
 **/
void
gtk_print_job_send (GtkPrintJob             *job,
                    GtkPrintJobCompleteFunc  callback,
                    gpointer                 user_data,
		    GDestroyNotify           dnotify)
{
}


#define __GTK_PRINT_JOB_C__

