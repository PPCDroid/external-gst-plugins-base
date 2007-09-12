/* GStreamer
 * Copyright (C) <2007> Sebastian Dröge <slomo@circular-chaos.org>
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

#include <glib.h>
#include <math.h>

#include "kiss_fftr_s16.h"
#include "gstfft.h"
#include "gstffts16.h"

/**
 * SECTION:gstffts16
 * @short_description: FFT functions for signed 16 bit integer samples
 *
 * #GstFFTS16 provides a FFT implementation and related functions for
 * signed 16 bit integer samples. To use this call gst_fft_s16_new() for
 * allocating a #GstFFTS16 instance with the appropiate parameters and
 * then call gst_fft_s16_fft() or gst_fft_s16_inverse_fft() to perform the
 * FFT or inverse FFT on a buffer of samples.
 *
 * After use free the #GstFFTS16 instance with gst_fft_s16_free().
 *
 * For the best performance use gst_fft_next_fast_length() to get a
 * number that is entirely a product of 2, 3 and 5 and use this as the
 * @len parameter for gst_fft_s16_new().
 *
 * The @len parameter specifies the number of samples in the time domain that
 * will be processed or generated. The number of samples in the frequency domain
 * is @len/2 + 1. To get n samples in the frequency domain use 2*n - 2 as @len.
 *
 * Before performing the FFT on time domain data it usually makes sense
 * to apply a window function to it. For this gst_fft_s16_window() can comfortably
 * be used.
 *
 * For calculating the magnitude or phase of frequency data the functions
 * gst_fft_s16_magnitude() and gst_fft_s16_phase() exist, if you want to calculate
 * the magnitude yourself note that the magnitude of the frequency data is
 * a value between 0 and 32767 and is not to be scaled by the length of the FFT.
 *
 */

/**
 * gst_fft_s16_new:
 * @len: Length of the FFT in the time domain
 * @inverse: %TRUE if the #GstFFTS16 instance should be used for the inverse FFT
 *
 * This returns a new #GstFFTS16 instance with the given parameters. It makes
 * sense to keep one instance for several calls for speed reasons.
 *
 * @len must be even and to get the best performance a product of
 * 2, 3 and 5. To get the next number with this characteristics use
 * gst_fft_next_fast_length().
 *
 * Returns: a new #GstFFTS16 instance.
 */
GstFFTS16 *
gst_fft_s16_new (gint len, gboolean inverse)
{
  GstFFTS16 *self;

  g_return_val_if_fail (len > 0, NULL);
  g_return_val_if_fail (len % 2 == 0, NULL);

  self = g_new (GstFFTS16, 1);

  self->cfg = kiss_fftr_s16_alloc (len, (inverse) ? 1 : 0, NULL, NULL);
  g_assert (self->cfg);

  self->inverse = inverse;
  self->len = len;

  return self;
}

/**
 * gst_fft_s16_fft:
 * @self: #GstFFTS16 instance for this call
 * @timedata: Buffer of the samples in the time domain
 * @freqdata: Target buffer for the samples in the frequency domain
 *
 * This performs the FFT on @timedata and puts the result in @freqdata.
 *
 * @timedata must have as many samples as specified with the @len parameter while
 * allocating the #GstFFTS16 instance with gst_fft_s16_new().
 *
 * @freqdata must be large enough to hold @len/2 + 1 #GstFFTS16Complex frequency
 * domain samples.
 *
 */
void
gst_fft_s16_fft (GstFFTS16 * self, const gint16 * timedata,
    GstFFTS16Complex * freqdata)
{
  g_return_if_fail (self);
  g_return_if_fail (!self->inverse);
  g_return_if_fail (timedata);
  g_return_if_fail (freqdata);

  kiss_fftr_s16 (self->cfg, timedata, (kiss_fft_s16_cpx *) freqdata);
}

/**
 * gst_fft_s16_inverse_fft:
 * @self: #GstFFTS16 instance for this call
 * @freqdata: Buffer of the samples in the frequency domain
 * @timedata: Target buffer for the samples in the time domain
 *
 * This performs the inverse FFT on @freqdata and puts the result in @timedata.
 *
 * @freqdata must have @len/2 + 1 samples, where @len is the parameter specified
 * while allocating the #GstFFTS16 instance with gst_fft_s16_new().
 *
 * @timedata must be large enough to hold @len time domain samples.
 *
 */
