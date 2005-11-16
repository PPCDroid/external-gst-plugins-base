/* GStreamer
 * Copyright (C) <1999> Erik Walthinsen <omega@cse.ogi.edu>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#include <unistd.h>
#include <gst/gst.h>

gint
main (gint argc, gchar * argv[])
{
  GstElement *pipeline, *filesrc, *decodebin;
  GstStateChangeReturn res;
  GstIterator *it;
  gpointer data;

  gst_init (&argc, &argv);

  pipeline = gst_pipeline_new ("pipeline");

  filesrc = gst_element_factory_make ("filesrc", "filesrc");
  g_assert (filesrc);

  decodebin = gst_element_factory_make ("decodebin", "decodebin");
  g_assert (decodebin);

  gst_bin_add_many (GST_BIN (pipeline), filesrc, decodebin, NULL);
  gst_element_link (filesrc, decodebin);

  if (argc < 2) {
    g_print ("usage: %s <uri>\n", argv[0]);
    exit (-1);
  }
  g_object_set (G_OBJECT (filesrc), "location", argv[1], NULL);

  g_print ("pause..\n");
  res = gst_element_set_state (pipeline, GST_STATE_PAUSED);
  if (res == GST_STATE_CHANGE_FAILURE) {
    g_print ("could not pause\n");
    exit (-1);
  }
  g_print ("waiting..\n");
  res = gst_element_get_state (pipeline, NULL, NULL, GST_CLOCK_TIME_NONE);
  if (res != GST_STATE_CHANGE_SUCCESS) {
    g_print ("could not complete pause\n");
    exit (-1);
  }
  g_print ("stats..\n");

  it = gst_element_iterate_src_pads (decodebin);
  while (gst_iterator_next (it, &data) == GST_ITERATOR_OK) {
    GstPad *pad = GST_PAD (data);
    GstCaps *caps;
    gchar *str;
    GstQuery *query;

    g_print ("stream %s:\n", GST_OBJECT_NAME (pad));

    caps = gst_pad_get_caps (pad);
    str = gst_caps_to_string (caps);
    g_print (" caps: %s\n", str);
    g_free (str);

    query = gst_query_new_duration (GST_FORMAT_TIME);
    if (gst_pad_query (pad, query)) {
      gint64 duration;

      gst_query_parse_duration (query, NULL, &duration);

      g_print (" duration: %" GST_TIME_FORMAT "\n", GST_TIME_ARGS (duration));
    }
    gst_query_unref (query);

    gst_object_unref (pad);
  }
  gst_iterator_free (it);

  return 0;
}