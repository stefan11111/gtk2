/* GTK - The GIMP Toolkit
 * gtkpagesetup.c: Page Setup
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

#include "gtkpagesetup.h"
#include "gtkprintoperation.h" /* for GtkPrintError */
#include "gtkintl.h"
#include "gtktypebuiltins.h"


GType
gtk_page_setup_get_type (void)
{
  return 0;
}

GtkPageSetup *
gtk_page_setup_new (void)
{
  return NULL;
}

/**
 * gtk_page_setup_copy:
 * @other: the #GtkPageSetup to copy
 *
 * Copies a #GtkPageSetup.
 *
 * Return value: (transfer full): a copy of @other
 *
 * Since: 2.10
 */
GtkPageSetup *
gtk_page_setup_copy (GtkPageSetup *other)
{
  return other;
}

/**
 * gtk_page_setup_get_orientation:
 * @setup: a #GtkPageSetup
 * 
 * Gets the page orientation of the #GtkPageSetup.
 * 
 * Return value: the page orientation
 *
 * Since: 2.10
 */
GtkPageOrientation
gtk_page_setup_get_orientation (GtkPageSetup *setup)
{
  return 0;
}

/**
 * gtk_page_setup_set_orientation:
 * @setup: a #GtkPageSetup
 * @orientation: a #GtkPageOrientation value
 * 
 * Sets the page orientation of the #GtkPageSetup.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_orientation (GtkPageSetup       *setup,
				GtkPageOrientation  orientation)
{
}

/**
 * gtk_page_setup_get_paper_size:
 * @setup: a #GtkPageSetup
 * 
 * Gets the paper size of the #GtkPageSetup.
 * 
 * Return value: the paper size
 *
 * Since: 2.10
 */
GtkPaperSize *
gtk_page_setup_get_paper_size (GtkPageSetup *setup)
{
  return NULL;
}

/**
 * gtk_page_setup_set_paper_size:
 * @setup: a #GtkPageSetup
 * @size: a #GtkPaperSize 
 * 
 * Sets the paper size of the #GtkPageSetup without
 * changing the margins. See 
 * gtk_page_setup_set_paper_size_and_default_margins().
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_paper_size (GtkPageSetup *setup,
			       GtkPaperSize *size)
{
}

/**
 * gtk_page_setup_set_paper_size_and_default_margins:
 * @setup: a #GtkPageSetup
 * @size: a #GtkPaperSize 
 * 
 * Sets the paper size of the #GtkPageSetup and modifies
 * the margins according to the new paper size.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_paper_size_and_default_margins (GtkPageSetup *setup,
						   GtkPaperSize *size)
{
}

/**
 * gtk_page_setup_get_top_margin:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Gets the top margin in units of @unit.
 * 
 * Return value: the top margin
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_top_margin (GtkPageSetup *setup,
			       GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_set_top_margin:
 * @setup: a #GtkPageSetup
 * @margin: the new top margin in units of @unit
 * @unit: the units for @margin
 * 
 * Sets the top margin of the #GtkPageSetup.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_top_margin (GtkPageSetup *setup,
			       gdouble       margin,
			       GtkUnit       unit)
{
}

/**
 * gtk_page_setup_get_bottom_margin:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Gets the bottom margin in units of @unit.
 * 
 * Return value: the bottom margin
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_bottom_margin (GtkPageSetup *setup,
				  GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_set_bottom_margin:
 * @setup: a #GtkPageSetup
 * @margin: the new bottom margin in units of @unit
 * @unit: the units for @margin
 * 
 * Sets the bottom margin of the #GtkPageSetup.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_bottom_margin (GtkPageSetup *setup,
				  gdouble       margin,
				  GtkUnit       unit)
{
}

/**
 * gtk_page_setup_get_left_margin:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Gets the left margin in units of @unit.
 * 
 * Return value: the left margin
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_left_margin (GtkPageSetup *setup,
				GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_set_left_margin:
 * @setup: a #GtkPageSetup
 * @margin: the new left margin in units of @unit
 * @unit: the units for @margin
 * 
 * Sets the left margin of the #GtkPageSetup.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_left_margin (GtkPageSetup *setup,
				gdouble       margin,
				GtkUnit       unit)
{
}

/**
 * gtk_page_setup_get_right_margin:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Gets the right margin in units of @unit.
 * 
 * Return value: the right margin
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_right_margin (GtkPageSetup *setup,
				 GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_set_right_margin:
 * @setup: a #GtkPageSetup
 * @margin: the new right margin in units of @unit
 * @unit: the units for @margin
 * 
 * Sets the right margin of the #GtkPageSetup.
 *
 * Since: 2.10
 */
void
gtk_page_setup_set_right_margin (GtkPageSetup *setup,
				 gdouble       margin,
				 GtkUnit       unit)
{
}

