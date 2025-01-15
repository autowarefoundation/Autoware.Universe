# autoware_traffic_light_visualization

## Purpose

The `autoware_traffic_light_visualization` is a package that includes two visualizing nodes:

### traffic_light_map_visualizer

The node shows traffic light's color and position on rviz as markers.

![traffic light map visualization](./images/map-visualization.png)

-

### traffic_light_roi_visualizer

The node draws the result of traffic light recognition on the input image as shown in the following figure and publishes it.

![traffic light roi visualization](./images/roi-visualization.png)

- The colors `~/input/rois` and `~/input/rough/rois` are the same as `color` whose `shape` is CIRCLE in `~/input/traffic_signals` (unknown shows as white).
- The labels in the upper left of `~/input/rois` shows `shape` and `confidence` in `~/input/traffic_signals`.

## Inner-workings / Algorithms

## Inputs / Outputs

### traffic_light_map_visualizer

#### Input

| Name                 | Type                                                  | Description              |
| -------------------- | ----------------------------------------------------- | ------------------------ |
| `~/input/tl_state`   | autoware_perception_msgs::msg::TrafficLightGroupArray | status of traffic lights |
| `~/input/vector_map` | autoware_map_msgs::msg::LaneletMapBin                 | vector map               |

#### Output

| Name                     | Type                                 | Description                                          |
| ------------------------ | ------------------------------------ | ---------------------------------------------------- |
| `~/output/traffic_light` | visualization_msgs::msg::MarkerArray | marker array that indicates status of traffic lights |

### traffic_light_roi_visualizer

#### Input

| Name                          | Type                                             | Description                                                      |
| ----------------------------- | ------------------------------------------------ | ---------------------------------------------------------------- |
| `~/input/traffic_signals`     | tier4_perception_msgs::msg::TrafficLightArray    | status of traffic lights                                         |
| `~/input/image`               | sensor_msgs::msg::Image                          | the image captured by perception cameras                         |
| `~/input/rois`                | tier4_perception_msgs::msg::TrafficLightRoiArray | the ROIs detected by `autoware_traffic_light_fine_detector`      |
| `~/input/rough/rois` (option) | tier4_perception_msgs::msg::TrafficLightRoiArray | the ROIs detected by `autoware_traffic_light_map_based_detector` |

#### Output

| Name             | Type                    | Description            |
| ---------------- | ----------------------- | ---------------------- |
| `~/output/image` | sensor_msgs::msg::Image | output image with ROIs |

## Parameters

### traffic_light_map_visualizer

None

### traffic_light_roi_visualizer

#### Node Parameters

{{json_to_markdown("perception/autoware_traffic_light_visualization/schema/traffic_light_visualization.schema.json")}}

## Assumptions / Known limits

## (Optional) Error detection and handling

## (Optional) Performance characterization

## (Optional) References/External links

## (Optional) Future extensions / Unimplemented parts
