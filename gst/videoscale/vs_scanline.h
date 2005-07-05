/*
 * Image Scaling Functions
 * Copyright (c) 2005 David A. Schleef <ds@schleef.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __VS_SCANLINE_H__
#define __VS_SCANLINE_H__

#include <glib.h>

G_BEGIN_DECLS

void vs_scanline_downsample_Y (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_Y (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_Y (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_Y (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_RGBA (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_RGBA (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_RGBA (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_RGBA (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_RGB (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_RGB (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_RGB (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_RGB (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_YUYV (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_YUYV (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_YUYV (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_YUYV (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_UYVY (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_UYVY (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_UYVY (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_UYVY (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_RGB565 (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_RGB565 (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_RGB565 (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_RGB565 (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

void vs_scanline_downsample_RGB555 (guint8 *dest, guint8 *src, int n);
void vs_scanline_resample_nearest_RGB555 (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_resample_linear_RGB555 (guint8 *dest, guint8 *src, int n, int *accumulator, int increment);
void vs_scanline_merge_linear_RGB555 (guint8 *dest, guint8 *src1, guint8 *src2, int n, int x);

G_END_DECLS

#endif
