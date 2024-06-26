/* GTK - The GIMP Toolkit
 * Copyright (C) 1997 David Mosberger
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

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#undef GTK_DISABLE_DEPRECATED

#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <gdk-pixbuf/gdk-pixdata.h>

#include "gtkgamma.h"
#include "gtkcurve.h"
#include "gtkdialog.h"
#include "gtkdrawingarea.h"
#include "gtkentry.h"
#include "gtkhbox.h"
#include "gtkimage.h"
#include "gtklabel.h"
#include "gtkmain.h"
#include "gtkradiobutton.h"
#include "gtkstock.h"
#include "gtktable.h"
#include "gtkvbox.h"
#include "gtkwindow.h"
#include "gtkintl.h"


/* forward declarations: */
static void gtk_gamma_curve_destroy (GtkObject *object);

static void curve_type_changed_callback (GtkWidget *w, gpointer data);
static void button_realize_callback     (GtkWidget *w);
static void button_toggled_callback     (GtkWidget *w, gpointer data);
static void button_clicked_callback     (GtkWidget *w, gpointer data);

enum
  {
    LINEAR = 0,
    SPLINE,
    FREE,
    GAMMA,
    RESET,
    NUM_XPMS
  };

/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (spline_pixdata)
#endif
#ifdef __GNUC__
static const guint8 spline_pixdata[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 spline_pixdata[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (182) */
  "\0\0\0\316"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\216\0\0\0\0\202\0\0\0\377\211\0\0\0\0\206\377\0\0\377\1\0\0\0\377\207"
  "\0\0\0\0\202\377\0\0\377\214\0\0\0\0\2\0\0\0\377\274--\377\215\0\0\0"
  "\0\203\0\0\0\377\215\0\0\0\0\2\274--\377\0\0\0\377\216\0\0\0\0\1\377"
  "\0\0\377\216\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\216\0\0"
  "\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377"
  "\217\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\216\0\0\0\0\2\0"
  "\0\0\377\274--\377\216\0\0\0\0\202\0\0\0\377\216\0\0\0\0"};


/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (linear_pixdata)
#endif
#ifdef __GNUC__
static const guint8 linear_pixdata[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 linear_pixdata[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (323) */
  "\0\0\1["
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\216\0\0\0\0\202\0\0\0\377\216\0\0\0\0\2\202AA\377\0\0\0\377\216\0\0"
  "\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377"
  "\206\0\0\0\0\3\177\177\177\377\0\0\0\377\177\177\177\377\205\0\0\0\0"
  "\1\377\0\0\377\207\0\0\0\0\203\0\0\0\377\205\0\0\0\0\1\377\0\0\377\207"
  "\0\0\0\0\3\202AA\377\0\0\0\377\202AA\377\204\0\0\0\0\1\377\0\0\377\210"
  "\0\0\0\0\3\377\0\0\377\0\0\0\0\377\0\0\377\204\0\0\0\0\1\377\0\0\377"
  "\207\0\0\0\0\1\377\0\0\377\203\0\0\0\0\1\377\0\0\377\202\0\0\0\0\1\377"
  "\0\0\377\210\0\0\0\0\1\377\0\0\377\203\0\0\0\0\1\377\0\0\377\202\0\0"
  "\0\0\1\377\0\0\377\207\0\0\0\0\1\377\0\0\377\205\0\0\0\0\3\377\0\0\377"
  "\0\0\0\377\202AA\377\207\0\0\0\0\1\377\0\0\377\205\0\0\0\0\203\0\0\0"
  "\377\206\0\0\0\0\1\377\0\0\377\206\0\0\0\0\3\177\177\177\377\0\0\0\377"
  "\177\177\177\377\205\0\0\0\0\2\0\0\0\377\202AA\377\216\0\0\0\0\202\0"
  "\0\0\377\216\0\0\0\0"};


/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (free_pixdata)
#endif
#ifdef __GNUC__
static const guint8 free_pixdata[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 free_pixdata[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (204) */
  "\0\0\0\344"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\246\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\220\0\0\0\0\1\377"
  "\0\0\377\217\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\220\0\0"
  "\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377\217\0\0\0\0\1\377\0\0\377"
  "\214\0\0\0\0\1\377\0\0\377\203\0\0\0\0\2\377\0\0\377\0\0\0\0\205\377"
  "\0\0\377\204\0\0\0\0\1\377\0\0\377\204\0\0\0\0\1\377\0\0\377\211\0\0"
  "\0\0\1\377\0\0\377\205\0\0\0\0\1\377\0\0\377\210\0\0\0\0\1\377\0\0\377"
  "\207\0\0\0\0\1\377\0\0\377\206\0\0\0\0\1\377\0\0\377\210\0\0\0\0\1\377"
  "\0\0\377\205\0\0\0\0\1\377\0\0\377\217\0\0\0\0"};


/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (gamma_pixdata)
#endif
#ifdef __GNUC__
static const guint8 gamma_pixdata[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 gamma_pixdata[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (218) */
  "\0\0\0\362"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\264\0\0\0\0\2\0\0\0\377^^^\377\202\0\0\0\0\3\214\214\214\377\0\0\0\377"
  "\214\214\214\377\211\0\0\0\0\7FFF\377\27\27\27\377\273\273\273\377\0"
  "\0\0\0uuu\377\27\27\27\377\244\244\244\377\212\0\0\0\0\3uuu\377\214\214"
  "\214\377\0\0\0\0\202FFF\377\214\0\0\0\0\4\0\0\0\377\0\0\0\0\0\0\0\377"
  "\214\214\214\377\214\0\0\0\0\3FFF\377\0\0\0\0FFF\377\215\0\0\0\0\3FF"
  "F\377\27\27\27\377\214\214\214\377\215\0\0\0\0\2\244\244\244\377\0\0"
  "\0\377\216\0\0\0\0\2uuu\377^^^\377\216\0\0\0\0\2///\377\0\0\0\377\216"
  "\0\0\0\0\202\0\0\0\377\216\0\0\0\0\2\0\0\0\377///\377\250\0\0\0\0"};


/* GdkPixbuf RGBA C-Source image dump 1-byte-run-length-encoded */

#ifdef __SUNPRO_C
#pragma align 4 (reset_pixdata)
#endif
#ifdef __GNUC__
static const guint8 reset_pixdata[] __attribute__ ((__aligned__ (4))) = 
#else
static const guint8 reset_pixdata[] = 
#endif
{ ""
  /* Pixbuf magic (0x47646b50) */
  "GdkP"
  /* length: header (24) + pixel_data (173) */
  "\0\0\0\305"
  /* pixdata_type (0x2010002) */
  "\2\1\0\2"
  /* rowstride (64) */
  "\0\0\0@"
  /* width (16) */
  "\0\0\0\20"
  /* height (16) */
  "\0\0\0\20"
  /* pixel_data: */
  "\216\0\0\0\0\202\0\0\0\377\216\0\0\0\0\2\202AA\377\0\0\0\377\215\0\0"
  "\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377"
  "\216\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377"
  "\0\0\377\216\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377\216\0\0"
  "\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377\216\0\0\0\0\1\377\0\0\377"
  "\216\0\0\0\0\1\377\0\0\377\215\0\0\0\0\2\0\0\0\377\202AA\377\216\0\0"
  "\0\0\202\0\0\0\377\216\0\0\0\0"};

G_DEFINE_TYPE (GtkGammaCurve, gtk_gamma_curve, GTK_TYPE_VBOX)

static void
gtk_gamma_curve_class_init (GtkGammaCurveClass *class)
{
  GtkObjectClass *object_class;

  object_class = (GtkObjectClass *) class;
  object_class->destroy = gtk_gamma_curve_destroy;
}

static void
gtk_gamma_curve_init (GtkGammaCurve *curve)
{
  GtkWidget *vbox;
  int i;

  curve->gamma = 1.0;

  curve->table = gtk_table_new (1, 2, FALSE);
  gtk_table_set_col_spacings (GTK_TABLE (curve->table), 3);
  gtk_container_add (GTK_CONTAINER (curve), curve->table);

  curve->curve = gtk_curve_new ();
  g_signal_connect (curve->curve, "curve-type-changed",
		    G_CALLBACK (curve_type_changed_callback), curve);
  gtk_table_attach_defaults (GTK_TABLE (curve->table), curve->curve, 0, 1, 0, 1);

  vbox = gtk_vbox_new (/* homogeneous */ FALSE, /* spacing */ 3);
  gtk_table_attach (GTK_TABLE (curve->table), vbox, 1, 2, 0, 1, 0, 0, 0, 0);

  /* toggle buttons: */
  for (i = 0; i < 3; ++i)
    {
      curve->button[i] = gtk_toggle_button_new ();
      g_object_set_data (G_OBJECT (curve->button[i]), I_("_GtkGammaCurveIndex"),
			 GINT_TO_POINTER (i));
      gtk_container_add (GTK_CONTAINER (vbox), curve->button[i]);
      g_signal_connect (curve->button[i], "realize",
			G_CALLBACK (button_realize_callback), NULL);
      g_signal_connect (curve->button[i], "toggled",
			G_CALLBACK (button_toggled_callback), curve);
      gtk_widget_show (curve->button[i]);
    }

  /* push buttons: */
  for (i = 3; i < 5; ++i)
    {
      curve->button[i] = gtk_button_new ();
      g_object_set_data (G_OBJECT (curve->button[i]), I_("_GtkGammaCurveIndex"),
			 GINT_TO_POINTER (i));
      gtk_container_add (GTK_CONTAINER (vbox), curve->button[i]);
      g_signal_connect (curve->button[i], "realize",
			G_CALLBACK (button_realize_callback), NULL);
      g_signal_connect (curve->button[i], "clicked",
			G_CALLBACK (button_clicked_callback), curve);
      gtk_widget_show (curve->button[i]);
    }

  gtk_widget_show (vbox);
  gtk_widget_show (curve->table);
  gtk_widget_show (curve->curve);
}

#define	return_header_corrupt(error)	{ \
  g_set_error_literal (error, GDK_PIXBUF_ERROR, \
                       GDK_PIXBUF_ERROR_CORRUPT_IMAGE, "Image header corrupt"); \
  return FALSE; \
}
#define	return_invalid_format(error)	{ \
  g_set_error_literal (error, GDK_PIXBUF_ERROR, \
                       GDK_PIXBUF_ERROR_UNKNOWN_TYPE, "Image format unknown"); \
  return FALSE; \
}
#define	return_pixel_corrupt(error)	{ \
  g_set_error_literal (error, GDK_PIXBUF_ERROR, \
                       GDK_PIXBUF_ERROR_CORRUPT_IMAGE, "Image pixel data corrupt"); \
  return FALSE; \
}

static inline const guint8 *
get_uint32 (const guint8 *stream, guint *result)
{
  *result = (stream[0] << 24) + (stream[1] << 16) + (stream[2] << 8) + stream[3];
  return stream + 4;
}

static gboolean
_gdk_pixdata_deserialize (GdkPixdata   *pixdata,
			 guint         stream_length,
			 const guint8 *stream,
			 GError	     **error)
{
  guint color_type, sample_width, encoding;
  g_return_val_if_fail (pixdata != NULL, FALSE);
  if (stream_length < GDK_PIXDATA_HEADER_LENGTH)
    return_header_corrupt (error);
  g_return_val_if_fail (stream != NULL, FALSE);
  /* deserialize header */
  stream = get_uint32 (stream, &pixdata->magic);
  stream = get_uint32 (stream, (guint32 *)&pixdata->length);
  if (pixdata->magic != GDK_PIXBUF_MAGIC_NUMBER || pixdata->length < GDK_PIXDATA_HEADER_LENGTH)
    return_header_corrupt (error);
  stream = get_uint32 (stream, &pixdata->pixdata_type);
  stream = get_uint32 (stream, &pixdata->rowstride);
  stream = get_uint32 (stream, &pixdata->width);
  stream = get_uint32 (stream, &pixdata->height);
  if (pixdata->width < 1 || pixdata->height < 1 ||
      pixdata->rowstride < pixdata->width)
    return_header_corrupt (error);
  color_type = pixdata->pixdata_type & GDK_PIXDATA_COLOR_TYPE_MASK;
  sample_width = pixdata->pixdata_type & GDK_PIXDATA_SAMPLE_WIDTH_MASK;
  encoding = pixdata->pixdata_type & GDK_PIXDATA_ENCODING_MASK;
  if ((color_type != GDK_PIXDATA_COLOR_TYPE_RGB &&
       color_type != GDK_PIXDATA_COLOR_TYPE_RGBA) ||
      sample_width != GDK_PIXDATA_SAMPLE_WIDTH_8 ||
      (encoding != GDK_PIXDATA_ENCODING_RAW &&
       encoding != GDK_PIXDATA_ENCODING_RLE))
    return_invalid_format (error);
  /* deserialize pixel data */
  if (stream_length < pixdata->length - GDK_PIXDATA_HEADER_LENGTH)
    return_pixel_corrupt (error);
  pixdata->pixel_data = (guint8 *)stream;
  return TRUE;
}

/* From glib's gmem.c */
#define SIZE_OVERFLOWS(a,b) (G_UNLIKELY ((b) > 0 && (a) > G_MAXSIZE / (b)))
#define RLE_OVERRUN(offset) (rle_buffer_limit == NULL ? FALSE : rle_buffer + (offset) > rle_buffer_limit)

static GdkPixbuf*
_gdk_pixbuf_from_pixdata (const GdkPixdata *pixdata,
			 gboolean          copy_pixels,
			 GError          **error)
{
  guint encoding, bpp;
  guint8 *data = NULL;

  g_return_val_if_fail (pixdata != NULL, NULL);
  g_return_val_if_fail (pixdata->width > 0, NULL);
  g_return_val_if_fail (pixdata->height > 0, NULL);
  g_return_val_if_fail (pixdata->rowstride >= pixdata->width, NULL);
  g_return_val_if_fail ((pixdata->pixdata_type & GDK_PIXDATA_COLOR_TYPE_MASK) == GDK_PIXDATA_COLOR_TYPE_RGB ||
			(pixdata->pixdata_type & GDK_PIXDATA_COLOR_TYPE_MASK) == GDK_PIXDATA_COLOR_TYPE_RGBA, NULL);
  g_return_val_if_fail ((pixdata->pixdata_type & GDK_PIXDATA_SAMPLE_WIDTH_MASK) == GDK_PIXDATA_SAMPLE_WIDTH_8, NULL);
  g_return_val_if_fail ((pixdata->pixdata_type & GDK_PIXDATA_ENCODING_MASK) == GDK_PIXDATA_ENCODING_RAW ||
			(pixdata->pixdata_type & GDK_PIXDATA_ENCODING_MASK) == GDK_PIXDATA_ENCODING_RLE, NULL);
  g_return_val_if_fail (pixdata->pixel_data != NULL, NULL);

  bpp = (pixdata->pixdata_type & GDK_PIXDATA_COLOR_TYPE_MASK) == GDK_PIXDATA_COLOR_TYPE_RGB ? 3 : 4;
  encoding = pixdata->pixdata_type & GDK_PIXDATA_ENCODING_MASK;

  g_debug ("gdk_pixbuf_from_pixdata() called on:");
  g_debug ("\tEncoding %s", encoding == GDK_PIXDATA_ENCODING_RAW ? "raw" : "rle");
  g_debug ("\tDimensions: %d x %d", pixdata->width, pixdata->height);
  g_debug ("\tRowstride: %d, Length: %d", pixdata->rowstride, pixdata->length);
  g_debug ("\tCopy pixels == %s", copy_pixels ? "true" : "false");

  if (encoding == GDK_PIXDATA_ENCODING_RLE)
    copy_pixels = TRUE;

  /* Sanity check the length and dimensions */
  if (SIZE_OVERFLOWS (pixdata->height, pixdata->rowstride))
    {
      g_set_error_literal (error, GDK_PIXBUF_ERROR,
                           GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                           _("Image pixel data corrupt"));
      return NULL;
    }

  if (encoding == GDK_PIXDATA_ENCODING_RAW &&
      pixdata->length >= 1 &&
      pixdata->length < pixdata->height * pixdata->rowstride - GDK_PIXDATA_HEADER_LENGTH)
    {
      g_set_error_literal (error, GDK_PIXBUF_ERROR,
                           GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                           _("Image pixel data corrupt"));
      return NULL;
    }

  if (copy_pixels)
    {
      data = g_try_malloc_n (pixdata->height, pixdata->rowstride);
      if (!data)
	{
	  g_set_error (error, GDK_PIXBUF_ERROR,
		       GDK_PIXBUF_ERROR_INSUFFICIENT_MEMORY,
		       pixdata->rowstride * pixdata->height == 1 ? "failed to allocate image buffer of %u byte" :
		                                                   "failed to allocate image buffer of %u bytes",
		       pixdata->rowstride * pixdata->height);
	  return NULL;
	}
    }
  if (encoding == GDK_PIXDATA_ENCODING_RLE)
    {
      const guint8 *rle_buffer = pixdata->pixel_data;
      guint8 *rle_buffer_limit = NULL;
      guint8 *image_buffer = data;
      guint8 *image_limit = data + pixdata->rowstride * pixdata->height;
      gboolean check_overrun = FALSE;

      if (pixdata->length >= 1)
        rle_buffer_limit = pixdata->pixel_data + pixdata->length - GDK_PIXDATA_HEADER_LENGTH;

      while (image_buffer < image_limit &&
             (rle_buffer_limit != NULL || rle_buffer > rle_buffer_limit))
	{
	  guint length;

	  if (RLE_OVERRUN(1))
	    {
	      check_overrun = TRUE;
	      break;
	    }

	  length = *(rle_buffer++);

	  if (length & 128)
	    {
	      length = length - 128;
	      check_overrun = image_buffer + length * bpp > image_limit;
	      if (check_overrun)
		length = (image_limit - image_buffer) / bpp;
	      if (RLE_OVERRUN(bpp < 4 ? 3 : 4))
	        {
	          check_overrun = TRUE;
	          break;
	        }
	      if (bpp < 4)	/* RGB */
		do
		  {
		    memcpy (image_buffer, rle_buffer, 3);
		    image_buffer += 3;
		  }
		while (--length);
	      else		/* RGBA */
		do
		  {
		    memcpy (image_buffer, rle_buffer, 4);
		    image_buffer += 4;
		  }
		while (--length);
	      if (RLE_OVERRUN(bpp))
	        {
	          check_overrun = TRUE;
	          break;
		}
	      rle_buffer += bpp;
	    }
	  else
	    {
	      length *= bpp;
	      check_overrun = image_buffer + length > image_limit;
	      if (check_overrun)
		length = image_limit - image_buffer;
	      if (RLE_OVERRUN(length))
	        {
	          check_overrun = TRUE;
	          break;
		}
	      memcpy (image_buffer, rle_buffer, length);
	      image_buffer += length;
	      rle_buffer += length;
	    }
	}
      if (check_overrun)
	{
	  g_free (data);
	  g_set_error_literal (error, GDK_PIXBUF_ERROR,
                               GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
                               _("Image pixel data corrupt"));
	  return NULL;
	}
    }
  else if (copy_pixels)
    memcpy (data, pixdata->pixel_data, pixdata->rowstride * pixdata->height);
  else
    data = pixdata->pixel_data;

  return gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB,
				   (pixdata->pixdata_type & GDK_PIXDATA_COLOR_TYPE_MASK) == GDK_PIXDATA_COLOR_TYPE_RGBA,
				   8, pixdata->width, pixdata->height, pixdata->rowstride,
				   copy_pixels ? (GdkPixbufDestroyNotify) g_free : NULL, data);
}

