/* GTK - The GIMP Toolkit
 * gtkpapersize.c: Paper Size
 * Copyright (C) 2006, Red Hat, Inc.
 * Copyright © 2006, 2007 Christian Persch
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

#include "gtkpapersize.h"
#include "gtkprintoperation.h"  /* for GtkPrintError */
#include "gtkintl.h"


GType
gtk_paper_size_get_type (void)
{
  return 0;
}

/**
 * gtk_paper_size_new:
 * @name: (allow-none): a paper size name, or %NULL
 *
 * Creates a new #GtkPaperSize object by parsing a
 * <ulink url="ftp://ftp.pwg.org/pub/pwg/candidates/cs-pwgmsn10-20020226-5101.1.pdf">PWG 5101.1-2002</ulink>
 * paper name.
 *
 * If @name is %NULL, the default paper size is returned,
 * see gtk_paper_size_get_default().
 * 
 * Return value: a new #GtkPaperSize, use gtk_paper_size_free()
 * to free it
 *
 * Since: 2.10
 */
GtkPaperSize *
gtk_paper_size_new (const gchar *name)
{
  return NULL;
}

/**
 * gtk_paper_size_new_from_ppd:
 * @ppd_name: a PPD paper name
 * @ppd_display_name: the corresponding human-readable name
 * @width: the paper width, in points
 * @height: the paper height in points
 * 
 * Creates a new #GtkPaperSize object by using 
 * PPD information. 
 * 
 * If @ppd_name is not a recognized PPD paper name, 
 * @ppd_display_name, @width and @height are used to 
 * construct a custom #GtkPaperSize object.
 *
 * Return value: a new #GtkPaperSize, use gtk_paper_size_free()
 * to free it
 *
 * Since: 2.10
 */
GtkPaperSize *
gtk_paper_size_new_from_ppd (const gchar *ppd_name,
			     const gchar *ppd_display_name,
			     gdouble      width,
			     gdouble      height)
{
  return NULL;
}

/**
 * gtk_paper_size_new_custom:
 * @name: the paper name 
 * @display_name: the human-readable name
 * @width: the paper width, in units of @unit
 * @height: the paper height, in units of @unit
 * @unit: the unit for @width and @height
 * 
 * Creates a new #GtkPaperSize object with the
 * given parameters.
 * 
 * Return value: a new #GtkPaperSize object, use gtk_paper_size_free()
 * to free it
 *
 * Since: 2.10
 */
GtkPaperSize *
gtk_paper_size_new_custom (const gchar *name, 
			   const gchar *display_name,
			   gdouble      width, 
			   gdouble      height, 
			   GtkUnit      unit)
{
  return NULL;
}

/**
 * gtk_paper_size_copy:
 * @other: a #GtkPaperSize
 * 
 * Copies an existing #GtkPaperSize.
 * 
 * Return value: a copy of @other
 *
 * Since: 2.10
 */
GtkPaperSize *
gtk_paper_size_copy (GtkPaperSize *other)
{
  return NULL;
}

/**
 * gtk_paper_size_free:
 * @size: a #GtkPaperSize
 * 
 * Free the given #GtkPaperSize object.
 *
 * Since: 2.10
 */
void
gtk_paper_size_free (GtkPaperSize *size)
{
}

/**
 * gtk_paper_size_is_equal:
 * @size1: a #GtkPaperSize object
 * @size2: another #GtkPaperSize object
 * 
 * Compares two #GtkPaperSize objects.
 * 
 * Return value: %TRUE, if @size1 and @size2 
 * represent the same paper size
 *
 * Since: 2.10
 */
gboolean
gtk_paper_size_is_equal (GtkPaperSize *size1,
			 GtkPaperSize *size2)
{
  return 0;
}

GList * _gtk_load_custom_papers (void);

/**
 * gtk_paper_size_get_paper_sizes:
 * @include_custom: whether to include custom paper sizes
 *     as defined in the page setup dialog
 *
 * Creates a list of known paper sizes.
 *
 * Return value:  (element-type GtkPaperSize) (transfer full): a newly allocated list of newly
 *    allocated #GtkPaperSize objects
 *
 * Since: 2.12
 */
GList *
gtk_paper_size_get_paper_sizes (gboolean include_custom)
{
  return NULL;
}


/**
 * gtk_paper_size_get_name:
 * @size: a #GtkPaperSize object
 * 
 * Gets the name of the #GtkPaperSize.
 * 
 * Return value: the name of @size
 *
 * Since: 2.10
 */
const gchar *
gtk_paper_size_get_name (GtkPaperSize *size)
{
  return NULL;
}

/**
 * gtk_paper_size_get_display_name:
 * @size: a #GtkPaperSize object
 * 
 * Gets the human-readable name of the #GtkPaperSize.
 * 
 * Return value: the human-readable name of @size
 *
 * Since: 2.10
 */
const gchar *
gtk_paper_size_get_display_name (GtkPaperSize *size)
{
  return NULL;
}

