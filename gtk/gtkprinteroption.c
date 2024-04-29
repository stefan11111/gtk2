/* GTK - The GIMP Toolkit
 * gtkprinteroption.c: Handling possible settings for a specific printer setting
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

#include "gtkprinteroption.h"


/*****************************************
 *            GtkPrinterOption           *
 *****************************************/


GtkPrinterOption *
gtk_printer_option_new (const char *name, const char *display_text,
			GtkPrinterOptionType type)
{
  return NULL;
}

void
gtk_printer_option_set (GtkPrinterOption *option,
			const char *value)
{
}

void
gtk_printer_option_set_boolean (GtkPrinterOption *option,
				gboolean value)
{
}

void
gtk_printer_option_set_has_conflict  (GtkPrinterOption *option,
				      gboolean  has_conflict)
{
}

void
gtk_printer_option_clear_has_conflict (GtkPrinterOption     *option)
{
}

void
gtk_printer_option_allocate_choices (GtkPrinterOption     *option,
				     int num)
{
}

void
gtk_printer_option_choices_from_array (GtkPrinterOption   *option,
				       int                 num_choices,
				       char               *choices[],
				       char              *choices_display[])
{
}

gboolean
gtk_printer_option_has_choice (GtkPrinterOption     *option,
			       const char           *choice)
{
  return 0;
}

void
gtk_printer_option_set_activates_default (GtkPrinterOption *option,
					  gboolean          activates)
{
}

gboolean
gtk_printer_option_get_activates_default (GtkPrinterOption *option)
{
  return 0;
}


#define __GTK_PRINTER_OPTION_C__

