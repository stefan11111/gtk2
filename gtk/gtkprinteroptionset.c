/* GTK - The GIMP Toolkit
 * gtkprintbackend.h: Abstract printer backend interfaces
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

#include "gtkprinteroptionset.h"


GtkPrinterOptionSet *
gtk_printer_option_set_new (void)
{
  return NULL;
}

void
gtk_printer_option_set_remove (GtkPrinterOptionSet *set,
			       GtkPrinterOption    *option)
{
}

void
gtk_printer_option_set_add (GtkPrinterOptionSet *set,
			    GtkPrinterOption    *option)
{
}

GtkPrinterOption *
gtk_printer_option_set_lookup (GtkPrinterOptionSet *set,
			       const char          *name)
{
  return NULL;
}

void
gtk_printer_option_set_clear_conflicts (GtkPrinterOptionSet *set)
{
}

GList *
gtk_printer_option_set_get_groups (GtkPrinterOptionSet *set)
{
  return NULL;
}

void
gtk_printer_option_set_foreach_in_group (GtkPrinterOptionSet     *set,
					 const char              *group,
					 GtkPrinterOptionSetFunc  func,
					 gpointer                 user_data)
{
}

void
gtk_printer_option_set_foreach (GtkPrinterOptionSet *set,
				GtkPrinterOptionSetFunc func,
				gpointer user_data)
{
}


#define __GTK_PRINTER_OPTION_SET_C__

