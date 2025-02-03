# autoware_tensorrt_rtmdet

## Purpose

RTMDet is a real-time instance segmentation model which can be used for detecting objects like cars, pedestrians,
bicycles, etc. in a scene. This package provides a ROS 2 interface for RTMDet using TensorRT.

## Inner-workings / Algorithms

## Inputs / Outputs

### Input

| Name       | Type                | Description     |
| ---------- | ------------------- | --------------- |
| `in/image` | `sensor_msgs/Image` | The input image |

### Output

| Name              | Type                                                 | Description                                                         |
| ----------------- | ---------------------------------------------------- | ------------------------------------------------------------------- |
| `out/objects`     | `tier4_perception_msgs/DetectedObjectsWithFeature`   | The detected objects with 2D bounding boxes and scores              |
| `out/mask`        | `autoware_internal_perception_msgs/SegmentationMask` | The instance segmentation mask                                      |
| `out/color_mask`  | `sensor_msgs/Image`                                  | The colorized image of instance segmentation mask for visualization |
| `out/debug_image` | `sensor_msgs/Image`                                  | The image with 2D bounding boxes for visualization                  |

## Parameters

{{ json_to_markdown("perception/autoware_tensorrt_rtmdet/schema/rtmdet.schema.json") }}

## Assumptions / Known limits

## Onnx model

A sample model is provided in `autoware_data` folder by ansible script on env preparation stage. If you cannot find the
model, you follow instructions from the
[link](https://github.com/autowarefoundation/autoware/tree/main/ansible/roles/artifacts) to download the model.

The shared model was trained by open-mmlab using the COCO dataset. For more details,
see [link](https://github.com/open-mmlab/mmdetection/tree/3.x/configs/rtmdet#instance-segmentation).

### Package acceptable model generation

Users can generate their own model using official RTMDet repository. Please refer to the
[official repository](https://github.com/open-mmlab/mmdetection/tree/3.x/configs/rtmdet#rtmdet-an-empirical-study-of-designing-real-time-object-detectors)

## Label file

A sample label file (named label.txt) and instance segmentation color map file (name color_map.csv) are also
downloaded automatically during env preparation process.

These files are used to map the class index to class name and color respectively.

## Reference repositories

- <https://github.com/open-mmlab/mmdetection/tree/3.x/configs/rtmdet#rtmdet-an-empirical-study-of-designing-real-time-object-detectors>

## Citation

```bibtex
@misc{lyu2022rtmdetempiricalstudydesigning,
title={RTMDet: An Empirical Study of Designing Real-Time Object Detectors},
author={Chengqi Lyu and Wenwei Zhang and Haian Huang and Yue Zhou and Yudong Wang and Yanyi Liu and Shilong Zhang and Kai Chen},
year={2022},
eprint={2212.07784},
archivePrefix={arXiv},
primaryClass={cs.CV},
url={https://arxiv.org/abs/2212.07784},
}
```
