// Copyright 2024 TIER IV, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "autoware/tensorrt_rtmdet/preprocess.hpp"

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace autoware::tensorrt_rtmdet
{
constexpr size_t block = 512;
constexpr size_t max_blocks_per_dim = 65535;
constexpr int C = 3;

dim3 cuda_gridsize(size_t n)
{
  size_t k = (n - 1) / block + 1;
  size_t x = k;
  size_t y = 1;
  if (x > max_blocks_per_dim) {
    x = ceil(sqrt(k));
    y = (n - 1) / (x * block) + 1;
  }
  dim3 d;
  d.x = x;
  d.y = y;
  d.z = 1;
  return d;
}

__device__ double lerp1d(int a, int b, float w)
{
  return fma(w, (float)b, fma(-w, (float)a, (float)a));
}

__device__ float lerp2d(int f00, int f01, int f10, int f11, float centroid_h, float centroid_w)
{
  centroid_w = (1 + lroundf(centroid_w) - centroid_w) / 2;
  centroid_h = (1 + lroundf(centroid_h) - centroid_h) / 2;

  float r0, r1, r;
  r0 = (float)lround(lerp1d(f00, f01, centroid_w));
  r1 = (float)lround(lerp1d(f10, f11, centroid_w));

  r = lerp1d((int)r0, (int)r1, centroid_h);
  return r;
}

__global__ void resize_bilinear_kernel(
  int N, unsigned char * dst_img, unsigned char * src_img, int dst_w, int src_h, int src_w,
  float stride_h, float stride_w)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int W = dst_w;

  int n = 0;

  int w = index % W;
  int h = index / W;

  float centroid_h, centroid_w;
  centroid_h = stride_h * (float)(h + 0.5);
  centroid_w = stride_w * (float)(w + 0.5);

  int f00, f01, f10, f11;

  int src_h_idx = lroundf(centroid_h) - 1;
  int src_w_idx = lroundf(centroid_w) - 1;
  if (src_h_idx < 0) {
    src_h_idx = 0;
  }
  if (src_w_idx < 0) {
    src_w_idx = 0;
  }

  index = C * w + C * W * h;
#pragma unroll
  for (int c = 0; c < C; c++) {
    f00 = n * src_h * src_w * C + src_h_idx * src_w * C + src_w_idx * C + c;
    f01 = n * src_h * src_w * C + src_h_idx * src_w * C + (src_w_idx + 1) * C + c;
    f10 = n * src_h * src_w * C + (src_h_idx + 1) * src_w * C + src_w_idx * C + c;
    f11 = n * src_h * src_w * C + (src_h_idx + 1) * src_w * C + (src_w_idx + 1) * C + c;

    float rs = lroundf(lerp2d(
      (int)src_img[f00], (int)src_img[f01], (int)src_img[f10], (int)src_img[f11], centroid_h,
      centroid_w));
    dst_img[index + c] = (unsigned char)rs;
  }
}

void resize_bilinear_gpu(
  unsigned char * dst, unsigned char * src, int d_w, int d_h, int s_w, int s_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  float stride_h = (float)s_h / (float)d_h;
  float stride_w = (float)s_w / (float)d_w;

  resize_bilinear_kernel<<<cuda_gridsize(N), block, 0, stream>>>(
    N, dst, src, d_w, s_h, s_w, stride_h, stride_w);
}

__global__ void letterbox_kernel(
  int N, unsigned char * dst_img, unsigned char * src_img, int dst_w, int src_w, int letter_bot,
  int letter_right)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int W = dst_w;

  int w = index % W;
  int h = index / W;

  index = (C * w) + (C * W * h);
  int index2 = (C * w) + (C * src_w * h);
#pragma unroll
  for (int c = 0; c < C; c++) {
    dst_img[index + c] =
      (w >= letter_right || h >= letter_bot) ? (unsigned int)114 : src_img[index2 + c];
  }
}

void letterbox_gpu(
  unsigned char * dst, unsigned char * src, int d_w, int d_h, int s_w, int s_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  const float scale = std::min(d_w / (float)s_w, d_h / (float)s_h);
  int r_h = (int)(scale * s_h);
  int r_w = (int)(scale * s_w);

  letterbox_kernel<<<cuda_gridsize(N), block, 0, stream>>>(N, dst, src, d_w, r_w, r_h, r_w);
}

