# Autonomous Emergency Braking (AEB)

## Purpose / Role

`autonomous_emergency_braking` is a module that prevents collisions with obstacles on the predicted path created by a control module or sensor values estimated from the control module.

### Assumptions

This module has following assumptions.

- The predicted path of the ego vehicle can be made from either the path created from sensors or the path created from a control module, or both.

- The current speed and angular velocity can be obtained from the sensors of the ego vehicle, and it uses points as obstacles.

- The AEBs target obstacles are 2D points that can be obtained from the input point cloud or by obtaining the intersection points between the predicted ego footprint path and a predicted object's shape.

### IMU path generation: steering angle vs IMU's angular velocity

Currently, the IMU-based path is generated using the angular velocity obtained by the IMU itself. It has been suggested that the steering angle could be used instead onf the angular velocity.

The pros and cons of both approaches are:

IMU angular velocity:

- (+) Usually, it has high accuracy
- (-) Vehicle vibration might introduce noise.

Steering angle:

- (+) Not so noisy
- (-) May have a steering offset or a wrong gear ratio, and the steering angle of Autoware and the real steering may not be the same.

For the moment, there are no plans to implement the steering angle on the path creation process of the AEB module.

## Inner-workings / Algorithms

AEB has the following steps before it outputs the emergency stop signal.

1. Activate AEB if necessary.

2. Generate a predicted path of the ego vehicle.

3. Get target obstacles from the input point cloud and/or predicted object data.

4. Estimate the closest obstacle speed.

5. Collision check with target obstacles.

6. Send emergency stop signals to `/diagnostics`.

We give more details of each section below.

### 1. Activate AEB if necessary

We do not activate AEB module if it satisfies the following conditions.

- Ego vehicle is not in autonomous driving state

- When the ego vehicle is not moving (Current Velocity is below a 0.1 m/s threshold)

### 2. Generate a predicted path of the ego vehicle

#### 2.1 Overview of IMU Path Generation

AEB generates a predicted footprint path based on current velocity and current angular velocity obtained from attached sensors. Note that if `use_imu_path` is `false`, it skips this step. This predicted path is generated as:

$$
x_{k+1} = x_k + v cos(\theta_k) dt \\
y_{k+1} = y_k + v sin(\theta_k) dt \\
\theta_{k+1} = \theta_k + \omega dt
$$

where $v$ and $\omega$ are current longitudinal velocity and angular velocity respectively. $dt$ is time interval that users can define in advance with the `imu_prediction_time_interval` parameter. The IMU path is generated considering a time horizon, defined by the `imu_prediction_time_horizon` parameter.

#### 2.2 Constraints and Countermeasures in IMU Path Generation

Since the IMU path generation only uses the ego vehicle's current angular velocity, disregarding the MPC's planner steering, the shape of the IMU path tends to get distorted quite easily and protrude out of the ego vehicle's current lane, possibly causing unwanted emergency stops. There are two countermeasures for this issue:

1. Control using the `max_generated_imu_path_length` parameter

   - Generation stops when path length exceeds the set value
   - Avoid using a large `imu_prediction_time_horizon`

2. Control based on lateral deviation
   - Set the `limit_imu_path_lat_dev` parameter to "true"
   - Set deviation threshold using `imu_path_lat_dev_threshold`
   - Path generation stops when lateral deviation exceeds the threshold

#### 2.3 Advantages and Limitations of Lateral Deviation Control

The advantage of setting a lateral deviation limit with the `limit_imu_path_lat_dev` parameter is that the `imu_prediction_time_horizon` and the `max_generated_imu_path_length` can be increased without worries about the IMU predicted path deforming beyond a certain threshold. The downside is that the IMU path will be cut short when the ego has a high angular velocity, in said cases, the AEB module would mostly rely on the MPC path to prevent or mitigate collisions.

If it is assumed the ego vehicle will mostly travel along the centerline of its lanelets, it can be useful to set the lateral deviation threshold parameter `imu_path_lat_dev_threshold` to be equal to or smaller than the average lanelet width divided by 2, that way, the chance of the IMU predicted path leaving the current ego lanelet is smaller, and it is possible to increase the `imu_prediction_time_horizon` to prevent frontal collisions when the ego is mostly traveling in a straight line.

The lateral deviation is measured using the ego vehicle's current position as a reference, and it measures the distance of the furthermost vertex of the predicted ego footprint to the predicted path. The following image illustrates how the lateral deviation of a given ego pose is measured.

