
#ifndef AUTOWARE__BEV_OPS__BEV_POOL_CUDA_H_
#define AUTOWARE__BEV_OPS__BEV_POOL_CUDA_H_

#include <cuda_runtime_api.h>

void bev_pool(
  int b, int d, int h, int w, int n, int c, int n_intervals, const float * x,
  const int * geom_feats, const int * interval_starts, const int * interval_lengths, float * out,
  cudaStream_t & stream);

#endif  // AUTOWARE__BEV_OPS__BEV_POOL_CUDA_H_
