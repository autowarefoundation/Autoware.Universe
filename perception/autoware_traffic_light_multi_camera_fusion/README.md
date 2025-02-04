# autoware_traffic_light_multi_camera_fusion

## Overview

`autoware_traffic_light_multi_camera_fusion` performs traffic light signal fusion which can be summarized as the following two tasks:

1. Multi-Camera-Fusion: fusion each traffic light signal detected by different cameras.
2. Group-Fusion: Fusion each traffic light signal within the same group, which means traffic lights share the same regulatory element ID defined in lanelet2 map.

The fusion method is below.

1. Use the results of the new timestamp if the results are from the same sensor
2. Use the results that are not `elements.size() == 1 && color == UNKNOWN && shape == UNKNOWN`
3. Use the results that each vertex of ROI is not at the edge of the image
4. Use the results of high confidence

## Input topics

For every camera, the following three topics are subscribed:

| Name                                                  | Type                                             | Description                           |
| ----------------------------------------------------- | ------------------------------------------------ | ------------------------------------- |
| `~/<camera_namespace>/camera_info`                    | sensor_msgs::msg::CameraInfo                     | camera info from map_based_detector   |
| `~/<camera_namespace>/detection/rois`                 | tier4_perception_msgs::msg::TrafficLightRoiArray | detection roi from fine_detector      |
| `~/<camera_namespace>/classification/traffic_signals` | tier4_perception_msgs::msg::TrafficLightArray    | classification result from classifier |

You don't need to configure these topics manually. Just provide the `camera_namespaces` parameter and the node will automatically extract the `<camera_namespace>` and create the subscribers.

## Output topics

| Name                       | Type                                                  | Description                        |
| -------------------------- | ----------------------------------------------------- | ---------------------------------- |
| `~/output/traffic_signals` | autoware_perception_msgs::msg::TrafficLightGroupArray | traffic light signal fusion result |

## Node parameters

{{ json_to_markdown("perception/autoware_traffic_light_multi_camera_fusion/schema/traffic_light_multi_camera_fusion.schema.json") }}
