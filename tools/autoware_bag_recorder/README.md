# autoware_bag_recorder

## Purpose

`autoware_bag_recorder` is a ROS package designed to facilitate the recording of essential topics into bag files. The package is equipped with various configuration options to precisely define the specifications for recording.

## Inputs / Outputs

### Input

The package subscribes to the following topics for input:

| Name                         | Type                                | Description                                                                                           |
| ---------------------------- | ----------------------------------- | ----------------------------------------------------------------------------------------------------- |
| `/control/current_gate_mode` | `tier4_control_msgs::msg::GateMode` | This topic is required for recording bag files when `enable_only_auto_mode_recording` is set to true. |
| Any subscribed topic         | Any data type                       | Subscribe to any target topic that you want to save in a bag file.                                    |

### Output

The `autoware_bag_recorder` package does not produce any output topics.

## Parameters

### Core Parameters

Here are the core parameters that can be configured for the `autoware_bag_recorder` package:

| Name                               | Type   | Default Value        | Description                                                           |
| ---------------------------------- | ------ | -------------------- | --------------------------------------------------------------------- |
| `database_storage`                 | string | `"sqlite3"`          | Choose between `"sqlite3"` or `"mcap"` for database storage options.  |
| `maximum_record_time`              | int    | 36000                | Maximum duration (in seconds) for recording bag files.                |
| `maximum_allowed_bag_storage_size` | int    | 500.0                | Maximum allowed size (in GB) for storing bag files.                   |
| `maximum_bag_file_size`            | float  | 20.0                 | Maximum size (in GB) for each individual bag file.                    |
| `enable_only_auto_mode_recording`  | bool   | false                | If enabled, recording occurs only when the vehicle is in "AUTO" mode. |
| `number_of_maximum_bags`           | int    | 1000                 | Limit the number of stored bag files.                                 |
| `path`                             | string | `"bags/"`            | Path where bag files will be saved.                                   |
| `prefix`                           | string | `"logging_ros2bag_"` | Bag folder name prefix.                                               |
| `minimum_acceptable_disk_space`    | int    | 10                   | Minimum acceptable disk space (in GB) before taking action.           |
| `disk_space_threshold_action`      | string | `"shutdown"`         | Choose between `"remove"` or `"shutdown"` for disk space management.  |

### Topic Parameters

There are two topic section for this package: input_raw and other topics. input_raw_topics is used for the launching autoware pipeline with
logging (rosbag-replay simulation) simulator. Other topics can be any topics. If record_other value is false, then other topics will not be recorded.

| Name                              | Type           | Default Value | Description                                                    |
| --------------------------------- | -------------- | ------------- | -------------------------------------------------------------- |
| `record_other` `record_raw_input` | bool           | false         | If it is false, then this section topics will not be recorded. |
| `other_topics` `raw_input_topics` | vector(string) | any topics    | Topic names will be recorded.                                  |

## Assumptions / Known Limits

## References/External Links
