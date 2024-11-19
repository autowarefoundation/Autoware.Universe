^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_lidar_marker_localizer
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
* refactor(localization_util)!: prefix package and namespace with autoware (`#8922 <https://github.com/autowarefoundation/autoware.universe/issues/8922>`_)
  add autoware prefix to localization_util
* feat(localization): add `lidar_marker_localizer` (`#5573 <https://github.com/autowarefoundation/autoware.universe/issues/5573>`_)
  * Added lidar_marker_localizer
  * style(pre-commit): autofix
  * fix launch file
  * style(pre-commit): autofix
  * Removed subscriber_member_function.cpp
  * Renamed the package and the node
  * style(pre-commit): autofix
  * Removed pose_array_interpolator
  * Removed unused files
  * Removed include dir
  * style(pre-commit): autofix
  * Renamed wrong names
  * fix magic number
  * style(pre-commit): autofix
  * fix bug
  * parameterized
  * style(pre-commit): autofix
  * add base_covariance
  * style(pre-commit): autofix
  * Removed std::cerr
  * Removed unused code
  * Removed unnecessary publishers
  * Changed to use alias
  * Fixed result_base_link_on_map
  * Changed to use "using std::placeholders"
  * Refactored points_callback
  * Fixed as pointed out by linter
  * Refactored lidar_marker_localizer
  * Fixed const reference
  * Refactor point variables
  * Added detect_landmarks
  * rework filering params
  * fix marker position
  * style(pre-commit): autofix
  * fix build error
  * fix marker position
  * style(pre-commit): autofix
  * update readme
  * style(pre-commit): autofix
  * Added calculate_diff_pose
  * Fixed to pass linter
  * update package.xml
  * Fixed to use SmartPoseBuffer
  * Fixed function calculate_diff_pose to calculate_new_self_pose
  * Compatible with the latest landmark_manager
  * Fixed pub_marker
  * Fixed launch
  * Removed unused arg
  * Removed limit_distance_from_self_pose_to_marker_from_lanelet2
  * Fixed parse_landmarks
  * Fixed parameter type
  * Fixed typo
  * rework diagnostics
  * style(pre-commit): autofix
  * rotate covariance
  * style(pre-commit): autofix
  * add json schema
  * style(pre-commit): autofix
  * parameterize marker name
  * python to xml
  * update launch files
  * style(pre-commit): autofix
  * add debug/pose_with_covariance
  * style(pre-commit): autofix
  * update readme
  * update readme
  * add depend
  * add sample dataset
  * add param marker_height_from_ground
  * style(pre-commit): autofix
  * fix typo
  * add includes
  * add name to TODO comment
  * style(pre-commit): autofix
  * rename lidar-marker
  * modify sample dataset url
  * add flowchat to readme
  * fix callbackgroup
  * add TODO comment
  * fix throttle timer
  * delete unused valriable
  * delete unused line
  * style(pre-commit): autofix
  * fix the duplicated code
  * style(pre-commit): autofix
  * avoid division by zero
  * fix TODO comment
  * fix uncrustify failed
  * style(pre-commit): autofix
  * Update localization/landmark_based_localizer/lidar_marker_localizer/src/lidar_marker_localizer.cpp
  Co-authored-by: SakodaShintaro <rgbygscrsedppbwg@gmail.com>
  * change lint_common
  * update CMakeLists
  * save intensity func
  * style(pre-commit): autofix
  * style(pre-commit): autofix
  * fix build error
  * style(pre-commit): autofix
  * apply PointXYZIRC
  * add autoware prefix
  * componentize
  * move directory
  * use localization_util's diagnostics lib
  * style(pre-commit): autofix
  * applay linter
  * style(pre-commit): autofix
  * to pass spell-check
  * remove _ex
  * refactor
  * style(pre-commit): autofix
  * remove unused depend
  * update readme
  * fix typo
  * fix json
  * fix autoware prefix
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: yamato-ando <Yamato ANDO>
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
  Co-authored-by: yamato-ando <yamato.ando@tier4.jp>
* Contributors: Esteve Fernandez, Masaki Baba, SakodaShintaro, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
