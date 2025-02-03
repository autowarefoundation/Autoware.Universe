# The `autoware_traffic_light_selector` Package

## Overview

`autoware_traffic_light_selector` selects the interest traffic light from the list of detected traffic lights by deep learning neural network (DNN) based on the expect ROIs and rough ROIs information and then assign traffic_light_id for them.

## Input topics

| Name                  | Type                                                   | Description                                                          |
| --------------------- | ------------------------------------------------------ | -------------------------------------------------------------------- |
| `input/detected_rois` | tier4_perception_msgs::msg::DetectedObjectsWithFeature | detected traffic light by DNN                                        |
| `input/rough_rois`    | tier4_perception_msgs::msg::TrafficLightRoiArray       | location of traffic lights in image corresponding to the camera info |
| `input/expect_rois`   | tier4_perception_msgs::msg::TrafficLightRoiArray       | location of traffic lights in image without any offset               |

## Output topics

| Name                        | Type                                        | Description                        |
| --------------------------- | ------------------------------------------- | ---------------------------------- |
| `output/traffic_light_rois` | tier4_perception_msgs::TrafficLightRoiArray | detected traffic light of interest |

## Node parameters

{{json_to_markdown("perception/autoware_traffic_light_selector/schema/traffic_light_selector.schema.json")}}

N/A