__global__ void nhwc_to_nchw_kernel(
  int N, unsigned char * dst_img, unsigned char * src_img, int height, int width)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int x = index % width;
  int y = index / width;
  int src_index;
  int dst_index;
  for (int c = 0; c < C; c++) {
    src_index = c + (C * x) + (C * width * y);
    dst_index = x + (width * y) + (width * height * c);
    dst_img[dst_index] = src_img[src_index];
  }
}

void nhwc_to_nchw_gpu(
  unsigned char * dst, unsigned char * src, int d_w, int d_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  nhwc_to_nchw_kernel<<<cuda_gridsize(N), block, 0, stream>>>(N, dst, src, d_h, d_w);
}

__global__ void nchw_to_nhwc_kernel(
  int N, unsigned char * dst, unsigned char * src, int height, int width)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int x = index % width;
  int y = index / width;
  int src_index;
  int dst_index;
  for (int c = 0; c < C; c++) {
    // NHWC
    dst_index = c + (C * x) + (C * width * y);
    // NCHW
    src_index = x + (width * y) + (width * height * c);
    dst[dst_index] = src[src_index];
  }
}

void nchw_to_nhwc_gpu(
  unsigned char * dst, unsigned char * src, int d_w, int d_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  nchw_to_nhwc_kernel<<<cuda_gridsize(N), block, 0, stream>>>(N, dst, src, d_h, d_w);
}

__global__ void to_float_kernel(int N, float * dst32, unsigned char * src8, int height, int width)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int x = index % width;
  int y = index / width;
  int dst_index;
  for (int c = 0; c < C; c++) {
    // NCHW
    dst_index = x + (width * y) + (width * height * c);
    dst32[dst_index] = (float)(src8[dst_index]);
  }
}

void to_float_gpu(float * dst32, unsigned char * src, int d_w, int d_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  to_float_kernel<<<cuda_gridsize(N), block, 0, stream>>>(N, dst32, src, d_h, d_w);
}

__global__ void resize_bilinear_letterbox_kernel(
  int N, unsigned char * dst_img, unsigned char * src_img, int dst_w, int src_h, int src_w,
  float scale, int letter_bot, int letter_right)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int W = dst_w;
  int n = 0;  // index / (C*W*H);

  int w = index % W;
  int h = index / W;

  float centroid_h, centroid_w;
  centroid_h = scale * (float)(h + 0.5);
  centroid_w = scale * (float)(w + 0.5);

  int f00, f01, f10, f11;

  int src_h_idx = (int)lroundf(centroid_h) - 1;
  int src_w_idx = (int)lroundf(centroid_w) - 1;
  if (src_h_idx < 0) {
    src_h_idx = 0;
  }
  if (src_w_idx < 0) {
    src_w_idx = 0;
  }
  if (src_h_idx >= src_h) {
    src_h_idx = src_h - 1;
  }
  if (src_w_idx >= src_w) {
    src_w_idx = src_w - 1;
  }

  index = (C * w) + (C * W * h);
#pragma unroll
  for (int c = 0; c < C; c++) {
    f00 = n * src_h * src_w * C + src_h_idx * src_w * C + src_w_idx * C + c;
    f01 = n * src_h * src_w * C + src_h_idx * src_w * C + (src_w_idx + 1) * C + c;
    f10 = n * src_h * src_w * C + (src_h_idx + 1) * src_w * C + src_w_idx * C + c;
    f11 = n * src_h * src_w * C + (src_h_idx + 1) * src_w * C + (src_w_idx + 1) * C + c;

    float rs = lroundf(lerp2d(
      (int)src_img[f00], (int)src_img[f01], (int)src_img[f10], (int)src_img[f11], centroid_h,
      centroid_w));
    dst_img[index + c] = (unsigned char)rs;
    dst_img[index + c] = (h >= letter_bot) ? (unsigned int)114 : dst_img[index + c];
    dst_img[index + c] = (w >= letter_right) ? (unsigned int)114 : dst_img[index + c];
  }
}

