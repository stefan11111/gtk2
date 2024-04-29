/* GtkPrintUnixDialog
 * Copyright (C) 2006 John (J5) Palmieri  <johnp@redhat.com>
 * Copyright (C) 2006 Alexander Larsson <alexl@redhat.com>
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

#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <math.h>

#include "gtkintl.h"
#include "gtkprivate.h"

#include "gtkspinbutton.h"
#include "gtkcellrendererpixbuf.h"
#include "gtkcellrenderertext.h"
#include "gtkstock.h"
#include "gtkiconfactory.h"
#include "gtkimage.h"
#include "gtktreeselection.h"
#include "gtknotebook.h"
#include "gtkscrolledwindow.h"
#include "gtkcombobox.h"
#include "gtktogglebutton.h"
#include "gtkradiobutton.h"
#include "gtkdrawingarea.h"
#include "gtkvbox.h"
#include "gtktable.h"
#include "gtkframe.h"
#include "gtkalignment.h"
#include "gtklabel.h"
#include "gtkeventbox.h"
#include "gtkbuildable.h"

#include "gtkcustompaperunixdialog.h"
#include "gtkprintbackend.h"
#include "gtkprinter-private.h"
#include "gtkprintunixdialog.h"
#include "gtkprinteroptionwidget.h"
#include "gtkprintutils.h"


#include "gtkmessagedialog.h"
#include "gtkbutton.h"

/* This function controls "sensitive" property of GtkCellRenderer based on pause
 * state of printers. */
void set_cell_sensitivity_func (GtkTreeViewColumn *tree_column,
                                GtkCellRenderer   *cell,
                                GtkTreeModel      *tree_model,
                                GtkTreeIter       *iter,
                                gpointer           data)
{
}

/**
 * gtk_print_unix_dialog_new:
 * @title: (allow-none): Title of the dialog, or %NULL
 * @parent: (allow-none): Transient parent of the dialog, or %NULL
 *
 * Creates a new #GtkPrintUnixDialog.
 *
 * Return value: a new #GtkPrintUnixDialog
 *
 * Since: 2.10
 **/
GtkWidget *
gtk_print_unix_dialog_new (const gchar *title,
                           GtkWindow   *parent)
{
  return NULL;
}

/**
 * gtk_print_unix_dialog_get_selected_printer:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the currently selected printer.
 *
 * Returns: (transfer none): the currently selected printer
 *
 * Since: 2.10
 */
GtkPrinter *
gtk_print_unix_dialog_get_selected_printer (GtkPrintUnixDialog *dialog)
{
  return NULL;
}

/**
 * gtk_print_unix_dialog_set_page_setup:
 * @dialog: a #GtkPrintUnixDialog
 * @page_setup: a #GtkPageSetup
 *
 * Sets the page setup of the #GtkPrintUnixDialog.
 *
 * Since: 2.10
 */
void
gtk_print_unix_dialog_set_page_setup (GtkPrintUnixDialog *dialog,
                                      GtkPageSetup       *page_setup)
{
}

/**
 * gtk_print_unix_dialog_get_page_setup:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the page setup that is used by the #GtkPrintUnixDialog.
 *
 * Returns: (transfer none): the page setup of @dialog.
 *
 * Since: 2.10
 */
GtkPageSetup *
gtk_print_unix_dialog_get_page_setup (GtkPrintUnixDialog *dialog)
{
  return NULL;
}

/**
 * gtk_print_unix_dialog_get_page_setup_set:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the page setup that is used by the #GtkPrintUnixDialog.
 *
 * Returns: whether a page setup was set by user.
 *
 * Since: 2.18
 */
gboolean
gtk_print_unix_dialog_get_page_setup_set (GtkPrintUnixDialog *dialog)
{
  return 0;
}

/**
 * gtk_print_unix_dialog_set_current_page:
 * @dialog: a #GtkPrintUnixDialog
 * @current_page: the current page number.
 *
 * Sets the current page number. If @current_page is not -1, this enables
 * the current page choice for the range of pages to print.
 *
 * Since: 2.10
 */
void
gtk_print_unix_dialog_set_current_page (GtkPrintUnixDialog *dialog,
                                        gint                current_page)
{
}

/**
 * gtk_print_unix_dialog_get_current_page:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the current page of the #GtkPrintDialog.
 *
 * Returns: the current page of @dialog
 *
 * Since: 2.10
 */
gint
gtk_print_unix_dialog_get_current_page (GtkPrintUnixDialog *dialog)
{
  return 0;
}

/**
 * gtk_print_unix_dialog_set_settings:
 * @dialog: a #GtkPrintUnixDialog
 * @settings: (allow-none): a #GtkPrintSettings, or %NULL
 *
 * Sets the #GtkPrintSettings for the #GtkPrintUnixDialog. Typically,
 * this is used to restore saved print settings from a previous print
 * operation before the print dialog is shown.
 *
 * Since: 2.10
 **/