![measuring_lat_dev](./image/measuring-lat-dev-on-imu-path.drawio.svg)

#### 2.4 IMU Path Generation Algorithm

##### 2.4.1 Selection of Lateral Deviation Check Points

Select vehicle vertices for lateral deviation checks based on the following conditions:

- Forward motion ($v > 0$)
  - Right turn ($\omega > 0$): Right front vertex
  - Left turn ($\omega < 0$): Left front vertex
- Reverse motion ($v < 0$)
  - Right turn ($\omega > 0$): Right rear vertex
  - Left turn ($\omega < 0$): Left rear vertex
- Straight motion ($\omega = 0$): Check both front/rear vertices depending on forward/reverse motion

##### 2.4.2 Path Generation Process

Execute the following steps at each time step:

1. State Update

   - Calculate next position $(x_{k+1}, y_{k+1})$ and yaw angle $\theta_{k+1}$ based on current velocity $v$ and angular velocity $\omega$
   - Time interval $dt$ is based on the `imu_prediction_time_interval` parameter

2. Vehicle Footprint Generation

   - Place vehicle footprint at calculated position
   - Calculate check point coordinates

3. Lateral Deviation Calculation

   - Calculate lateral deviation from selected vertex to path
   - Update path length and elapsed time

4. Evaluation of Termination Conditions

##### 2.4.3 Termination Conditions

Path generation terminates when any of the following conditions are met:

1. Basic Termination Conditions (both must be satisfied)

   - Predicted time exceeds `imu_prediction_time_horizon`
   - AND path length exceeds `min_generated_imu_path_length`

2. Path Length Termination Condition

   - Path length exceeds `max_generated_imu_path_length`

3. Lateral Deviation Termination Condition (when `limit_imu_path_lat_dev = true`)
   - Lateral deviation of selected vertex exceeds `imu_path_lat_dev_threshold`

#### MPC path generation

If the `use_predicted_trajectory` parameter is set to true, the AEB module will directly use the predicted path from the MPC as a base to generate a footprint path. It will copy the ego poses generated by the MPC until a given time horizon. The `mpc_prediction_time_horizon` parameter dictates how far ahead in the future the MPC path will predict the ego vehicle's movement. Both the IMU footprint path and the MPC footprint path can be used at the same time.

### 3. Get target obstacles

After generating the ego footprint path(s), the target obstacles are identified. There are two methods to find target obstacles: using the input point cloud, or using the predicted object information coming from perception modules.

#### Pointcloud obstacle filtering

The AEB module can filter the input pointcloud to find target obstacles with which the ego vehicle might collide. This method can be enable if the `use_pointcloud_data` parameter is set to true. The pointcloud obstacle filtering has three major steps, which are rough filtering, noise filtering with clustering and rigorous filtering.

##### Rough filtering

In rough filtering step, we select target obstacle with simple filter. Create a search area up to a certain distance (default is half of the ego vehicle width plus the `path_footprint_extra_margin` parameter plus the `expand_width` parameter) away from the predicted path of the ego vehicle and ignore the point cloud that are not within it. The rough filtering step is illustrated below.

![rough_filtering](./image/obstacle_filtering_1.drawio.svg)

##### Noise filtering with clustering and convex hulls

To prevent the AEB from considering noisy points, euclidean clustering is performed on the filtered point cloud. The points in the point cloud that are not close enough to other points to form a cluster are discarded. Furthermore, each point in a cluster is compared against the `cluster_minimum_height` parameter, if no point inside a cluster has a height/z value greater than `cluster_minimum_height`, the whole cluster of points is discarded. The parameters `cluster_tolerance`, `minimum_cluster_size` and `maximum_cluster_size` can be used to tune the clustering and the size of objects to be ignored, for more information about the clustering method used by the AEB module, please check the official documentation on euclidean clustering of the PCL library: <https://pcl.readthedocs.io/projects/tutorials/en/master/cluster_extraction.html>.

Furthermore, a 2D convex hull is created around each detected cluster, the vertices of each hull represent the most extreme/outside points of the cluster. These vertices are then checked in the next step.

##### Rigorous filtering

After Noise filtering, the module performs a geometric collision check to determine whether the filtered obstacles/hull vertices actually have possibility to collide with the ego vehicle. In this check, the ego vehicle is represented as a rectangle, and the point cloud obstacles are represented as points. Only the vertices with a possibility of collision are labeled as target obstacles.