static void
button_realize_callback (GtkWidget *w)
{
  GtkWidget *image;
  struct {
    const guint8 *stream;
    gint length;
  } streams[5] = {
    { linear_pixdata, sizeof (linear_pixdata) },
    { spline_pixdata, sizeof (spline_pixdata) },
    { free_pixdata, sizeof (free_pixdata) },
    { gamma_pixdata, sizeof (gamma_pixdata) },
    { reset_pixdata, sizeof (reset_pixdata) }
  };
  GdkPixdata pixdata;
  GdkPixbuf *pixbuf;
  int i;

  i = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "_GtkGammaCurveIndex"));
  _gdk_pixdata_deserialize (&pixdata, streams[i].length, streams[i].stream, NULL);
  pixbuf = _gdk_pixbuf_from_pixdata (&pixdata, TRUE, NULL);
  image = gtk_image_new_from_pixbuf (pixbuf);
  gtk_container_add (GTK_CONTAINER (w), image);
  gtk_widget_show (image);

  g_object_unref (pixbuf);
}

static void
button_toggled_callback (GtkWidget *w, gpointer data)
{
  GtkGammaCurve *c = data;
  GtkCurveType type;
  int active, i;

  if (!GTK_TOGGLE_BUTTON (w)->active)
    return;

  active = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "_GtkGammaCurveIndex"));

  for (i = 0; i < 3; ++i)
    if ((i != active) && GTK_TOGGLE_BUTTON (c->button[i])->active)
      break;

  if (i < 3)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c->button[i]), FALSE);

  switch (active)
    {
    case 0:  type = GTK_CURVE_TYPE_SPLINE; break;
    case 1:  type = GTK_CURVE_TYPE_LINEAR; break;
    default: type = GTK_CURVE_TYPE_FREE; break;
    }
  gtk_curve_set_curve_type (GTK_CURVE (c->curve), type);
}

