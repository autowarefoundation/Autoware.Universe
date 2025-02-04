# Safety Check Utils

Safety check function checks if the given path will collide with a given target object.

## Purpose / Role

In the behavior path planner, certain modules (e.g., lane change) need to perform collision checks to ensure the safe navigation of the ego vehicle. These utility functions assist the user in conducting safety checks with other road participants.

### Assumptions

The safety check module is based on the following assumptions:

1. Users must provide the position, velocity, and shape of both the ego and target objects to the utility functions.
2. The yaw angle of each point in the predicted path of both the ego and target objects should point to the next point in the path.
3. The safety check module uses RSS distance to determine the safety of a potential collision with other objects.

### Limitations

Currently the yaw angle of each point of predicted paths of a target object does not point to the next point. Therefore, the safety check function might returns incorrect result for some edge case.

### Inner working / Algorithm

The flow of the safety check algorithm is described in the following explanations.

![safety_check_flow](../images/path_safety_checker/safety_check_flow.drawio.svg)

Here we explain each step of the algorithm flow.

#### 1. Get pose of the target object at a given time

For the first step, we obtain the pose of the target object at a given time. This can be done by interpolating the predicted path of the object.

#### 2. Check overlap

With the interpolated pose obtained in the step.1, we check if the object and ego vehicle overlaps at a given time. If they are overlapped each other, the given path is unsafe.

#### 3. Get front object

After the overlap check, it starts to perform the safety check for the broader range. In this step, it judges if ego or target object is in front of the other vehicle. We use arc length of the front point of each object along the given path to judge which one is in front of the other. In the following example, target object (red rectangle) is running in front of the ego vehicle (black rectangle).

![front_object](../images/path_safety_checker/front_object.drawio.svg)

#### 4. Calculate RSS distance

After determining which vehicle is ahead, the RSS (Responsibility-Sensitive Safety) distance is computed as follows:

$$
d_{\text{rss}} = v_{\text{rear}} (t_{\text{reaction}} + t_{\text{margin}}) + \frac{v_{\text{rear}}^2}{2|a_{\text{rear, decel}}|} - \frac{v_{\text{front}}^2}{2|a_{\text{front, decel}}|}
$$

where:

- $v_{\text{front}}$: Velocity of the front vehicle.
- $v_{\text{rear}}$: Velocity of the rear vehicle.
- $a_{\text{front, decel}}$: Expected deceleration of the front vehicle.
- $a_{\text{rear, decel}}$: Expected deceleration of the rear vehicle.
- $t_{\text{reaction}}$: Reaction time of the rear vehicle driver.
- $t_{\text{margin}}$: Additional safety time margin, which accounts for uncertainties in vehicle behavior or delays in driver reactions during braking.

!!! note

    RSS distance is normally used for objects traveling in the same direction, if the yaw difference between a given ego pose and object pose is more than a user-defined yaw difference threshold, the rss collision check will be skipped for that specific pair of poses.

#### 5. Create extended ego and target object polygons

In this step, we compute extended ego and target object polygons. The extended polygons can be described as:

![extended_polygons](../images/path_safety_checker/extended_polygons.drawio.svg)

As the picture shows, we expand the rear object polygon. For the longitudinal side, we extend it with the RSS distance, and for the lateral side, we extend it by the lateral margin.

#### 6. Check overlap

Similar to the previous step, we check the overlap of the extended rear object polygon and front object polygon. If they are overlapped each other, we regard it as the unsafe situation.

## Parameter configuration

The following parameters are related to the safety checks:

| Name                                  | Unit   | Type   | Description                                                                                                                                       |
| :------------------------------------ | ------ | ------ | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| `expected_front_deceleration`         | [m/s²] | double | The maximum deceleration of the front object during sudden braking.                                                                               |
| `expected_rear_deceleration`          | [m/s²] | double | The maximum deceleration of the rear object during sudden braking.                                                                                |
| `rear_vehicle_reaction_time`          | [s]    | double | The reaction time of the rear vehicle driver, starting from noticing the sudden braking of the front vehicle until stepping on the brake.         |
| `rear_vehicle_safety_time_margin`     | [s]    | double | The time buffer for the rear vehicle to come to a complete stop during sudden braking.                                                            |
| `lateral_distance_max_threshold`      | [m]    | double | The lateral distance threshold used to determine whether the lateral distance between two objects is sufficient for a safe lane change.           |
| `longitudinal_distance_min_threshold` | [m]    | double | The longitudinal distance threshold used to determine whether the longitudinal distance between two objects is sufficient for a safe lane change. |
| `longitudinal_velocity_delta_time`    | [s]    | double | A time multiplier used to compute the actual gap between vehicles at each predicted point (not RSS distance).                                     |
| `extended_polygon_policy`             | [-]    | string | Policy used to determine the polygon shape for the safety check. Available options are: `rectangle` or `along-path`.                              |

To provide flexibility, these parameters can vary by module. Each module can define its own set of safety check parameters to accommodate different scenarios or requirements.

!!! note

    Based on [RSS distance equation](#4-calculate-rss-distance), the $t_{\text{reaction}}$ and $t_{\text{margin}}$ are added together, so increasing either of them will make the RSS polygon longer. However, users should tune these parameters according to the RSS principles as closely as possible. Specifically:

    - $t_{\text{reaction}}$ should reflect the driver’s reaction time.
    - $t_{\text{margin}}$ should account for uncertainties, such as varying road conditions or unexpected reaction delays.

### Extended Polygon Policy

The Extended Polygon Policy defines the shape of the extended polygon. It can be configured using the parameter `extended_polygon_policy`. The value can be set to either `rectangle` or `along-path`.

- `rectangle`: A rectangular polygon is generated around the ego vehicle predicted pose. This option is simpler and computationally efficient, making it suitable for most scenarios.
- `along-path`: A polygon is generated by extending along the planned trajectory of the ego vehicle. This option provides a more accurate representation of the ego vehicle's path, especially on curved path like lane change.

<div align="center">
  <table>
    <tr>
      <td>
        <div style="text-align: center;">
        <div style="color: black; font-size: 20px; margin-bottom: 10px;">rectangle</div>
        <img src="images/path_safety_checker/rectangle.png" alt="Rectangle">
        </div>
      </td>
      <td>
        <div style="text-align: center;">
        <div style="color: black; font-size: 20px; margin-bottom: 10px;">along_path</div>
        <img src="images/path_safety_checker/along_path.png" alt="Along Path">
        </div>
      </td>
    </tr>
  </table>
</div>

!!! warning

    The extended polygon policy is applied only to the ego vehicle’s polygon when an object is in front of the ego vehicle. For objects behind the ego vehicle, this policy is not applied.