void
gst_fft_s16_inverse_fft (GstFFTS16 * self, const GstFFTS16Complex * freqdata,
    gint16 * timedata)
{
  g_return_if_fail (self);
  g_return_if_fail (self->inverse);
  g_return_if_fail (timedata);
  g_return_if_fail (freqdata);

  kiss_fftri_s16 (self->cfg, (kiss_fft_s16_cpx *) freqdata, timedata);
}

/**
 * gst_fft_s16_free:
 * @self: #GstFFTS16 instance for this call
 *
 * This frees the memory allocated for @self.
 *
 */
void
gst_fft_s16_free (GstFFTS16 * self)
{
  kiss_fftr_s16_free (self->cfg);
  g_free (self);
}

/**
 * gst_fft_s16_window:
 * @self: #GstFFTS16 instance for this call
 * @timedata: Time domain samples
 * @window: Window function to apply
 *
 * This calls the window function @window on the @timedata sample buffer.
 *
 */
void
gst_fft_s16_window (GstFFTS16 * self, gint16 * timedata, GstFFTWindow window)
{
  gint i, len;

  g_return_if_fail (self);
  g_return_if_fail (timedata);

  len = self->len;

  switch (window) {
    case GST_FFT_WINDOW_RECTANGULAR:
      /* do nothing */
      break;
    case GST_FFT_WINDOW_HAMMING:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.53836 - 0.46164 * cos (2.0 * M_PI * i / len));
      break;
    case GST_FFT_WINDOW_HANN:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.5 - 0.5 * cos (2.0 * M_PI * i / len));
      break;
    case GST_FFT_WINDOW_BARTLETT:
      for (i = 0; i < len; i++)
        timedata[i] *= (1.0 - fabs ((2.0 * i - len) / len));
      break;
    case GST_FFT_WINDOW_BLACKMAN:
      for (i = 0; i < len; i++)
        timedata[i] *= (0.42 - 0.5 * cos ((2.0 * i) / len) +
            0.08 * cos ((4.0 * i) / len));
      break;
    default:
      g_assert_not_reached ();
      break;
  }
}

/**
 * gst_fft_s16_magnitude:
 * @self: #GstFFTS16 instance for this call
 * @freqdata: Frequency domain samples
 * @magnitude: Target buffer for the magnitude
 * @decibel: %TRUE if the magnitude should be in decibel, %FALSE if it should be an amplitude
 *
 * This calculates the magnitude of @freqdata in @magnitude. Depending on the value
 * of @decibel the magnitude can be calculated in decibel or as amplitude between 0.0
 * and 1.0.
 *
 * @magnitude must be large enough to hold @len/2 + 1 values.
 *
 */
void
gst_fft_s16_magnitude (GstFFTS16 * self, GstFFTS16Complex * freqdata,
    gdouble * magnitude, gboolean decibel)
{
  gint i, len;
  gdouble val;

  g_return_if_fail (self);
  g_return_if_fail (freqdata);
  g_return_if_fail (magnitude);

  len = self->len / 2 + 1;

  for (i = 0; i < len; i++) {
    val = (gdouble) freqdata[i].r * (gdouble) freqdata[i].r
        + (gdouble) freqdata[i].i * (gdouble) freqdata[i].i;
    val = sqrt (val) / 32767.0;

    if (decibel)
      val = 20.0 * log10 (val);

    magnitude[i] = val;
  }
}

/**
 * gst_fft_s16_phase:
 * @self: #GstFFTS16 instance for this call
 * @freqdata: Frequency domain samples
 * @phase: Target buffer for the phase
 *
 * This calculates the phases of @freqdata in @phase. The returned
 * phases will be values between -pi and pi.
 *
 * @phase must be large enough to hold @len/2 + 1 values.
 *
 */
void
gst_fft_s16_phase (GstFFTS16 * self, GstFFTS16Complex * freqdata,
    gdouble * phase)
{
  gint i, len;

  g_return_if_fail (self);
  g_return_if_fail (freqdata);
  g_return_if_fail (phase);

  len = self->len / 2 + 1;

  for (i = 0; i < len; i++)
    phase[i] = atan2 (freqdata[i].i, freqdata[i].r);
}