static void
gamma_cancel_callback (GtkWidget *w, gpointer data)
{
  GtkGammaCurve *c = data;

  gtk_widget_destroy (c->gamma_dialog);
}

static void
gamma_ok_callback (GtkWidget *w, gpointer data)
{
  GtkGammaCurve *c = data;
  const gchar *start;
  gchar *end;
  gfloat v;

  start = gtk_entry_get_text (GTK_ENTRY (c->gamma_text));
  if (start)
    {
      v = g_strtod (start, &end);
      if (end > start && v > 0.0)
	c->gamma = v;
    }
  gtk_curve_set_gamma (GTK_CURVE (c->curve), c->gamma);
  gamma_cancel_callback (w, data);
}

static void
button_clicked_callback (GtkWidget *w, gpointer data)
{
  GtkGammaCurve *c = data;
  int active;

  active = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (w), "_GtkGammaCurveIndex"));
  if (active == 3)
    {
      /* set gamma */
      if (c->gamma_dialog)
	return;
      else
	{
	  GtkWidget *vbox, *hbox, *label, *button;
	  gchar buf[64];
	  
	  c->gamma_dialog = gtk_dialog_new ();
	  gtk_window_set_screen (GTK_WINDOW (c->gamma_dialog),
				 gtk_widget_get_screen (w));
	  gtk_window_set_title (GTK_WINDOW (c->gamma_dialog), _("Gamma"));
	  g_object_add_weak_pointer (G_OBJECT (c->gamma_dialog),
				     (gpointer *)&c->gamma_dialog);
	  
	  vbox = GTK_DIALOG (c->gamma_dialog)->vbox;
	  
	  hbox = gtk_hbox_new (/* homogeneous */ FALSE, 0);
	  gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 2);
	  gtk_widget_show (hbox);
	  
	  label = gtk_label_new_with_mnemonic (_("_Gamma value"));
	  gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 2);
	  gtk_widget_show (label);
	  
	  sprintf (buf, "%g", c->gamma);
	  c->gamma_text = gtk_entry_new ();
          gtk_label_set_mnemonic_widget (GTK_LABEL (label), c->gamma_text);
	  gtk_entry_set_text (GTK_ENTRY (c->gamma_text), buf);
	  gtk_box_pack_start (GTK_BOX (hbox), c->gamma_text, TRUE, TRUE, 2);
	  gtk_widget_show (c->gamma_text);
	  
	  /* fill in action area: */
	  hbox = GTK_DIALOG (c->gamma_dialog)->action_area;

          button = gtk_button_new_from_stock (GTK_STOCK_CANCEL);
	  g_signal_connect (button, "clicked",
			    G_CALLBACK (gamma_cancel_callback), c);
	  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
	  gtk_widget_show (button);
	  
          button = gtk_button_new_from_stock (GTK_STOCK_OK);
	  gtk_widget_set_can_default (button, TRUE);
	  g_signal_connect (button, "clicked",
			    G_CALLBACK (gamma_ok_callback), c);
	  gtk_box_pack_start (GTK_BOX (hbox), button, TRUE, TRUE, 0);
	  gtk_widget_grab_default (button);
	  gtk_widget_show (button);
	  
	  gtk_widget_show (c->gamma_dialog);
	}
    }
  else
    {
      /* reset */
      gtk_curve_reset (GTK_CURVE (c->curve));
    }
}

static void
curve_type_changed_callback (GtkWidget *w, gpointer data)
{
  GtkGammaCurve *c = data;
  GtkCurveType new_type;
  int active;

  new_type = GTK_CURVE (w)->curve_type;
  switch (new_type)
    {
    case GTK_CURVE_TYPE_SPLINE: active = 0; break;
    case GTK_CURVE_TYPE_LINEAR: active = 1; break;
    default:		        active = 2; break;
    }
  if (!GTK_TOGGLE_BUTTON (c->button[active])->active)
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (c->button[active]), TRUE);
}

GtkWidget*
gtk_gamma_curve_new (void)
{
  return g_object_new (GTK_TYPE_GAMMA_CURVE, NULL);
}

static void
gtk_gamma_curve_destroy (GtkObject *object)
{
  GtkGammaCurve *c = GTK_GAMMA_CURVE (object);

  if (c->gamma_dialog)
    gtk_widget_destroy (c->gamma_dialog);

  GTK_OBJECT_CLASS (gtk_gamma_curve_parent_class)->destroy (object);
}

#define __GTK_GAMMA_CURVE_C__