void resize_bilinear_letterbox_gpu(
  unsigned char * dst, unsigned char * src, int d_w, int d_h, int s_w, int s_h, cudaStream_t stream)
{
  int N = d_w * d_h;
  const float scale = std::min(d_w / (float)s_w, d_h / (float)s_h);
  int r_h = (int)(scale * s_h);
  int r_w = (int)(scale * s_w);
  resize_bilinear_letterbox_kernel<<<cuda_gridsize(N), block, 0, stream>>>(
    N, dst, src, d_w, s_h, s_w, 1.0 / scale, r_h, r_w);
}

__global__ void resize_bilinear_letterbox_nhwc_to_nchw32_kernel(
  int N, float * dst_img, unsigned char * src_img, int dst_h, int dst_w, int src_h, int src_w,
  float scale, int letter_bot, int letter_right, float norm)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int H = dst_h;
  int W = dst_w;

  int w = index % W;
  int h = index / W;

  float centroid_h, centroid_w;
  centroid_h = scale * (float)(h + 0.5);
  centroid_w = scale * (float)(w + 0.5);

  int f00, f01, f10, f11;

  int src_h_idx = lroundf(centroid_h) - 1;
  int src_w_idx = lroundf(centroid_w) - 1;
  src_h_idx = (src_h_idx < 0) ? 0 : src_h_idx;
  src_h_idx = (src_h_idx >= (src_h - 1)) ? src_h - 2 : src_h_idx;
  src_w_idx = (src_w_idx < 0) ? 0 : src_w_idx;
  src_w_idx = (src_w_idx >= (src_w - 1)) ? src_w - 2 : src_w_idx;

  int stride = src_w * C;

#pragma unroll
  for (int c = 0; c < C; c++) {
    f00 = src_h_idx * stride + src_w_idx * C + c;
    f01 = src_h_idx * stride + (src_w_idx + 1) * C + c;
    f10 = (src_h_idx + 1) * stride + src_w_idx * C + c;
    f11 = (src_h_idx + 1) * stride + (src_w_idx + 1) * C + c;

    float rs = lroundf(lerp2d(
      (int)src_img[f00], (int)src_img[f01], (int)src_img[f10], (int)src_img[f11], centroid_h,
      centroid_w));

    // NHCW
    int dst_index = w + (W * h) + (W * H * c);

    dst_img[dst_index] = (float)rs;
    dst_img[dst_index] = (h >= letter_bot) ? 114.0 : dst_img[dst_index];
    dst_img[dst_index] = (w >= letter_right) ? 114.0 : dst_img[dst_index];
    dst_img[dst_index] *= norm;
  }
}

void resize_bilinear_letterbox_nhwc_to_nchw32_gpu(
  float * dst, unsigned char * src, int d_w, int d_h, int s_w, int s_h, float norm,
  cudaStream_t stream)
{
  int N = d_w * d_h;
  const float scale = std::min(d_w / (float)s_w, d_h / (float)s_h);
  int r_h = scale * s_h;
  int r_w = scale * s_w;

  resize_bilinear_letterbox_nhwc_to_nchw32_kernel<<<cuda_gridsize(N), block, 0, stream>>>(
    N, dst, src, d_h, d_w, s_h, s_w, 1.0 / scale, r_h, r_w, norm);
}

__global__ void resize_bilinear_letterbox_nhwc_to_nchw32_batch_kernel(
  int N, float * dst_img, unsigned char * src_img, int dst_h, int dst_w, int src_h, int src_w,
  float scale_h, float scale_w, int letter_bot, int letter_right, int batch, const float * mean,
  const float * std)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int H = dst_h;
  int W = dst_w;

  int w = index % W;
  int h = index / (W);
  float centroid_h, centroid_w;
  centroid_h = scale_h * (float)(h + 0.5);
  centroid_w = scale_w * (float)(w + 0.5);

  int f00, f01, f10, f11;

  int src_h_idx = lroundf(centroid_h) - 1;
  int src_w_idx = lroundf(centroid_w) - 1;
  src_h_idx = (src_h_idx < 0) ? 0 : src_h_idx;
  src_h_idx = (src_h_idx >= (src_h - 1)) ? src_h - 2 : src_h_idx;
  src_w_idx = (src_w_idx < 0) ? 0 : src_w_idx;
  src_w_idx = (src_w_idx >= (src_w - 1)) ? src_w - 2 : src_w_idx;

  int stride = src_w * C;
  int b_stride = src_h * src_w * C;

