^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_radar_object_clustering
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.39.0 (2024-11-25)
-------------------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* refactor(radar_tracks_msgs_converter, simple_object_merger, radar_tracks_noise_filter)!: add package name prefix of autoware\_ (`#8173 <https://github.com/autowarefoundation/autoware.universe/issues/8173>`_)
  * refactor: rename radar_tracks_msgs_converter package to autoware_radar_tracks_msgs_converter
  * refactor: rename simple_object_merger package to autoware_simple_object_merger
  * refactor: rename sensing/radar_tracks_noise_filter to sensing/autoware_radar_tracks_noise_filter
  ---------
* fix(autoware_radar_object_clustering): fix funcArgNamesDifferent (`#8014 <https://github.com/autowarefoundation/autoware.universe/issues/8014>`_)
  * fix:funcArgNamesDifferent
  * fix:funcArgNamesDifferent
  ---------
* refactor(multi_object_tracker)!: add package name prefix of autoware\_ (`#8083 <https://github.com/autowarefoundation/autoware.universe/issues/8083>`_)
  * refactor: rename multi_object_tracker package to autoware_multi_object_tracker
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* Contributors: Esteve Fernandez, Taekjin LEE, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
