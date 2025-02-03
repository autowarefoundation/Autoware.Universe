# low_intensity_cluster_filter

## Purpose

The `low_intensity_cluster_filter` is a node that filters clusters based on the intensity of their pointcloud.

Mainly this focuses on filtering out unknown objects with very low intensity pointcloud, such as fail detection of unknown objects caused by raindrop or water splash from ego or other fast moving vehicles.

## Inner-workings / Algorithms

## Inputs / Outputs

### Input

| Name           | Type                                                     | Description            |
| -------------- | -------------------------------------------------------- | ---------------------- |
| `input/object` | `tier4_perception_msgs::msg::DetectedObjectsWithFeature` | input detected objects |

### Output

| Name            | Type                                                     | Description               |
| --------------- | -------------------------------------------------------- | ------------------------- |
| `output/object` | `tier4_perception_msgs::msg::DetectedObjectsWithFeature` | filtered detected objects |

## Parameters

{{ json_to_markdown("perception/autoware_raindrop_cluster_filter>/schema/low_intensity_cluster_filter.schema.json") }}

## Assumptions / Known limits

## (Optional) Error detection and handling

## (Optional) Performance characterization

## (Optional) References/External links

## (Optional) Future extensions / Unimplemented parts