#pragma unroll
  for (int b = 0; b < batch; b++) {
    for (int c = 0; c < C; c++) {
      // NHWC
      f00 = src_h_idx * stride + src_w_idx * C + c + b * b_stride;
      f01 = src_h_idx * stride + (src_w_idx + 1) * C + c + b * b_stride;
      f10 = (src_h_idx + 1) * stride + src_w_idx * C + c + b * b_stride;
      f11 = (src_h_idx + 1) * stride + (src_w_idx + 1) * C + c + b * b_stride;

      float rs = lroundf(lerp2d(
        (int)src_img[f00], (int)src_img[f01], (int)src_img[f10], (int)src_img[f11], centroid_h,
        centroid_w));

      // NCHW
      int dst_index = w + (W * h) + (W * H * c) + b * (W * H * C);

      dst_img[dst_index] = (float)rs;
      dst_img[dst_index] = (h >= letter_bot) ? 114.0 : dst_img[dst_index];
      dst_img[dst_index] = (w >= letter_right) ? 114.0 : dst_img[dst_index];
      //            dst_img[dst_index] *= norm;
      dst_img[dst_index] -= mean[c];
      dst_img[dst_index] /= std[c];
    }
  }
}

void resize_bilinear_letterbox_nhwc_to_nchw32_batch_gpu(
  float * dst, unsigned char * src, int d_w, int d_h, int s_w, int s_h, int batch,
  const float * mean, const float * std, cudaStream_t stream)
{
  int N = d_w * d_h;

  const float scale_h = d_h / (float)s_h;
  const float scale_w = d_w / (float)s_w;

  int r_h = scale_h * s_h;
  int r_w = scale_w * s_w;

  float *d_mean, *d_std;
  cudaMalloc(&d_mean, 3 * sizeof(float));
  cudaMalloc(&d_std, 3 * sizeof(float));
  cudaMemcpyAsync(d_mean, mean, 3 * sizeof(float), cudaMemcpyHostToDevice, stream);
  cudaMemcpyAsync(d_std, std, 3 * sizeof(float), cudaMemcpyHostToDevice, stream);

  resize_bilinear_letterbox_nhwc_to_nchw32_batch_kernel<<<cuda_gridsize(N), block, 0, stream>>>(
    N, dst, src, d_h, d_w, s_h, s_w, 1.0 / scale_h, 1.0 / scale_w, r_h, r_w, batch, d_mean, d_std);
}

__global__ void argmax_gpu_kernel(
  int N, unsigned char * dst, float * src, int dst_h, int dst_w, int src_c, int src_h, int src_w,
  int batch)
{
  // NHWC
  int index = (blockIdx.x + blockIdx.y * gridDim.x) * blockDim.x + threadIdx.x;

  if (index >= N) return;
  int w = index % dst_w;
  int h = index / (dst_w);

  for (int b = 0; b < batch; b++) {
    float max_prob = 0.0;
    int max_index = 0;
    int dst_index = w + dst_w * h + b * dst_h * dst_w;
    for (int c = 0; c < src_c; c++) {
      int src_index = w + src_w * h + c * src_h * src_w + b * src_c * src_h * src_w;
      max_index = max_prob < src[src_index] ? c : max_index;
      max_prob = max_prob < src[src_index] ? src[src_index] : max_prob;
    }
    dst[dst_index] = max_index;
  }
}

void argmax_gpu(
  unsigned char * dst, float * src, int d_w, int d_h, int s_w, int s_h, int s_c, int batch,
  cudaStream_t stream)
{
  int N = d_w * d_h;
  argmax_gpu_kernel<<<cuda_gridsize(N), block, 0, stream>>>(
    N, dst, src, d_h, d_w, s_c, s_h, s_w, batch);
}

}  // namespace autoware::tensorrt_rtmdet