![rigorous_filtering](./image/obstacle_filtering_2.drawio.svg)

##### Obstacle labeling

After rigorous filtering, the remaining obstacles are labeled. An obstacle is given a "target" label for collision checking only if it falls within the ego vehicle's defined footprint (made using the ego vehicle's width and the `expand_width` parameter). For an emergency stop to occur, at least one obstacle needs to be labeled as a target.

![labeling](./image/labeling.drawio.svg)

#### Using predicted objects to get target obstacles

If the `use_predicted_object_data` parameter is set to true, the AEB can use predicted object data coming from the perception modules, to get target obstacle points. This is done by obtaining the 2D intersection points between the ego's predicted footprint path (made using the ego vehicle's width and the `expand_width` parameter) and each of the predicted objects enveloping polygon or bounding box. if there is no intersection, all points are discarded.

![predicted_object_and_path_intersection](./image/using-predicted-objects.drawio.svg)

### Finding the closest target obstacle

After identifying all possible obstacles using pointcloud data and/or predicted object data, the AEB module selects the closest point to the ego vehicle as the candidate for collision checking. The "closest object" is defined as an obstacle within the ego vehicle's footprint, determined by its width and the `expand_width` parameter, that is closest to the ego vehicle along the longitudinal axis, using the IMU or MPC path as a reference. Target obstacles are prioritized over those outside the ego path, even if the latter are longitudinally closer. This prioritization ensures that the collision check focuses on objects that pose the highest risk based on the vehicle's trajectory.

