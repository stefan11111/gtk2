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

GType
gtk_paper_size_get_type (void)
{
  return 0;
}

GtkPaperSize *
gtk_paper_size_new (const gchar *name)
{
  return NULL;
}

GtkPaperSize *
gtk_paper_size_new_from_ppd (const gchar *ppd_name,
			     const gchar *ppd_display_name,
			     gdouble      width,
			     gdouble      height)
{
  return NULL;
}

GtkPaperSize *
gtk_paper_size_new_custom (const gchar *name, 
			   const gchar *display_name,
			   gdouble      width, 
			   gdouble      height, 
			   GtkUnit      unit)
{
  return NULL;
}

GtkPaperSize *
gtk_paper_size_copy (GtkPaperSize *other)
{
  return NULL;
}

void
gtk_paper_size_free (GtkPaperSize *size)
{
}

gboolean
gtk_paper_size_is_equal (GtkPaperSize *size1,
			 GtkPaperSize *size2)
{
  return 0;
}

GList * _gtk_load_custom_papers (void);

GList *
gtk_paper_size_get_paper_sizes (gboolean include_custom)
{
  return NULL;
}

const gchar *
gtk_paper_size_get_name (GtkPaperSize *size)
{
  return NULL;
}

const gchar *
gtk_paper_size_get_display_name (GtkPaperSize *size)
{
  return NULL;
}

const gchar *
gtk_paper_size_get_ppd_name (GtkPaperSize *size)
{
  return NULL;
}

gdouble
gtk_paper_size_get_width (GtkPaperSize *size, 
			  GtkUnit       unit)
{
  return 0;
}

gdouble
gtk_paper_size_get_height (GtkPaperSize *size, 
			   GtkUnit       unit)
{
  return 0;
}

gboolean
gtk_paper_size_is_custom (GtkPaperSize *size)
{
  return 0;
}

void
gtk_paper_size_set_size (GtkPaperSize *size, 
			 gdouble       width, 
			 gdouble       height, 
			 GtkUnit       unit)
{
}

const gchar *
gtk_paper_size_get_default (void)
{
  return NULL;
}

gdouble
gtk_paper_size_get_default_top_margin (GtkPaperSize *size, 
				       GtkUnit       unit)
{
  return 0;
}

gdouble
gtk_paper_size_get_default_bottom_margin (GtkPaperSize *size, 
					  GtkUnit       unit)
{
  return 0;
}

gdouble
gtk_paper_size_get_default_left_margin (GtkPaperSize *size, 
					GtkUnit       unit)
{
  return 0;
}

gdouble
gtk_paper_size_get_default_right_margin (GtkPaperSize *size, 
					 GtkUnit       unit)
{
  return 0;
}

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

void
gtk_paper_size_to_key_file (GtkPaperSize *size,
			    GKeyFile     *key_file,
			    const gchar  *group_name)
{
}


#define __GTK_PAPER_SIZE_C__