/**
 * gtk_paper_size_get_ppd_name:
 * @size: a #GtkPaperSize object
 * 
 * Gets the PPD name of the #GtkPaperSize, which
 * may be %NULL.
 * 
 * Return value: the PPD name of @size
 *
 * Since: 2.10
 */
const gchar *
gtk_paper_size_get_ppd_name (GtkPaperSize *size)
{
  return NULL;
}

/**
 * gtk_paper_size_get_width:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the paper width of the #GtkPaperSize, in 
 * units of @unit.
 * 
 * Return value: the paper width 
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_width (GtkPaperSize *size, 
			  GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_get_height:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the paper height of the #GtkPaperSize, in 
 * units of @unit.
 * 
 * Return value: the paper height 
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_height (GtkPaperSize *size, 
			   GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_is_custom:
 * @size: a #GtkPaperSize object
 * 
 * Returns %TRUE if @size is not a standard paper size.
 * 
 * Return value: whether @size is a custom paper size.
 **/
gboolean
gtk_paper_size_is_custom (GtkPaperSize *size)
{
  return 0;
}

/**
 * gtk_paper_size_set_size:
 * @size: a custom #GtkPaperSize object
 * @width: the new width in units of @unit
 * @height: the new height in units of @unit
 * @unit: the unit for @width and @height
 * 
 * Changes the dimensions of a @size to @width x @height.
 *
 * Since: 2.10
 */
void
gtk_paper_size_set_size (GtkPaperSize *size, 
			 gdouble       width, 
			 gdouble       height, 
			 GtkUnit       unit)
{
}

/**
 * gtk_paper_size_get_default:
 *
 * Returns the name of the default paper size, which 
 * depends on the current locale.  
 * 
 * Return value: the name of the default paper size. The string
 * is owned by GTK+ and should not be modified.
 * 
 * Since: 2.10

 */
const gchar *
gtk_paper_size_get_default (void)
{
  return NULL;
}

/* These get the default margins used for the paper size. Its
 * larger than most printers margins, so that it will be within
 * the imageble area on any printer.
 *
 * I've taken the actual values used from the OSX page setup dialog.
 * I'm not sure exactly where they got these values for, but might
 * correspond to this (from ghostscript docs):
 * 
 * All DeskJets have 0.5 inches (1.27cm) of unprintable bottom margin,
 * due to the mechanical arrangement used to grab the paper. Side margins
 * are approximately 0.25 inches (0.64cm) for U.S. letter paper, and 0.15
 * inches (0.38cm) for A4.
 */

/**
 * gtk_paper_size_get_default_top_margin:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the default top margin for the #GtkPaperSize.
 * 
 * Return value: the default top margin
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_default_top_margin (GtkPaperSize *size, 
				       GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_get_default_bottom_margin:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the default bottom margin for the #GtkPaperSize.
 * 
 * Return value: the default bottom margin
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_default_bottom_margin (GtkPaperSize *size, 
					  GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_get_default_left_margin:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the default left margin for the #GtkPaperSize.
 * 
 * Return value: the default left margin
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_default_left_margin (GtkPaperSize *size, 
					GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_get_default_right_margin:
 * @size: a #GtkPaperSize object
 * @unit: the unit for the return value
 * 
 * Gets the default right margin for the #GtkPaperSize.
 * 
 * Return value: the default right margin
 *
 * Since: 2.10
 */
gdouble
gtk_paper_size_get_default_right_margin (GtkPaperSize *size, 
					 GtkUnit       unit)
{
  return 0;
}

/**
 * gtk_paper_size_new_from_key_file:
 * @key_file: the #GKeyFile to retrieve the papersize from
 * @group_name: the name ofthe group in the key file to read,
 *     or %NULL to read the first group
 * @error: (allow-none): return location for an error, or %NULL
 *
 * Reads a paper size from the group @group_name in the key file
 * @key_file. 
 *
 * Returns: a new #GtkPaperSize object with the restored
 *          paper size, or %NULL if an error occurred.
 *
 * Since: 2.12
 */
GtkPaperSize *
gtk_paper_size_new_from_key_file (GKeyFile    *key_file,
				  const gchar *group_name,
				  GError     **error)
{
  g_set_error_literal (error,
                       GTK_PRINT_ERROR,
                       GTK_PRINT_ERROR_INVALID_FILE,
                       "Printing is not supported");
  return NULL;
}

/**
 * gtk_paper_size_to_key_file:
 * @size: a #GtkPaperSize
 * @key_file: the #GKeyFile to save the paper size to
 * @group_name: the group to add the settings to in @key_file
 *
 * This function adds the paper size from @size to @key_file.
 *
 * Since: 2.12
 */
void
gtk_paper_size_to_key_file (GtkPaperSize *size,
			    GKeyFile     *key_file,
			    const gchar  *group_name)
{
}


#define __GTK_PAPER_SIZE_C__

