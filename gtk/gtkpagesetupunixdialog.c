/* GtkPageSetupUnixDialog 
 * Copyright (C) 2006 Alexander Larsson <alexl@redhat.com>
 * Copyright © 2006, 2007, 2008 Christian Persch
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "gtkpagesetupunixdialog.h"

GtkWidget *
gtk_page_setup_unix_dialog_new (const gchar *title,
				GtkWindow   *parent)
{
  return NULL;
}

void
gtk_page_setup_unix_dialog_set_page_setup (GtkPageSetupUnixDialog *dialog,
					   GtkPageSetup           *page_setup)
{
}

GtkPageSetup *
gtk_page_setup_unix_dialog_get_page_setup (GtkPageSetupUnixDialog *dialog)
{
  return NULL;
}

void
gtk_page_setup_unix_dialog_set_print_settings (GtkPageSetupUnixDialog *dialog,
					       GtkPrintSettings       *print_settings)
{
}

GtkPrintSettings *
gtk_page_setup_unix_dialog_get_print_settings (GtkPageSetupUnixDialog *dialog)
{
  return NULL;
}

#define __GTK_PAGE_SETUP_UNIX_DIALOG_C__

