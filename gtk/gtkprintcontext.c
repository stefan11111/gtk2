/* GTK - The GIMP Toolkit
 * gtkprintcontext.c: Print Context
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
gtk_print_context_get_type (void)
{
  return 0;
}

GtkPrintContext *
_gtk_print_context_new (GtkPrintOperation *op)
{
  return NULL;
}

void
gtk_print_context_set_cairo_context (GtkPrintContext *context,
				     cairo_t         *cr,
				     double           dpi_x,
				     double           dpi_y)
{
}


void
_gtk_print_context_rotate_according_to_orientation (GtkPrintContext *context)
{
}

void
_gtk_print_context_translate_into_margin (GtkPrintContext *context)
{
}

void
_gtk_print_context_set_page_setup (GtkPrintContext *context,
				   GtkPageSetup    *page_setup)
{
}

cairo_t *
gtk_print_context_get_cairo_context (GtkPrintContext *context)
{
  return NULL;
}

GtkPageSetup *
gtk_print_context_get_page_setup (GtkPrintContext *context)
{
  return NULL;
}

gdouble
gtk_print_context_get_width (GtkPrintContext *context)
{
  return 0;
}

gdouble
gtk_print_context_get_height (GtkPrintContext *context)
{
  return 0;
}

gdouble
gtk_print_context_get_dpi_x (GtkPrintContext *context)
{
  return 0;
}

gdouble
gtk_print_context_get_dpi_y (GtkPrintContext *context)
{
  return 0;
}

gboolean
gtk_print_context_get_hard_margins (GtkPrintContext *context,
				    gdouble         *top,
				    gdouble         *bottom,
				    gdouble         *left,
				    gdouble         *right)
{
  return 0;
}

void
_gtk_print_context_set_hard_margins (GtkPrintContext *context,
				     gdouble          top,
				     gdouble          bottom,
				     gdouble          left,
				     gdouble          right)
{
}

PangoFontMap *
gtk_print_context_get_pango_fontmap (GtkPrintContext *context)
{
  return NULL;
}

PangoContext *
gtk_print_context_create_pango_context (GtkPrintContext *context)
{
  return NULL;
}

PangoLayout *
gtk_print_context_create_pango_layout (GtkPrintContext *context)
{
  return NULL;
}


#define __GTK_PRINT_CONTEXT_C__

