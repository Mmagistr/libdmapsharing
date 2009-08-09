/*
 * GGstMP3InputStream class: Open a URI using g_gst_mp3_input_stream_new ().
 * Data is decoded using GStreamer and is then reencoded as an MP3
 * stream by the class's read operations.
 *
 * Copyright (C) 2008 W. Michael Petullo <mike@flyn.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __G_GST_MP3_INPUT_STREAM
#define __G_GST_MP3_INPUT_STREAM

#include <gio/gio.h>

#include "g-gst-input-stream.h"

G_BEGIN_DECLS

#define TYPE_G_GST_MP3_INPUT_STREAM         (g_gst_mp3_input_stream_get_type ())
#define G_GST_MP3_INPUT_STREAM(o)           (G_TYPE_CHECK_INSTANCE_CAST ((o), \
				               TYPE_G_GST_MP3_INPUT_STREAM, \
					       GGstMP3InputStream))
#define G_GST_MP3_INPUT_STREAM_CLASS(k)     (G_TYPE_CHECK_CLASS_CAST((k), \
				               TYPE_G_GST_MP3_INPUT_STREAM, \
					       GGstMP3InputStreamClass))
#define IS_G_GST_MP3_INPUT_STREAM(o)        (G_TYPE_CHECK_INSTANCE_TYPE ((o), \
				               TYPE_G_GST_MP3_INPUT_STREAM))
#define IS_G_GST_MP3_INPUT_STREAM_CLASS(k)  (G_TYPE_CHECK_CLASS_TYPE ((k), \
				               TYPE_G_GST_MP3_INPUT_STREAM_CLASS))
#define G_GST_MP3_INPUT_STREAM_GET_CLASS(o) (G_TYPE_INSTANCE_GET_CLASS ((o), \
				               TYPE_G_GST_MP3_INPUT_STREAM, \
					       GGstMP3InputStreamClass))
#define G_GST_MP3_INPUT_STREAM_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
					   TYPE_G_GST_MP3_INPUT_STREAM, \
					   GGstMP3InputStreamPrivate))

typedef struct GGstMP3InputStreamPrivate GGstMP3InputStreamPrivate;

typedef struct {
	GGstInputStream parent;
	GGstMP3InputStreamPrivate *priv;
} GGstMP3InputStream;

typedef struct {
	GGstInputStreamClass parent;
} GGstMP3InputStreamClass;

GType         g_gst_mp3_input_stream_get_type (void);

GInputStream* g_gst_mp3_input_stream_new         (GInputStream *stream);

G_END_DECLS

#endif /* __G_GST_MP3_INPUT_STREAM */
