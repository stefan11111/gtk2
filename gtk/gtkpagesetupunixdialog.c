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

#include "gtkprivate.h"

#include "gtkliststore.h"
#include "gtkstock.h"
#include "gtktreeviewcolumn.h"
#include "gtktreeselection.h"
#include "gtktreemodel.h"
#include "gtkbutton.h"
#include "gtkscrolledwindow.h"
#include "gtkvbox.h"
#include "gtkhbox.h"
#include "gtkframe.h"
#include "gtkeventbox.h"
#include "gtkcombobox.h"
#include "gtktogglebutton.h"
#include "gtkradiobutton.h"
#include "gtklabel.h"
#include "gtktable.h"
#include "gtkcelllayout.h"
#include "gtkcellrenderertext.h"
#include "gtkalignment.h"
#include "gtkspinbutton.h"
#include "gtkbbox.h"
#include "gtkhbbox.h"

#include "gtkpagesetupunixdialog.h"
#include "gtkcustompaperunixdialog.h"
#include "gtkprintbackend.h"
#include "gtkpapersize.h"
#include "gtkprintutils.h"


/**
 * gtk_page_setup_unix_dialog_new:
 * @title: (allow-none): the title of the dialog, or %NULL
 * @parent: (allow-none): transient parent of the dialog, or %NULL
 *
 * Creates a new page setup dialog.
 *
 * Returns: the new #GtkPageSetupUnixDialog
 *
 * Since: 2.10
 */
GtkWidget *
gtk_page_setup_unix_dialog_new (const gchar *title,
				GtkWindow   *parent)
{
  return NULL;
}

/**
 * gtk_page_setup_unix_dialog_set_page_setup:
 * @dialog: a #GtkPageSetupUnixDialog
 * @page_setup: a #GtkPageSetup
 * 
 * Sets the #GtkPageSetup from which the page setup
 * dialog takes its values.
 *
 * Since: 2.10
 **/
void
gtk_page_setup_unix_dialog_set_page_setup (GtkPageSetupUnixDialog *dialog,
					   GtkPageSetup           *page_setup)
{
}

/**
 * gtk_page_setup_unix_dialog_get_page_setup:
 * @dialog: a #GtkPageSetupUnixDialog
 * 
 * Gets the currently selected page setup from the dialog. 
 * 
 * Returns: (transfer none): the current page setup 
 *
 * Since: 2.10
 **/
GtkPageSetup *
gtk_page_setup_unix_dialog_get_page_setup (GtkPageSetupUnixDialog *dialog)
{
  return NULL;
}

/**
 * gtk_page_setup_unix_dialog_set_print_settings:
 * @dialog: a #GtkPageSetupUnixDialog
 * @print_settings: a #GtkPrintSettings
 * 
 * Sets the #GtkPrintSettings from which the page setup dialog
 * takes its values.
 * 
 * Since: 2.10
 **/
void
gtk_page_setup_unix_dialog_set_print_settings (GtkPageSetupUnixDialog *dialog,
					       GtkPrintSettings       *print_settings)
{
}

/**
 * gtk_page_setup_unix_dialog_get_print_settings:
 * @dialog: a #GtkPageSetupUnixDialog
 * 
 * Gets the current print settings from the dialog.
 * 
 * Returns: (transfer none): the current print settings
 *
 * Since: 2.10
 **/
GtkPrintSettings *
gtk_page_setup_unix_dialog_get_print_settings (GtkPageSetupUnixDialog *dialog)
{
  return NULL;
}

#define __GTK_PAGE_SETUP_UNIX_DIALOG_C__