void
gtk_print_unix_dialog_set_settings (GtkPrintUnixDialog *dialog,
                                    GtkPrintSettings   *settings)
{
}

/**
 * gtk_print_unix_dialog_get_settings:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets a new #GtkPrintSettings object that represents the
 * current values in the print dialog. Note that this creates a
 * <emphasis>new object</emphasis>, and you need to unref it
 * if don't want to keep it.
 *
 * Returns: a new #GtkPrintSettings object with the values from @dialog
 *
 * Since: 2.10
 */
GtkPrintSettings *
gtk_print_unix_dialog_get_settings (GtkPrintUnixDialog *dialog)
{
  return NULL;
}

/**
 * gtk_print_unix_dialog_add_custom_tab:
 * @dialog: a #GtkPrintUnixDialog
 * @child: the widget to put in the custom tab
 * @tab_label: the widget to use as tab label
 *
 * Adds a custom tab to the print dialog.
 *
 * Since: 2.10
 */
void
gtk_print_unix_dialog_add_custom_tab (GtkPrintUnixDialog *dialog,
                                      GtkWidget          *child,
                                      GtkWidget          *tab_label)
{
}

/**
 * gtk_print_unix_dialog_set_manual_capabilities:
 * @dialog: a #GtkPrintUnixDialog
 * @capabilities: the printing capabilities of your application
 *
 * This lets you specify the printing capabilities your application
 * supports. For instance, if you can handle scaling the output then
 * you pass #GTK_PRINT_CAPABILITY_SCALE. If you don't pass that, then
 * the dialog will only let you select the scale if the printing
 * system automatically handles scaling.
 *
 * Since: 2.10
 */
void
gtk_print_unix_dialog_set_manual_capabilities (GtkPrintUnixDialog   *dialog,
                                               GtkPrintCapabilities  capabilities)
{
}

/**
 * gtk_print_unix_dialog_get_manual_capabilities:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the value of #GtkPrintUnixDialog::manual-capabilities property.
 *
 * Returns: the printing capabilities
 *
 * Since: 2.18
 */
GtkPrintCapabilities
gtk_print_unix_dialog_get_manual_capabilities (GtkPrintUnixDialog *dialog)
{
  return 0;
}

/**
 * gtk_print_unix_dialog_set_support_selection:
 * @dialog: a #GtkPrintUnixDialog
 * @support_selection: %TRUE to allow print selection
 *
 * Sets whether the print dialog allows user to print a selection.
 *
 * Since: 2.18
 */
void
gtk_print_unix_dialog_set_support_selection (GtkPrintUnixDialog *dialog,
                                             gboolean            support_selection)
{
}

/**
 * gtk_print_unix_dialog_get_support_selection:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the value of #GtkPrintUnixDialog::support-selection property.
 *
 * Returns: whether the application supports print of selection
 *
 * Since: 2.18
 */
gboolean
gtk_print_unix_dialog_get_support_selection (GtkPrintUnixDialog *dialog)
{
  return 0;
}

/**
 * gtk_print_unix_dialog_set_has_selection:
 * @dialog: a #GtkPrintUnixDialog
 * @has_selection: %TRUE indicates that a selection exists
 *
 * Sets whether a selection exists.
 *
 * Since: 2.18
 */
void
gtk_print_unix_dialog_set_has_selection (GtkPrintUnixDialog *dialog,
                                         gboolean            has_selection)
{
}

/**
 * gtk_print_unix_dialog_get_has_selection:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the value of #GtkPrintUnixDialog::has-selection property.
 *
 * Returns: whether there is a selection
 *
 * Since: 2.18
 */
gboolean
gtk_print_unix_dialog_get_has_selection (GtkPrintUnixDialog *dialog)
{
  return 0;
}

/**
 * gtk_print_unix_dialog_set_embed_page_setup
 * @dialog: a #GtkPrintUnixDialog
 * @embed: embed page setup selection
 *
 * Embed page size combo box and orientation combo box into page setup page.
 *
 * Since: 2.18
 */
void
gtk_print_unix_dialog_set_embed_page_setup (GtkPrintUnixDialog *dialog,
                                            gboolean            embed)
{
}

/**
 * gtk_print_unix_dialog_get_embed_page_setup:
 * @dialog: a #GtkPrintUnixDialog
 *
 * Gets the value of #GtkPrintUnixDialog::embed-page-setup property.
 *
 * Returns: whether there is a selection
 *
 * Since: 2.18
 */
gboolean
gtk_print_unix_dialog_get_embed_page_setup (GtkPrintUnixDialog *dialog)
{
  return 0;
}

#define __GTK_PRINT_UNIX_DIALOG_C__

