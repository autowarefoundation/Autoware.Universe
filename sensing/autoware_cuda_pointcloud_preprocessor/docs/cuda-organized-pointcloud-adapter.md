# cuda_organized_pointcloud_adapter

## Purpose

The node `cuda_pointcloud_preprocessor` expects a 2D pointcloud where every row represents a single channel/ring and each row's points are in non decreasing azimuth order.

To utilize the previously mentioned node, this node provides an adapter to convert standard flat pointclouds (`height` equals to 1) to the required 2D tensor.

In addition, this node uses the `cuda_blackboard`, a cuda transport layer that enables a zero-copy mechanism between GPU and GPU memory for both input and output.

## Inner-workings / Algorithms

To create the required 2D tensor, this node iterates the input pointcloud sequentially, filling the output 2D tensor depending on the input point's channel.

The output tensor's size is also estimated in this node, based on the largest `channel` value and the maximum number of points per channel observed so far.

## Inputs / Outputs

### Input

| Name                 | Type                            | Description               |
| -------------------- | ------------------------------- | ------------------------- |
| `~/input/pointcloud` | `sensor_msgs::msg::PointCloud2` | Input pointcloud's topic. |

### Output

| Name                       | Type                                             | Description                              |
| -------------------------- | ------------------------------------------------ | ---------------------------------------- |
| `~/output/pointcloud`      | `sensor_msgs::msg::PointCloud2`                  | Processed pointcloud's topic             |
| `~/output/pointcloud/cuda` | `negotiated_interfaces/msg/NegotiatedTopicsInfo` | Processed pointcloud's negotiation topic |

## Parameters

### Core Parameters

{{ json_to_markdown("sensing/autoware_cuda_pointcloud_preprocessor/schema/cuda_pointcloud_preprocessor.schema.schema.json") }}

## Assumptions / Known limits

- This algorithm assumes that the input points will be in non-decreasing azimuth order (per ring).
- This node expects that the input pointcloud is flat (`height` equals to 1) and follows the `autoware::point_types::PointXYZIRC` layout defined in the `autoware_point_types` package.
