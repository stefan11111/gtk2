/* GTK - The GIMP Toolkit
 * gtkprintsettings.c: Print Settings
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

#include <gtk/gtk.h>
#include "gtkprintsettings.h"

GType
gtk_print_settings_get_type (void)
{
  return 0;
}

GtkPrintSettings *
gtk_print_settings_new (void)
{
  return NULL;
}

GtkPrintSettings *
gtk_print_settings_copy (GtkPrintSettings *other)
{
  return NULL;
}

const gchar *
gtk_print_settings_get (GtkPrintSettings *settings,
			const gchar      *key)
{
  return NULL;
}

void
gtk_print_settings_set (GtkPrintSettings *settings,
			const gchar      *key,
			const gchar      *value)
{
}

void
gtk_print_settings_unset (GtkPrintSettings *settings,
			  const gchar      *key)
{
}

gboolean        
gtk_print_settings_has_key (GtkPrintSettings *settings,
			    const gchar      *key)
{
  return 0;
}


gboolean
gtk_print_settings_get_bool (GtkPrintSettings *settings,
			     const gchar      *key)
{
  return 0;
}

void
gtk_print_settings_set_bool (GtkPrintSettings *settings,
			     const gchar      *key,
			     gboolean          value)
{
}

gdouble
gtk_print_settings_get_double_with_default (GtkPrintSettings *settings,
					    const gchar      *key,
					    gdouble           def)
{
  return 0;
}

gdouble
gtk_print_settings_get_double (GtkPrintSettings *settings,
			       const gchar      *key)
{
  return 0;
}

void
gtk_print_settings_set_double (GtkPrintSettings *settings,
			       const gchar      *key,
			       gdouble           value)
{
}

gdouble
gtk_print_settings_get_length (GtkPrintSettings *settings,
			       const gchar      *key,
			       GtkUnit           unit)
{
  return 0;
}

void
gtk_print_settings_set_length (GtkPrintSettings *settings,
			       const gchar      *key,
			       gdouble           value, 
			       GtkUnit           unit)
{
}

gint
gtk_print_settings_get_int_with_default (GtkPrintSettings *settings,
					 const gchar      *key,
					 gint              def)
{
  return 0;
}

gint
gtk_print_settings_get_int (GtkPrintSettings *settings,
			    const gchar      *key)
{
  return 0;
}

void
gtk_print_settings_set_int (GtkPrintSettings *settings,
			    const gchar      *key,
			    gint              value)
{
}

void
gtk_print_settings_foreach (GtkPrintSettings    *settings,
			    GtkPrintSettingsFunc func,
			    gpointer             user_data)
{
}

const gchar *
gtk_print_settings_get_printer (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_printer (GtkPrintSettings *settings,
				const gchar      *printer)
{
}

GtkPageOrientation
gtk_print_settings_get_orientation (GtkPrintSettings *settings)
{
  return GTK_PAGE_ORIENTATION_PORTRAIT;
}

void
gtk_print_settings_set_orientation (GtkPrintSettings   *settings,
				    GtkPageOrientation  orientation)
{
}

GtkPaperSize *
gtk_print_settings_get_paper_size (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_paper_size (GtkPrintSettings *settings,
				   GtkPaperSize     *paper_size)
{
}

gdouble
gtk_print_settings_get_paper_width (GtkPrintSettings *settings,
				    GtkUnit           unit)
{
  return 0;
}

void
gtk_print_settings_set_paper_width (GtkPrintSettings *settings,
				    gdouble           width, 
				    GtkUnit           unit)
{
}

gdouble
gtk_print_settings_get_paper_height (GtkPrintSettings *settings,
				     GtkUnit           unit)
{
  return 0;
}

void
gtk_print_settings_set_paper_height (GtkPrintSettings *settings,
				     gdouble           height, 
				     GtkUnit           unit)
{
}

gboolean
gtk_print_settings_get_use_color (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_use_color (GtkPrintSettings *settings,
				  gboolean          use_color)
{
}

gboolean
gtk_print_settings_get_collate (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_collate (GtkPrintSettings *settings,
				gboolean          collate)
{
}

gboolean
gtk_print_settings_get_reverse (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_reverse (GtkPrintSettings *settings,
				  gboolean        reverse)
{
}

GtkPrintDuplex
gtk_print_settings_get_duplex (GtkPrintSettings *settings)
{
  return GTK_PRINT_DUPLEX_SIMPLEX;
}

void
gtk_print_settings_set_duplex (GtkPrintSettings *settings,
			       GtkPrintDuplex    duplex)
{
}

GtkPrintQuality
gtk_print_settings_get_quality (GtkPrintSettings *settings)
{
  return GTK_PRINT_QUALITY_NORMAL;
}

void
gtk_print_settings_set_quality (GtkPrintSettings *settings,
				GtkPrintQuality   quality)
{
}

GtkPageSet
gtk_print_settings_get_page_set (GtkPrintSettings *settings)
{
  return GTK_PAGE_SET_ALL;
}

void
gtk_print_settings_set_page_set (GtkPrintSettings *settings,
				 GtkPageSet        page_set)
{
}

GtkNumberUpLayout
gtk_print_settings_get_number_up_layout (GtkPrintSettings *settings)
{
  return GTK_NUMBER_UP_LAYOUT_LEFT_TO_RIGHT_TOP_TO_BOTTOM;
}

void
gtk_print_settings_set_number_up_layout (GtkPrintSettings  *settings,
					 GtkNumberUpLayout  number_up_layout)
{
}

gint
gtk_print_settings_get_n_copies (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_n_copies (GtkPrintSettings *settings,
				 gint              num_copies)
{
}

gint
gtk_print_settings_get_number_up (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_number_up (GtkPrintSettings *settings,
				  gint              number_up)
{
}

gint
gtk_print_settings_get_resolution (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_resolution (GtkPrintSettings *settings,
				   gint              resolution)
{
}

gint
gtk_print_settings_get_resolution_x (GtkPrintSettings *settings)
{
  return 0;
}

gint
gtk_print_settings_get_resolution_y (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_resolution_xy (GtkPrintSettings *settings,
				      gint              resolution_x,
				      gint              resolution_y)
{
}

gdouble
gtk_print_settings_get_printer_lpi (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_printer_lpi (GtkPrintSettings *settings,
				    gdouble           lpi)
{
}

gdouble
gtk_print_settings_get_scale (GtkPrintSettings *settings)
{
  return 0;
}

void
gtk_print_settings_set_scale (GtkPrintSettings *settings,
			      gdouble           scale)
{
}

GtkPrintPages
gtk_print_settings_get_print_pages (GtkPrintSettings *settings)
{
  return GTK_PRINT_PAGES_ALL;
}

void
gtk_print_settings_set_print_pages (GtkPrintSettings *settings,
				    GtkPrintPages     pages)
{
}

GtkPageRange *
gtk_print_settings_get_page_ranges (GtkPrintSettings *settings,
				    gint             *num_ranges)
{
  return NULL;
}

void
gtk_print_settings_set_page_ranges  (GtkPrintSettings *settings,
				     GtkPageRange     *page_ranges,
				     gint              num_ranges)
{
}

const gchar *
gtk_print_settings_get_default_source (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_default_source (GtkPrintSettings *settings,
				       const gchar      *default_source)
{
}
     
const gchar *
gtk_print_settings_get_media_type (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_media_type (GtkPrintSettings *settings,
				   const gchar      *media_type)
{
}

const gchar *
gtk_print_settings_get_dither (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_dither (GtkPrintSettings *settings,
			       const gchar      *dither)
{
}

const gchar *
gtk_print_settings_get_finishings (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_finishings (GtkPrintSettings *settings,
				   const gchar      *finishings)
{
}

const gchar *
gtk_print_settings_get_output_bin (GtkPrintSettings *settings)
{
  return NULL;
}

void
gtk_print_settings_set_output_bin (GtkPrintSettings *settings,
				   const gchar      *output_bin)
{
}

gboolean
gtk_print_settings_load_file (GtkPrintSettings *settings,
                              const gchar      *file_name,
                              GError          **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

GtkPrintSettings *
gtk_print_settings_new_from_file (const gchar  *file_name,
			          GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

gboolean
gtk_print_settings_load_key_file (GtkPrintSettings *settings,
				  GKeyFile         *key_file,
				  const gchar      *group_name,
				  GError          **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

GtkPrintSettings *
gtk_print_settings_new_from_key_file (GKeyFile     *key_file,
				      const gchar  *group_name,
				      GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

gboolean
gtk_print_settings_to_file (GtkPrintSettings  *settings,
			    const gchar       *file_name,
			    GError           **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

void
gtk_print_settings_to_key_file (GtkPrintSettings  *settings,
			        GKeyFile          *key_file,
				const gchar       *group_name)
{
}


#define __GTK_PRINT_SETTINGS_C__

