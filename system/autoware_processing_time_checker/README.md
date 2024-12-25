# Processing Time Checker

## Purpose

This node checks whether the processing time of each module is valid or not, and send a diagnostic.
NOTE: Currently, there is no validation feature, and "OK" is always assigned in the diagnostic.

### Standalone Startup

```bash
ros2 launch autoware_processing_time_checker processing_time_checker.launch.xml
```

## Inner-workings / Algorithms

## Inputs / Outputs

### Input

| Name                      | Type                              | Description                    |
| ------------------------- | --------------------------------- | ------------------------------ |
| `/.../processing_time_ms` | `tier4_debug_msgs/Float64Stamped` | processing time of each module |

### Output

| Name                                      | Type                                  | Description                        |
| ----------------------------------------- | ------------------------------------- | ---------------------------------- |
| `/system/processing_time_checker/metrics` | `tier4_metric_msgs::msg::MetricArray` | processing time of all the modules |

## Parameters

{{ json_to_markdown("system/autoware_processing_time_checker/schema/processing_time_checker.schema.json") }}

If `output_metrics = true`, the node writes the statics of the processing_time measured during its lifetime to `<ros2_logging_directory>/autoware_metrics/<node_name>-<time_stamp>.json` when shut down.

## Assumptions / Known limits

TBD.
