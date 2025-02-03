# autoware_cuda_pointcloud_preprocessor

## Purpose

The pointcloud preprocessing implemented in `autoware_pointcloud_preprocessor` has been thoroughly tested in autoware. However, the latency it introduces does not scale well with modern LiDAR devices due to the high number of points they introduce.

To alleviate this issue, this package reimplements most of the pipeline presented in `autoware_pointcloud_preprocessor` leveraging the use of GPGPUs. In particular, this package makes use of CUDA to provide accelerated versions of the already established implementations, while also maintaining compatibility with normal ROS nodes/topics. <!-- cSpell: ignore GPGPUs >

## Inner-workings / Algorithms

A detailed description of each filter's algorithm is available in the following links.

| Filter Name                       | Description                                                                                                                                  | Detail                                            |
| --------------------------------- | -------------------------------------------------------------------------------------------------------------------------------------------- | ------------------------------------------------- |
| cuda_organized_pointcloud_adapter | Organizes a pointcloud per ring/channel, so that the memory layout allows parallel processing in cuda                                        | [link](docs/cuda-organized-pointcloud-adapter.md) |
| cuda_pointcloud_preprocessor      | Implements the cropping, distortion correction, and outlier filtering (ring-based) of the `autoware_pointcloud_preprocessor`'s CPU versions. | [link](docs/cuda-pointcloud-preprocessor.md)      |

## (Optional) Future extensions / Unimplemented parts

The subsample filters implemented in `autoware_pointcloud_preprocessor` will have similar counterparts in this package.
