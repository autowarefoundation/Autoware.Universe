# scenario_simulator_v2 Adapter

## Purpose

This package provides a node to convert various messages from the Autoware into `tier4_simulation_msgs::msg::UserDefinedValue` messages for the scenario_simulator_v2.
Currently, this node supports conversion of:

- `tier4_metric_msgs::msg::MetricArray` for metric topics

## Inner-workings / Algorithms

- For `tier4_metric_msgs::msg::MetricArray`,
  The node subscribes to all topics listed in the parameter `metric_topic_list`.
  Each time such message is received, it is converted into as many `UserDefinedValue` messages as the number of `Metric` objects.
  The format of the output topic is detailed in the _output_ section.

## Inputs / Outputs

### Inputs

The node listens to `MetricArray` messages on the topics specified in `metric_topic_list`.

### Outputs

The node outputs `UserDefinedValue` messages that are converted from the received messages.

- For `MetricArray` messages,
  The name of the output topics are generated from the corresponding input topic, the name of the metric.
  - For example, we might listen to topic `/planning/planning_evaluator/metrics` and receive a `MetricArray` with 2 metrics:
    - metric with `name: "metricA/x"`
    - metric with `name: "metricA/y"`
  - The resulting topics to publish the `UserDefinedValue` are as follows:
    - `/planning/planning_evaluator/metrics/metricA/x`
    - `/planning/planning_evaluator/metrics/metricA/y`

## Parameters

{{ json_to_markdown("src/autoware/universe/evaluator/scenario_simulator_v2_adapter/schema/scenario_simulator_v2_adapter.schema.json") }}

## Assumptions / Known limits

Values in the `Metric` objects of a `MetricArray` are assumed to be of type `double`.

## Future extensions / Unimplemented parts