/**
 * gtk_page_setup_get_paper_width:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Returns the paper width in units of @unit.
 * 
 * Note that this function takes orientation, but 
 * not margins into consideration. 
 * See gtk_page_setup_get_page_width().
 *
 * Return value: the paper width.
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_paper_width (GtkPageSetup *setup,
				GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_get_paper_height:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Returns the paper height in units of @unit.
 * 
 * Note that this function takes orientation, but 
 * not margins into consideration.
 * See gtk_page_setup_get_page_height().
 *
 * Return value: the paper height.
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_paper_height (GtkPageSetup *setup,
				 GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_get_page_width:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Returns the page width in units of @unit.
 * 
 * Note that this function takes orientation and
 * margins into consideration. 
 * See gtk_page_setup_get_paper_width().
 *
 * Return value: the page width.
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_page_width (GtkPageSetup *setup,
			       GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_get_page_height:
 * @setup: a #GtkPageSetup
 * @unit: the unit for the return value
 * 
 * Returns the page height in units of @unit.
 * 
 * Note that this function takes orientation and
 * margins into consideration. 
 * See gtk_page_setup_get_paper_height().
 *
 * Return value: the page height.
 *
 * Since: 2.10
 */
gdouble
gtk_page_setup_get_page_height (GtkPageSetup *setup,
				GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_page_setup_load_file:
 * @setup: a #GtkPageSetup
 * @file_name: the filename to read the page setup from
 * @error: (allow-none): return location for an error, or %NULL
 *
 * Reads the page setup from the file @file_name.
 * See gtk_page_setup_to_file().
 *
 * Return value: %TRUE on success
 *
 * Since: 2.14
 */
gboolean
gtk_page_setup_load_file (GtkPageSetup *setup,
                          const gchar  *file_name,
			  GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

/**
 * gtk_page_setup_new_from_file:
 * @file_name: the filename to read the page setup from
 * @error: (allow-none): return location for an error, or %NULL
 * 
 * Reads the page setup from the file @file_name. Returns a 
 * new #GtkPageSetup object with the restored page setup, 
 * or %NULL if an error occurred. See gtk_page_setup_to_file().
 *
 * Return value: the restored #GtkPageSetup
 * 
 * Since: 2.12
 */
GtkPageSetup *
gtk_page_setup_new_from_file (const gchar  *file_name,
			      GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

/**
 * gtk_page_setup_load_key_file:
 * @setup: a #GtkPageSetup
 * @key_file: the #GKeyFile to retrieve the page_setup from
 * @group_name: (allow-none): the name of the group in the key_file to read, or %NULL
 *              to use the default name "Page Setup"
 * @error: (allow-none): return location for an error, or %NULL
 * 
 * Reads the page setup from the group @group_name in the key file
 * @key_file.
 * 
 * Return value: %TRUE on success
 *
 * Since: 2.14
 */
gboolean
gtk_page_setup_load_key_file (GtkPageSetup *setup,
                              GKeyFile     *key_file,
                              const gchar  *group_name,
                              GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

/**
 * gtk_page_setup_new_from_key_file:
 * @key_file: the #GKeyFile to retrieve the page_setup from
 * @group_name: (allow-none): the name of the group in the key_file to read, or %NULL
 *              to use the default name "Page Setup"
 * @error: (allow-none): return location for an error, or %NULL
 *
 * Reads the page setup from the group @group_name in the key file
 * @key_file. Returns a new #GtkPageSetup object with the restored
 * page setup, or %NULL if an error occurred.
 *
 * Return value: the restored #GtkPageSetup
 *
 * Since: 2.12
 */
GtkPageSetup *
gtk_page_setup_new_from_key_file (GKeyFile     *key_file,
				  const gchar  *group_name,
				  GError      **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

/**
 * gtk_page_setup_to_file:
 * @setup: a #GtkPageSetup
 * @file_name: the file to save to
 * @error: (allow-none): return location for errors, or %NULL
 * 
 * This function saves the information from @setup to @file_name.
 * 
 * Return value: %TRUE on success
 *
 * Since: 2.12
 */
gboolean
gtk_page_setup_to_file (GtkPageSetup  *setup,
		        const char    *file_name,
			GError       **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return 0;
}

/**
 * gtk_page_setup_to_key_file:
 * @setup: a #GtkPageSetup
 * @key_file: the #GKeyFile to save the page setup to
 * @group_name: the group to add the settings to in @key_file, 
 *      or %NULL to use the default name "Page Setup"
 * 
 * This function adds the page setup from @setup to @key_file.
 * 
 * Since: 2.12
 */
void
gtk_page_setup_to_key_file (GtkPageSetup *setup,
			    GKeyFile     *key_file,
			    const gchar  *group_name)
{
}

#define __GTK_PAGE_SETUP_C__

