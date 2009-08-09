/*
 * GGstWAVInputStream class: Open a URI using g_gst_wav_input_stream_new ().
 * Data is decoded using GStreamer and is then reencoded as a WAV
 * stream by the class's read operations.
 *
 * Copyright (C) 2009 W. Michael Petullo <mike@flyn.org>
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

#include <gst/gst.h>
#include <gst/app/gstappsink.h>

#include "gst-util.h"
#include "g-gst-wav-input-stream.h"

#define GST_APP_MAX_BUFFERS 1024

struct GGstWAVInputStreamPrivate {
	GstElement *pipeline;
	GstElement *src;
	GstElement *decode;
	GstElement *convert;
	GstCaps *filter;
	GstElement *encode;
	GstElement *sink;
};

static void
new_decoded_pad_cb (GstElement *element,
		    GstPad *pad,
		    gboolean arg,
		    GGstWAVInputStream *stream)
{
	/* Link remaining pad after decodebin does its magic. */
	GstPad *conv_pad;

	conv_pad = gst_element_get_static_pad (stream->priv->convert, "sink");
	g_assert (conv_pad != NULL);

	if (pads_compatible (pad, conv_pad)) {
		g_assert (! GST_PAD_IS_LINKED (
			gst_element_get_static_pad (stream->priv->convert, "sink")));

		gst_pad_link (pad, conv_pad);

		if (gst_element_link_filtered (stream->priv->convert,
					       stream->priv->encode,
					       stream->priv->filter) == FALSE) {
			g_warning ("Error linking convert and encode elements");
		}
		if (gst_element_link (stream->priv->encode,
				      stream->priv->sink) == FALSE) {
			g_warning ("Error linking encode and sink elements");
		}
	} else {
		g_warning ("Could not link GStreamer pipeline.");
	}
}

GInputStream* g_gst_wav_input_stream_new (GInputStream *src_stream)
{
	GstStateChangeReturn sret;
	GstState state;
	GGstWAVInputStream *stream;

	stream = G_GST_WAV_INPUT_STREAM (g_object_new (TYPE_G_GST_WAV_INPUT_STREAM,
						   NULL));

	stream->priv->pipeline = gst_pipeline_new ("pipeline");

	stream->priv->src     = gst_element_factory_make ("giostreamsrc", "src");
	stream->priv->decode  = gst_element_factory_make ("decodebin", "decode");
	stream->priv->convert = gst_element_factory_make ("audioconvert", "convert");
	/* Roku clients support a subset of the WAV format. */
	stream->priv->filter = gst_caps_new_simple ("audio/x-raw-int",
						    "channels", G_TYPE_INT, 2,
						    "width", G_TYPE_INT, 16,
						    "depth", G_TYPE_INT, 16,
						    NULL);
	stream->priv->encode  = gst_element_factory_make ("wavenc", "encode");
	stream->priv->sink    = gst_element_factory_make ("appsink", "sink");

	gst_bin_add_many (GST_BIN (stream->priv->pipeline),
			  stream->priv->src,
			  stream->priv->decode,
			  stream->priv->convert,
			  stream->priv->encode,
			  stream->priv->sink,
			  NULL);

	if (gst_element_link (stream->priv->src, stream->priv->decode) == FALSE) {
		g_warning ("Error linking source and decode elements");
	}

	g_object_set (G_OBJECT (stream->priv->src), "stream", src_stream, NULL);

	g_signal_connect (stream->priv->decode, "new-decoded-pad", G_CALLBACK (new_decoded_pad_cb), stream);

	g_object_set (G_OBJECT (stream->priv->sink), "emit-signals", TRUE, "sync", FALSE, NULL);
	gst_app_sink_set_max_buffers (GST_APP_SINK (stream->priv->sink),
				      GST_APP_MAX_BUFFERS);
	gst_app_sink_set_drop (GST_APP_SINK (stream->priv->sink), FALSE);

	g_signal_connect (stream->priv->sink, "new-buffer", G_CALLBACK (g_gst_input_stream_new_buffer_cb), stream);

	/* FIXME: this technique is shared with dmapd-daap-share.c */
	sret = gst_element_set_state (stream->priv->pipeline, GST_STATE_PLAYING);
	if (GST_STATE_CHANGE_ASYNC == sret) {
		if (GST_STATE_CHANGE_SUCCESS != gst_element_get_state (GST_ELEMENT (stream->priv->pipeline), &state, NULL, 5 * GST_SECOND)) {
			g_warning ("State change failed for stream.");
		}
	} else if (sret != GST_STATE_CHANGE_SUCCESS) {
		g_warning ("Could not read stream.");
	}

	g_assert (G_IS_SEEKABLE (stream));
	return G_INPUT_STREAM (stream);
}

static void
g_gst_wav_input_stream_kill_pipeline (GGstInputStream *stream)
{
        GGstWAVInputStream *wav_stream = G_GST_WAV_INPUT_STREAM (stream);

	gst_element_set_state (wav_stream->priv->pipeline, GST_STATE_NULL);
	gst_object_unref (GST_OBJECT (wav_stream->priv->pipeline));
}

G_DEFINE_TYPE (GGstWAVInputStream, g_gst_wav_input_stream, TYPE_G_GST_INPUT_STREAM)

static void
g_gst_wav_input_stream_class_init (GGstWAVInputStreamClass *klass)
{
	GGstInputStreamClass *parent_class = G_GST_INPUT_STREAM_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GGstWAVInputStreamPrivate));

	parent_class->kill_pipeline = g_gst_wav_input_stream_kill_pipeline;
}

static void
g_gst_wav_input_stream_init (GGstWAVInputStream *stream)
{
	stream->priv = G_GST_WAV_INPUT_STREAM_GET_PRIVATE (stream);

}
