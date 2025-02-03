# autoware_mtr

## Purpose

The `autoware_mtr` package is used for 3D object motion prediction based on ML-based model called MTR.

## Inner-workings / Algorithms

The implementation bases on MTR [1] work. It uses TensorRT library for data process and network interface.

## Inputs / Outputs

### Input

| Name                 | Type                                            | Description              |
| -------------------- | ----------------------------------------------- | ------------------------ |
| `~/input/objects`    | `autoware_perception_msgs::msg::TrackedObjects` | Input agent state.       |
| `~/input/vector_map` | `autoware_map_msgs::msg::LeneletMapBin`         | Input vector map.        |
| `~/input/ego`        | `sensor_msgs::msg::Odometry`                    | Input ego vehicle state. |

### Output

| Name               | Type                                              | Description                |
| ------------------ | ------------------------------------------------- | -------------------------- |
| `~/output/objects` | `autoware_perception_msgs::msg::PredictedObjects` | Predicted objects' motion. |

## Parameters

Following parameters can be specified in `.launch.xml` or command line.

### `param_path`

File path to the MTR configuration. (Default: `autoware_mtr/config/mtr.param.yaml`)

### Configuration Parameters

#### `model_params`

| Name                          |  type   | Description                                                 |
| :---------------------------- | :-----: | :---------------------------------------------------------- |
| `model_path`                  |  `str`  | ONNX or engine file path.                                   |
| `target_labels`               | `str[]` | An array of label names to be predicted.                    |
| `num_past`                    |  `int`  | The number of history length.                               |
| `num_mode`                    |  `int`  | The number of predicted modes.                              |
| `num_future`                  |  `int`  | The number of predicted future length.                      |
| `max_num_polyline`            |  `int`  | The maximum number of polylines to be contained in input.   |
| `max_num_point`               |  `int`  | The maximum number of points included in a single polyline. |
| `point_break_distance`        | `float` | Distance threshold to separate points into two polylines.   |
| `intention_point_filepath`    |  `str`  | File path to intension points (.csv).                       |
| `num_intention_point_cluster` |  `int`  | The number of clusters of intention points.                 |

#### `build_params`

| Name         |  type  | Description                                                 |
| :----------- | :----: | :---------------------------------------------------------- |
| `is_dynamic` | `bool` | Indicates whether the model allows dynamic shape inference. |
| `precision`  | `str`  | Precision mode.                                             |
| `MINMAX`     | `str`  | Calibration mode.                                           |

### `data_path`

Directory path to ONNX or TensorRT engine file. (Default: `autoware_mtr/data`)

### `build_only`

This option performs to build the TensorRT engine file from the ONNX file and exit after finishing it. (Default: `false`)

You can execute with the following command:

```bash
ros2 launch autoware_mtr mtr.launch.xml build_only:=true
```
