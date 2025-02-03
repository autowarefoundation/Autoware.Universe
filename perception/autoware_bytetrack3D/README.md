# bytetrack3d

## Purpose

Algorithm `ByteTrack3D` is improved based on 2D tracking algorithm `ByteTrack`, mainly aims to perform 3D multi-object tracking.
Because the algorithm associates almost every detection box including ones with low detection scores, the number of false negatives is expected to decrease by using it.

## Inner-workings / Algorithms

### Cite

<!-- cspell: ignore Yifu Peize Jiang Dongdong Fucheng Weng Zehuan Xinggang -->

- Yifu Zhang, Peize Sun, Yi Jiang, Dongdong Yu, Fucheng Weng, Zehuan Yuan, Ping Luo, Wenyu Liu, and Xinggang Wang,
  "ByteTrack: Multi-Object Tracking by Associating Every Detection Box", in the proc. of the ECCV
  2022, [[ref](https://arxiv.org/abs/2110.06864)]
- This package is ported version toward Autoware from [this repository](https://github.com/ifzhang/ByteTrack)
  (The C++ implementation by the ByteTrack's authors)

### 3d tracking modification from original codes

Design a 3D tracker based on the tracking concept of `ByteTrack`, mainly changing the state vector of the Kalman filter.  
Original codes use the `top-left` and `size` as the state vector, and for 3D tracking, we uses `3d-center-point` and `3d-size` and `yaw` as the state vector.

Kalman filter settings can be controlled by the parameters in `config/kalman_filter.param.yaml`.

## Inputs / Outputs

### bytetrack3d_node

#### Input

| Name         | Type                                             | Description                                 |
| ------------ | ------------------------------------------------ | ------------------------------------------- |
| `in/objects` | `autoware_perception_msgs::msg::DetectedObjects` | The detected objects with 3D bounding boxes |

#### Output

| Name          | Type                                            | Description                    |
| ------------- | ----------------------------------------------- | ------------------------------ |
| `out/objects` | `autoware_perception_msgs::msg::TrackedObjects` | The 3D tracking bounding boxes |

## Parameters

### bytetrack3d_node

| Name                  | Type | Default Value | Description                                              |
| --------------------- | ---- | ------------- | -------------------------------------------------------- |
| `track_buffer_length` | int  | 30            | The frame count that a tracklet is considered to be lost |

## Assumptions/Known limits

## Reference repositories

- <https://github.com/ifzhang/ByteTrack>

## License

The codes under the `lib` directory are copied from [the original codes](https://github.com/ifzhang/ByteTrack/tree/72ca8b45d36caf5a39e949c6aa815d9abffd1ab5/deploy/TensorRT/cpp) and modified.
The original codes belong to the MIT license stated as follows, while this ported packages are provided with Apache License 2.0:

> MIT License
>
> Copyright (c) 2021 Yifu Zhang
>
> Permission is hereby granted, free of charge, to any person obtaining a copy
> of this software and associated documentation files (the "Software"), to deal
> in the Software without restriction, including without limitation the rights
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
> copies of the Software, and to permit persons to whom the Software is
> furnished to do so, subject to the following conditions:
>
> The above copyright notice and this permission notice shall be included in all
> copies or substantial portions of the Software.
>
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
> SOFTWARE.
