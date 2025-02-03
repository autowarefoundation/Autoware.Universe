# radar_threshold_filter

## radar_threshold_filter_node

Remove noise from radar return by threshold.

- Amplitude filter: Low amplitude consider noise
- FOV filter: Pointcloud from radar's FOV edge occur perturbation
- Range filter: Too near pointcloud often occur noise

Calculation cost is O(n). `n` is the number of radar return.

### Input topics

| Name        | Type                         | Description           |
| ----------- | ---------------------------- | --------------------- |
| input/radar | radar_msgs/msg/RadarScan.msg | Radar pointcloud data |

### Output topics

| Name         | Type                         | Description               |
| ------------ | ---------------------------- | ------------------------- |
| output/radar | radar_msgs/msg/RadarScan.msg | Filtered radar pointcloud |

### Parameters

{{ json_to_markdown("/sensing/autoware_radar_threshold_filter/schema/radar_threshold_filter.schema.json") }} |

### How to launch

```sh
ros2 launch autoware_radar_threshold_filter radar_threshold_filter.launch.xml
```