If no target obstacles are found, the AEB module considers other nearby obstacles outside the path. In such cases, it skips the collision check but records the position of the closest obstacle to calculate its speed (Step #4). Note that, obstacles obtained with predicted object data are all target obstacles since they are within the ego footprint path and it is not necessary to calculate their speed (it is already calculated by the perception module). Such obstacles are excluded from step #4.

![closest_object](./image/closest-point.drawio.svg)

### 4. Obstacle velocity estimation

To begin calculating the target point's velocity, the point must enter the speed calculation area,
which is defined by the `speed_calculation_expansion_margin` parameter plus the ego vehicles width and the `expand_width` parameter.
Depending on the operational environment,
this margin can reduce unnecessary autonomous emergency braking
caused by velocity miscalculations during the initial calculation steps.

![speed_calculation_expansion](./image/speed_calculation_expansion.drawio.svg)

Once the position of the closest obstacle/point is determined, the AEB modules uses the history of previously detected objects to estimate the closest object relative speed using the following equations:

$$
d_{t} = t_{1} - t_{0}
$$

$$
d_{x} = norm(o_{x} - prev_{x})
$$

$$
v_{norm} = d_{x} / d_{t}
$$

Where $t_{1}$ and $t_{0}$ are the timestamps of the point clouds used to detect the current closest object and the closest object of the previous point cloud frame, and $o_{x}$ and $prev_{x}$ are the positions of those objects, respectively.

![relative_speed](./image/object_relative_speed.drawio.svg)

Note that, when the closest obstacle/point comes from using predicted object data, $v_{norm}$ is calculated by directly computing the norm of the predicted object's velocity in the x and y axes.

The velocity vector is then compared against the ego's predicted path to get the longitudinal velocity $v_{obj}$:

$$
v_{obj} = v_{norm} * Cos(yaw_{diff}) + v_{ego}
$$

where $yaw_{diff}$ is the difference in yaw between the ego path and the displacement vector $$v_{pos} = o_{pos} - prev_{pos} $$ and $v_{ego}$ is the ego's current speed, which accounts for the movement of points caused by the ego moving and not the object. All these equations are performed disregarding the z axis (in 2D).

Note that the object velocity is calculated against the ego's current movement direction. If the object moves in the opposite direction to the ego's movement, the object velocity will be negative, which will reduce the rss distance on the next step.

The resulting estimated object speed is added to a queue of speeds with timestamps. The AEB then checks for expiration of past speed estimations and eliminates expired speed measurements from the queue, the object expiration is determined by checking if the time elapsed since the speed was first added to the queue is larger than the parameter `previous_obstacle_keep_time`. Finally, the median speed of the queue is calculated. The median speed will be used to calculate the RSS distance used for collision checking.

### 5. Collision check with target obstacles using RSS distance

In the fifth step, the AEB module checks for collision with the closest target obstacle using RSS distance.
Only the closest target object is evaluated because RSS distance is used to determine collision risk. If the nearest target point is deemed safe, all other potential obstacles within the path are also assumed to be safe.

RSS distance is formulated as:

$$
d = v_{ego}*t_{response} + v_{ego}^2/(2*a_{min}) -(sign(v_{obj})) * v_{obj}^2/(2*a_{obj_{min}}) + offset
$$

where $v_{ego}$ and $v_{obj}$ is current ego and obstacle velocity, $a_{min}$ and $a_{obj_{min}}$ is ego and object minimum acceleration (maximum deceleration), $t_{response}$ is response time of the ego vehicle to start deceleration. Therefore the distance from the ego vehicle to the obstacle is smaller than this RSS distance $d$, the ego vehicle send emergency stop signals.

Only obstacles classified as "targets" (as defined in Step #3) are considered for RSS distance calculations. Among these "target" obstacles, the one closest to the ego vehicle is used for the calculation. If no "target" obstacles are present—meaning no obstacles fall within the ego vehicle's predicted path (determined by its width and an expanded margin)—this step is skipped. Instead, the position of the closest obstacle is recorded for future speed calculations (Step #4). In this scenario, no emergency stop diagnostic message is generated. The process is illustrated in the accompanying diagram.

![rss_check](./image/rss_check.drawio.svg)

### 6. Send emergency stop signals to `/diagnostics`

If AEB detects collision with point cloud obstacles in the previous step, it sends emergency signal to `/diagnostics` in this step. Note that in order to enable emergency stop, it has to send ERROR level emergency. Moreover, AEB user should modify the setting file to keep the emergency level, otherwise Autoware does not hold the emergency state.

## Use cases

### Front vehicle suddenly brakes

The AEB can activate when a vehicle in front suddenly brakes, and a collision is detected by the AEB module. Provided the distance between the ego vehicle and the front vehicle is large enough and the ego’s emergency acceleration value is high enough, it is possible to avoid or soften collisions with vehicles in front that suddenly brake. NOTE: the acceleration used by the AEB to calculate rss_distance is NOT necessarily the acceleration used by the ego while doing an emergency brake. The acceleration used by the real vehicle can be tuned by changing the [mrm_emergency stop jerk and acceleration values](https://github.com/tier4/autoware_launch/blob/d1b2688f2788acab95bb9995d72efd7182e9006a/autoware_launch/config/system/mrm_emergency_stop_operator/mrm_emergency_stop_operator.param.yaml#L4).

![front vehicle collision prevention](./image/front_vehicle_collision.drawio.svg)

### Stop for objects that appear suddenly

When an object appears suddenly, the AEB can act as a fail-safe to stop the ego vehicle when other modules fail to detect the object on time. If sudden object cut ins are expected, it might be useful for the AEB module to detect collisions of objects BEFORE they enter the real ego vehicle path by increasing the `expand_width` parameter.

![occluded object collision prevention](./image/occluded_space.drawio.svg)

### Preventing Collisions with rear objects

The AEB module can also prevent collisions when the ego vehicle is moving backwards.

![backward driving](./image/backward-driving.drawio.svg)

### Preventing collisions in case of wrong Odometry (IMU path only)

When vehicle odometry information is faulty, it is possible that the MPC fails to predict a correct path for the ego vehicle. If the MPC predicted path is wrong, collision avoidance will not work as intended on the planning modules. However, the AEB’s IMU path does not depend on the MPC and could be able to predict a collision when the other modules cannot. As an example you can see a figure of a hypothetical case in which the MPC path is wrong and only the AEB’s IMU path detects a collision.

![wrong mpc](./image/wrong-mpc.drawio.svg)

## Parameters

{{ json_to_markdown("control/autoware_autonomous_emergency_braking/schema/autonomous_emergency_braking.schema.json") }}

## Limitations

- The distance required to stop after collision detection depends on the ego vehicle's speed and deceleration performance. To avoid collisions, it's necessary to increase the detection distance and set a higher deceleration rate. However, this creates a trade-off as it may also increase the number of unnecessary activations. Therefore, it's essential to consider what role this module should play and adjust the parameters accordingly.

- AEB might not be able to react with obstacles that are close to the ground. It depends on the performance of the pre-processing methods applied to the point cloud.

- Longitudinal acceleration information obtained from sensors is not used due to the high amount of noise.

- The accuracy of the predicted path created from sensor data depends on the accuracy of sensors attached to the ego vehicle.

![aeb_range](./image/range.drawio.svg)
