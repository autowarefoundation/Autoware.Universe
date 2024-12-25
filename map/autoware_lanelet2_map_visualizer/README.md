# autoware_lanelet2_map_visualizer package

This package provides the features of visualizing the lanelet2 maps.

## lanelet2_map_visualization

### Feature

lanelet2_map_visualization visualizes autoware_map_msgs/LaneletMapBin messages into visualization_msgs/MarkerArray.

### How to Run

`ros2 run autoware_lanelet2_map_visualizer lanelet2_map_visualization`

### Subscribed Topics

- ~input/lanelet2_map (autoware_map_msgs/LaneletMapBin) : binary data of Lanelet2 Map

### Published Topics

- ~output/lanelet2_map_marker (visualization_msgs/MarkerArray) : visualization messages for RViz
