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
#include "gtkalias.h"

#include "gtkmessagedialog.h"
#include "gtkbutton.h"

#define EXAMPLE_PAGE_AREA_SIZE 110
#define RULER_DISTANCE 7.5
#define RULER_RADIUS 2


#define GTK_PRINT_UNIX_DIALOG_GET_PRIVATE(o)  \
   (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_PRINT_UNIX_DIALOG, GtkPrintUnixDialogPrivate))


GType gtk_print_unix_dialog_get_type()
{
}

static const gchar * const common_paper_sizes[16] = {
  "na_letter",
  "na_legal",
  "iso_a4",
  "iso_a5",
  "roc_16k",
  "iso_b5",
  "jis_b5",
  "na_number-10",
  "iso_dl",
  "jpn_chou3",
  "na_ledger",
  "iso_a3",
};

enum {
  PAGE_SETUP_LIST_COL_PAGE_SETUP,
  PAGE_SETUP_LIST_COL_IS_SEPARATOR,
  PAGE_SETUP_LIST_N_COLS
};

enum {
  PROP_0,
  PROP_PAGE_SETUP,
  PROP_CURRENT_PAGE,
  PROP_PRINT_SETTINGS,
  PROP_SELECTED_PRINTER,
  PROP_MANUAL_CAPABILITIES,
  PROP_SUPPORT_SELECTION,
  PROP_HAS_SELECTION,
  PROP_EMBED_PAGE_SETUP
};

enum {
  PRINTER_LIST_COL_ICON,
  PRINTER_LIST_COL_NAME,
  PRINTER_LIST_COL_STATE,
  PRINTER_LIST_COL_JOBS,
  PRINTER_LIST_COL_LOCATION,
  PRINTER_LIST_COL_PRINTER_OBJ,
  PRINTER_LIST_N_COLS
};

struct GtkPrintUnixDialogPrivate
{
  GtkWidget *notebook;

  GtkWidget *printer_treeview;

  GtkPrintCapabilities manual_capabilities;
  GtkPrintCapabilities printer_capabilities;

  GtkTreeModel *printer_list;
  GtkTreeModelFilter *printer_list_filter;

  GtkPageSetup *page_setup;
  gboolean page_setup_set;
  gboolean embed_page_setup;
  GtkListStore *page_setup_list;
  GtkListStore *custom_paper_list;

  gboolean support_selection;
  gboolean has_selection;

  GtkWidget *all_pages_radio;
  GtkWidget *current_page_radio;
  GtkWidget *selection_radio;
  GtkWidget *range_table;
  GtkWidget *page_range_radio;
  GtkWidget *page_range_entry;

  GtkWidget *copies_spin;
  GtkWidget *collate_check;
  GtkWidget *reverse_check;
  GtkWidget *collate_image;
  GtkWidget *page_layout_preview;
  GtkWidget *scale_spin;
  GtkWidget *page_set_combo;
  GtkWidget *print_now_radio;
  GtkWidget *print_at_radio;
  GtkWidget *print_at_entry;
  GtkWidget *print_hold_radio;
  GtkWidget *preview_button;
  GtkWidget *paper_size_combo;
  GtkWidget *paper_size_combo_label;
  GtkWidget *orientation_combo;
  GtkWidget *orientation_combo_label;
  gboolean internal_page_setup_change;
  gboolean updating_print_at;
  GtkPrinterOptionWidget *pages_per_sheet;
  GtkPrinterOptionWidget *duplex;
  GtkPrinterOptionWidget *paper_type;
  GtkPrinterOptionWidget *paper_source;
  GtkPrinterOptionWidget *output_tray;
  GtkPrinterOptionWidget *job_prio;
  GtkPrinterOptionWidget *billing_info;
  GtkPrinterOptionWidget *cover_before;
  GtkPrinterOptionWidget *cover_after;
  GtkPrinterOptionWidget *number_up_layout;

  GtkWidget *conflicts_widget;

  GtkWidget *job_page;
  GtkWidget *finishing_table;
  GtkWidget *finishing_page;
  GtkWidget *image_quality_table;
  GtkWidget *image_quality_page;
  GtkWidget *color_table;
  GtkWidget *color_page;

  GtkWidget *advanced_vbox;
  GtkWidget *advanced_page;

  GtkWidget *extension_point;

  /* These are set initially on selected printer (either default printer,
   * printer taken from set settings, or user-selected), but when any
   * setting is changed by the user it is cleared.
   */
  GtkPrintSettings *initial_settings;

  GtkPrinterOption *number_up_layout_n_option;
  GtkPrinterOption *number_up_layout_2_option;

  /* This is the initial printer set by set_settings. We look for it in
   * the added printers. We clear this whenever the user manually changes
   * to another printer, when the user changes a setting or when we find
   * this printer.
   */
  char *waiting_for_printer;
  gboolean internal_printer_change;

  GList *print_backends;

  GtkPrinter *current_printer;
  GtkPrinter *request_details_printer;
  guint request_details_tag;
  GtkPrinterOptionSet *options;
  gulong options_changed_handler;
  gulong mark_conflicts_id;

  gchar *format_for_printer;

  gint current_page;
};

static GtkBuildableIface *parent_buildable_iface;

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
  GtkWidget *result;
  const gchar *_title = _("Print");

  if (title)
    _title = title;

  result = g_object_new (GTK_TYPE_PRINT_UNIX_DIALOG,
                         "transient-for", parent,
                         "title", _title,
                         "has-separator", FALSE,
                         NULL);

  return result;
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
  return 1;
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
#include "gtkaliasdef.c"
