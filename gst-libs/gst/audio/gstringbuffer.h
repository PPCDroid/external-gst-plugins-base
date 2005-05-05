/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2005 Wim Taymans <wim@fluendo.com>
 *
 * gstringbuffer.h: 
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

#ifndef __GST_RINGBUFFER_H__
#define __GST_RINGBUFFER_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_RINGBUFFER  	         (gst_ringbuffer_get_type())
#define GST_RINGBUFFER(obj) 		 (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_RINGBUFFER,GstRingBuffer))
#define GST_RINGBUFFER_CLASS(klass) 	 (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_RINGBUFFER,GstRingBufferClass))
#define GST_RINGBUFFER_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), GST_TYPE_RINGBUFFER, GstRingBufferClass))
#define GST_IS_RINGBUFFER(obj)  	 (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_RINGBUFFER))
#define GST_IS_RINGBUFFER_CLASS(obj)     (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_RINGBUFFER))

typedef struct _GstRingBuffer GstRingBuffer;
typedef struct _GstRingBufferClass GstRingBufferClass;
typedef struct _GstRingBufferSpec GstRingBufferSpec;

/* called to fill data with len bytes of samples */
typedef void (*GstRingBufferCallback) (GstRingBuffer *rbuf, guint8* data, guint len, gpointer user_data);

typedef enum {
  GST_RINGBUFFER_STATE_STOPPED,
  GST_RINGBUFFER_STATE_PAUSED,
  GST_RINGBUFFER_STATE_PLAYING,
} GstRingBufferState;

typedef enum {
  GST_SEGSTATE_INVALID,
  GST_SEGSTATE_EMPTY,
  GST_SEGSTATE_FILLED,
  GST_SEGSTATE_PARTIAL,
} GstRingBufferSegState;

typedef enum
{
  GST_BUFTYPE_LINEAR,
  GST_BUFTYPE_FLOAT,
  GST_BUFTYPE_MU_LAW,
  GST_BUFTYPE_A_LAW,
  GST_BUFTYPE_IMA_ADPCM,
  GST_BUFTYPE_MPEG,
  GST_BUFTYPE_GSM,
} GstBufferFormatType;

typedef enum
{
  GST_UNKNOWN,

  GST_S8,
  GST_U8,

  GST_S16_LE,
  GST_S16_BE,
  GST_U16_LE,
  GST_U16_BE,

  GST_S24_LE,
  GST_S24_BE,
  GST_U24_LE,
  GST_U24_BE,

  GST_S32_LE,
  GST_S32_BE,
  GST_U32_LE,
  GST_U32_BE,

  GST_S24_3LE,
  GST_S24_3BE,
  GST_U24_3LE,
  GST_U24_3BE,
  GST_S20_3LE,
  GST_S20_3BE,
  GST_U20_3LE,
  GST_U20_3BE,
  GST_S18_3LE,
  GST_S18_3BE,
  GST_U18_3LE,
  GST_U18_3BE,

  GST_FLOAT32_LE,
  GST_FLOAT32_BE,

  GST_FLOAT64_LE,
  GST_FLOAT64_BE,

  GST_MU_LAW,
  GST_A_LAW,
  GST_IMA_ADPCM,
  GST_MPEG,
  GST_GSM,

  /* fill me */

} GstBufferFormat;

struct _GstRingBufferSpec
{
  /* in */
  GstCaps  *caps;		/* the caps of the buffer */

  /* in/out */
  GstBufferFormatType   type;
  GstBufferFormat format;
  gboolean  sign;
  gboolean  bigend;
  gint	    width;
  gint	    depth;
  gint      rate;
  gint      channels;
  
  GstClockTime latency_time;	/* the required/actual latency time */
  GstClockTime buffer_time;	/* the required/actual time of the buffer */
  gint     segsize;		/* size of one buffer segement */
  gint     segtotal;		/* total number of segments */

  /* out */
  gint     bytes_per_sample;	/* number of bytes of one sample */
  guint8   silence_sample[32];  /* bytes representing silence */
};

#define GST_RINGBUFFER_GET_COND(buf) (((GstRingBuffer *)buf)->cond)
#define GST_RINGBUFFER_WAIT(buf)     (g_cond_wait (GST_RINGBUFFER_GET_COND (buf), GST_GET_LOCK (buf)))
#define GST_RINGBUFFER_SIGNAL(buf)   (g_cond_signal (GST_RINGBUFFER_GET_COND (buf)))
#define GST_RINGBUFFER_BROADCAST(buf)(g_cond_broadcast (GST_RINGBUFFER_GET_COND (buf)))

struct _GstRingBuffer {
  GstObject 	         object;

  /*< public >*/ /* with LOCK */
  GCond                 *cond;
  gboolean               acquired;
  GstBuffer             *data;
  GstRingBufferSpec      spec;
  GstRingBufferSegState *segstate;
  gint     		 samples_per_seg;     /* number of samples per segment */
  guint8		*empty_seg;

  /*< public >*/ /* ATOMIC */
  gint			 state;		/* state of the buffer */
  gint			 segplayed;     /* number of segments played since last start */
  gint			 waiting;	/* when waiting for a segment to be freed */

  /*< private >*/
  guint64		 next_sample;	/* the next sample we need to write */
  GstRingBufferCallback  callback;
  gpointer               cb_data;
};

struct _GstRingBufferClass {
  GstObjectClass parent_class;

  /*< public >*/
  /* allocate the resources for the ringbuffer using the given specs */
  gboolean     (*acquire)      (GstRingBuffer *buf, GstRingBufferSpec *spec);
  /* free resources of the ringbuffer */
  gboolean     (*release)      (GstRingBuffer *buf);

  /* playback control */
  gboolean     (*play)         (GstRingBuffer *buf);
  gboolean     (*pause)        (GstRingBuffer *buf);
  gboolean     (*resume)       (GstRingBuffer *buf);
  gboolean     (*stop)         (GstRingBuffer *buf);

  /* number of samples queued in device */
  guint        (*delay)        (GstRingBuffer *buf);
};

GType gst_ringbuffer_get_type(void);

/* callback stuff */
void     	gst_ringbuffer_set_callback   	(GstRingBuffer *buf, GstRingBufferCallback cb, 
						 gpointer user_data);

/* allocate resources */
gboolean 	gst_ringbuffer_acquire 		(GstRingBuffer *buf, GstRingBufferSpec *spec);
gboolean 	gst_ringbuffer_release 		(GstRingBuffer *buf);

/* playback/pause */
gboolean 	gst_ringbuffer_play 		(GstRingBuffer *buf);
gboolean 	gst_ringbuffer_pause 		(GstRingBuffer *buf);
gboolean 	gst_ringbuffer_stop 		(GstRingBuffer *buf);

/* get status */
guint	 	gst_ringbuffer_delay 		(GstRingBuffer *buf);
guint64	 	gst_ringbuffer_played_samples	(GstRingBuffer *buf);

void	 	gst_ringbuffer_set_sample	(GstRingBuffer *buf, guint64 sample);

/* commit samples */
guint 		gst_ringbuffer_commit 		(GstRingBuffer *buf, guint64 sample, 
						 guchar *data, guint len);

/* mostly protected */
gboolean	gst_ringbuffer_prepare_read	(GstRingBuffer *buf, gint *segment, guint8 **readptr, gint *len);
void		gst_ringbuffer_clear		(GstRingBuffer *buf, gint segment);
void	 	gst_ringbuffer_advance	 	(GstRingBuffer *buf, guint advance);

G_END_DECLS

#endif /* __GST_RINGBUFFER_H__ */