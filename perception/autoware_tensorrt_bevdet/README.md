# tensorrt_bevdet <!-- cspell: ignore bevdet -->

## Purpose

The core algorithm, named `BEVDet`, it unifies multi-view images into the perspective of BEV for 3D object detection task.

## Inner-workings / Algorithms

### Cite

<!-- cspell: ignore Junjie Huang, Guan Huang -->

- Junjie Huang, Guan Huang, "BEVPoolv2: A Cutting-edge Implementation of BEVDet Toward Deployment", [[ref](https://arxiv.org/pdf/2211.17111)]
- [bevdet_vendor](https://github.com/autowarefoundation/bevdet_vendor) package are copied from the [original codes](https://github.com/LCH1238/bevdet-tensorrt-cpp/tree/one) (The TensorRT, C++ implementation by LCH1238) and modified.
- This package is ported version toward Autoware from [bevdet_vendor](https://github.com/autowarefoundation/bevdet_vendor).

## Inputs / Outputs

### Inputs

| Name                               | Type                           | Description                         |
| ---------------------------------- | ------------------------------ | ----------------------------------- |
| `~/input/topic_img_fl`             | `sensor_msgs::msg::Image`      | input front_left camera image       |
| `~/input/topic_img_f`              | `sensor_msgs::msg::Image`      | input front camera image            |
| `~/input/topic_img_fr`             | `sensor_msgs::msg::Image`      | input front_right camera image      |
| `~/input/topic_img_bl`             | `sensor_msgs::msg::Image`      | input back_left camera image        |
| `~/input/topic_img_b`              | `sensor_msgs::msg::Image`      | input back camera image             |
| `~/input/topic_img_br`             | `sensor_msgs::msg::Image`      | input back_right camera image       |
| `~/input/topic_img_fl/camera_info` | `sensor_msgs::msg::CameraInfo` | input front_left camera parameters  |
| `~/input/topic_img_f/camera_info`  | `sensor_msgs::msg::CameraInfo` | input front camera parameters       |
| `~/input/topic_img_fr/camera_info` | `sensor_msgs::msg::CameraInfo` | input front_right camera parameters |
| `~/input/topic_img_bl/camera_info` | `sensor_msgs::msg::CameraInfo` | input back_left camera parameters   |
| `~/input/topic_img_b/camera_info`  | `sensor_msgs::msg::CameraInfo` | input back camera parameters        |
| `~/input/topic_img_br/camera_info` | `sensor_msgs::msg::CameraInfo` | input back_right camera parameters  |

### Outputs

| Name             | Type                                             | Description      |
| ---------------- | ------------------------------------------------ | ---------------- |
| `~/output/boxes` | `autoware_perception_msgs::msg::DetectedObjects` | detected objects |

## How to Use Tensorrt BEVDet Node

1. launch `tensorrt_bevdet_node`

   ```bash

   ros2 launch autoware_tensorrt_bevdet tensorrt_bevdet_node.launch.xml
   ```

2. play ros2 bag of nuScenes data

   Please refer to open source repository [ros2_dataset_bridge](https://github.com/Owen-Liuyuxuan/ros2_dataset_bridge) to publish the ROS 2 topics for NuScenes dataset.

## Trained Models

You can download the onnx format of trained models by clicking on the links below.

- BEVDet: [bevdet_one_lt_d.onnx](https://drive.google.com/file/d/1eMGJfdCVlDPBphBTjMcnIh3wdW7Q7WZB/view?usp=sharing)

The `BEVDet` model was trained in `NuScenes` dataset for 20 epochs.

## Limitation

The model is trained on open-source dataset `NuScenes` and has poor generalization on its own dataset, If you want to use this model to infer your data, you need to retrain it.

## Training BEVDet Model

If you want to train model using the [TIER IV's internal database(~2600 key frames)](https://drive.google.com/file/d/1UaarK88HZu09sf7Ix-bEVl9zGNGFwTVL/view?usp=sharing), please refer to the following repositories:[BEVDet adapted to TIER IV dataset](https://github.com/cyn-liu/BEVDet/tree/train_export).

## References/External links

[1] <https://github.com/HuangJunJie2017/BEVDet/tree/dev2.1>

[2] <https://github.com/LCH1238/BEVDet/tree/export>

[3] <https://github.com/LCH1238/bevdet-tensorrt-cpp/tree/one>
