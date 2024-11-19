^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_radar_crossing_objects_noise_filter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(radar_tracks_msgs_converter, simple_object_merger, radar_tracks_noise_filter)!: add package name prefix of autoware\_ (`#8173 <https://github.com/autowarefoundation/autoware.universe/issues/8173>`_)
  * refactor: rename radar_tracks_msgs_converter package to autoware_radar_tracks_msgs_converter
  * refactor: rename simple_object_merger package to autoware_simple_object_merger
  * refactor: rename sensing/radar_tracks_noise_filter to sensing/autoware_radar_tracks_noise_filter
  ---------
* refactor(radar)!: add package name prefix of autoware\_ (`#7892 <https://github.com/autowarefoundation/autoware.universe/issues/7892>`_)
  * refactor: rename radar_object_tracker
  * refactor: rename package from radar_object_tracker to autoware_radar_object_tracker
  * refactor: rename package from radar_object_clustering to autoware_radar_object_clustering
  * refactor: rename package from radar_fusion_to_detected_object to autoware_radar_fusion_to_detected_object
  * refactor: rename radar_crossing_objects_noise_filter to autoware_radar_crossing_objects_noise_filter
  * refactor: rename object_velocity_splitter to autoware_object_velocity_splitter
  * refactor: rename object_range_splitter to autoware_object_range_splitter
  * refactor: update readme
  ---------
* Contributors: Taekjin LEE, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
