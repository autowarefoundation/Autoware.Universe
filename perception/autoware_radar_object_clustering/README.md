# `autoware_radar_object_clustering`

This package contains a radar object clustering for [autoware_perception_msgs/msg/DetectedObject](https://github.com/autowarefoundation/autoware_msgs/tree/main/autoware_perception_msgs/msg/DetectedObject.idl) input.

This package can make clustered objects from radar DetectedObjects, the objects which is converted from RadarTracks by [radar_tracks_msgs_converter](https://github.com/autowarefoundation/autoware.universe/tree/main/perception/autoware_radar_tracks_msgs_converter) and is processed by noise filter.
In other word, this package can combine multiple radar detections from one object into one and adjust class and size.

![radar_clustering](docs/radar_clustering.drawio.svg)

## Design

### Background

In radars with object output, there are cases that multiple detection results are obtained from one object, especially for large vehicles such as trucks and trailers.
Its multiple detection results cause separation of objects in tracking module.
Therefore, by this package the multiple detection results are clustered into one object in advance.

### Algorithm

#### 1. Sort by distance from `base_link`

At first, to prevent changing the result from depending on the order of objects in DetectedObjects, input objects are sorted by distance from `base_link`.
In addition, to apply matching in closeness order considering occlusion, objects are sorted in order of short distance in advance.

#### 2. Clustering

If two radar objects are near, and yaw angle direction and velocity between two radar objects is similar (the degree of these is defined by parameters), then these are clustered.
Note that radar characteristic affect parameters for this matching.
For example, if resolution of range distance or angle is low and accuracy of velocity is high, then `distance_threshold` parameter should be bigger and should set matching that strongly looks at velocity similarity.

![clustering](docs/clustering.drawio.svg)

After grouping for all radar objects, if multiple radar objects are grouping, the kinematics of the new clustered object is calculated from average of that and label and shape of the new clustered object is calculated from top confidence in radar objects.

#### 3. Fixed label correction

When the label information from radar outputs lack accuracy, `is_fixed_label` parameter is recommended to set `true`.
If the parameter is true, the label of a clustered object is overwritten by the label set by `fixed_label` parameter.
If this package use for faraway dynamic object detection with radar, the parameter is recommended to set to `VEHICLE`.

#### 4. Fixed size correction

When the size information from radar outputs lack accuracy, `is_fixed_size` parameter is recommended to set `true`.
If the parameter is true, the size of a clustered object is overwritten by the label set by `size_x`, `size_y`, and `size_z` parameters.
If this package use for faraway dynamic object detection with radar, the parameter is recommended to set to
`size_x`, `size_y`, `size_z`, as average of vehicle size.
Note that to use for [multi_objects_tracker](https://github.com/autowarefoundation/autoware.universe/tree/main/perception/autoware_multi_object_tracker), the size parameters need to exceed `min_area_matrix` parameters of it.

### Limitation

For now, size estimation for clustered object is not implemented.
So `is_fixed_size` parameter is recommended to set `true`, and size parameters is recommended to set to value near to average size of vehicles.

## Interface

### Input

- `~/input/objects` (`autoware_perception_msgs/msg/DetectedObjects.msg`)
  - Radar objects

### Output

- `~/output/objects` (`autoware_perception_msgs/msg/DetectedObjects.msg`)
  - Output objects

### Parameter

{{ json_to_markdown("perception/autoware_radar_object_clustering/config/radar_object_clustering.param.yaml/schema/radar_object_clustering.schema.json") }}
