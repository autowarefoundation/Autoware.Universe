^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_localization_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

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
* refactor(ndt_scan_matcher)!: prefix package and namespace with autoware (`#8904 <https://github.com/autowarefoundation/autoware.universe/issues/8904>`_)
  add autoware\_ prefix
* refactor(ekf_localizer)!: prefix package and namespace with autoware (`#8888 <https://github.com/autowarefoundation/autoware.universe/issues/8888>`_)
  * import lanelet2_map_preprocessor
  * move headers to include/autoware/efk_localier
  ---------
* refactor(pose_initializer)!: prefix package and namespace with autoware (`#8701 <https://github.com/autowarefoundation/autoware.universe/issues/8701>`_)
  * add autoware\_ prefix
  * fix link
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* chore(stop_filter): extract stop_filter.param.yaml to autoware_launch (`#8681 <https://github.com/autowarefoundation/autoware.universe/issues/8681>`_)
  Extract stop_filter.param.yaml to autoware_launch
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
* refactor(pose_instability_detector)!: prefix package and namespace with autoware (`#8568 <https://github.com/autowarefoundation/autoware.universe/issues/8568>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* refactor(pose_estimator_arbiter)!: prefix package and namespace with autoware (`#8386 <https://github.com/autowarefoundation/autoware.universe/issues/8386>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  * fix link for landmark_based_localizer
  * remove Shadowing
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* refactor(gyro_odometer)!: prefix package and namespace with autoware (`#8340 <https://github.com/autowarefoundation/autoware.universe/issues/8340>`_)
  * add autoware\_ prefix
  * add missing header
  * use target_include_directories instead
  * add autoware\_ prefix
  ---------
* refactor(localization_error_monitor)!: prefix package and namespace with autoware (`#8423 <https://github.com/autowarefoundation/autoware.universe/issues/8423>`_)
  add autoware\_ prefix
* refactor(geo_pose_projector)!: prefix package and namespace with autoware (`#8334 <https://github.com/autowarefoundation/autoware.universe/issues/8334>`_)
  * add autoware\_ prefix
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* refactor(twist2accel)!: prefix package and namespace with autoware (`#8299 <https://github.com/autowarefoundation/autoware.universe/issues/8299>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  * add autoware\_ prefix
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* refactor(pointcloud_preprocessor): prefix package and namespace with autoware (`#7983 <https://github.com/autowarefoundation/autoware.universe/issues/7983>`_)
  * refactor(pointcloud_preprocessor)!: prefix package and namespace with autoware
  * style(pre-commit): autofix
  * style(pointcloud_preprocessor): suppress line length check for macros
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * refactor(pointcloud_preprocessor): directory structure (soft)
  * refactor(pointcloud_preprocessor): directory structure (hard)
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* chore(localization, map): remove maintainer (`#7940 <https://github.com/autowarefoundation/autoware.universe/issues/7940>`_)
* refactor(stop_filter): prefix package and namespace with autoware (`#7789 <https://github.com/autowarefoundation/autoware.universe/issues/7789>`_)
  * refactor(stop_filter): prefix package and namespace with autoware
  * fix launch files and update CODEOWNERS
  ---------
* refactor(ar_tag_based_localizer): add prefix "autoware\_" to ar_tag_based_localizer (`#7483 <https://github.com/autowarefoundation/autoware.universe/issues/7483>`_)
  * Added prefix "autoware\_" to ar_tag_based_localizer
  * style(pre-commit): autofix
  * Fixed localization_launch
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_pose_covariance_modifier): add new node to early fuse gnss and ndt poses (`#6570 <https://github.com/autowarefoundation/autoware.universe/issues/6570>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Masaki Baba, SakodaShintaro, TaikiYamada4, Yutaka Kondo, kminoda, melike tanrikulu

0.26.0 (2024-04-03)
-------------------
* feat(pose_initilizer): set intial pose directly (`#6692 <https://github.com/autowarefoundation/autoware.universe/issues/6692>`_)
  * feat(pose_initilizer): set intial pose directly
  * style(pre-commit): autofix
  * fix arg order
  * minor change
  * style(pre-commit): autofix
  * remove blank lines
  * change types
  * add wait_for_service
  * style(pre-commit): autofix
  * fix default quaternion
  * rename params
  * input quaternion validation
  * fix message
  * style(pre-commit): autofix
  * add std::abs
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_localization_launch):  change the default input pointcloud of localization into the concatenated pointcloud (`#6528 <https://github.com/autowarefoundation/autoware.universe/issues/6528>`_)
  refactor lacun argument lidar_container_name to localization_pointcloud_container_name
* fix(ar_tag_based_localizer): add ar tag based localizer param (`#6390 <https://github.com/autowarefoundation/autoware.universe/issues/6390>`_)
  Added ar_tag_based_localizer_param_path
* chore(tier4_localization_launch): add maintainer (`#6350 <https://github.com/autowarefoundation/autoware.universe/issues/6350>`_)
  add maintainer
* chore(ndt scan matcher): rename config path (`#6333 <https://github.com/autowarefoundation/autoware.universe/issues/6333>`_)
  * refactor(tier4_localization_launch): use util.launch.xml instead of util.launch.py
  * style(pre-commit): autofix
  * chore(ndt_scan_matcher): rename config path
  * rename path
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_localization_launch): use util.launch.xml instead of util.launch.py (`#6287 <https://github.com/autowarefoundation/autoware.universe/issues/6287>`_)
  * refactor(tier4_localization_launch): use util.launch.xml instead of util.launch.py
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(pose_estimator_arbiter): add pose_estimator_arbiter with simple switching rule (`#6144 <https://github.com/autowarefoundation/autoware.universe/issues/6144>`_)
  * implement pose_estimator_manager pkg
  * tmp
  * swap ndt & yabloc
  * add suspension service in yabloc particle filter
  * add pluginlib for switching rule
  * implement switch rule using pluginlib
  * WIP: implement simple_switch_rule
  * implement pcd_occupancy_rule based switcher
  * resolve conflicts occured by rebase
  * sub_manager_node is not necessary
  * add ndt_yabloc_eagleye
  * fix bug
  * intuitive_multi_pose_estimator_launch
  * yabloc_pf shoulbe be activated at the first
  * merge swith_rule_plugin as inheritance
  * fix launch bug
  * add eagleye_area rule
  * implement strict switching rule
  * refine message
  * fix merge conflict
  * use hysteresis threshold for pcd occupancy criteria
  * fix merge conflict
  * add gtest
  * add component test
  * add artag submanager
  * add ar_tag_position to get ar-tag position
  * check distance to nearest ar marker
  * switch ARTAG localizer if ar marker locates around ego
  * improve ar_tag_position.[hc]pp
  * split update() from map_base_rule.cpp
  * apply pre-commit
  * add license description
  * update include guard
  * reflected all pre-commit's points
  * use magic_enum
  * add pcd_occupancy helper
  * change directory structure
  * change namespace
  * remap some topics
  * update test
  * add shared_data to share data
  * remove obsolete comments
  * share subscribed data by SharedData
  * remove obsolete comments and fix to pass test.py
  * rename SharedData
  * stream debug_msg as is
  * add README.md
  * Update README.md
  update README.md on github
  * fix eagleye bug
  * update README
  * wip
  * update README.md
  * update README
  * use landmark_manager
  * add glog & fix rule_helper bug
  * publish empty diagnostics
  * fix artag arbitorator
  * implement callback_involving_variable
  * rename invokingVariable
  * clarify log level
  * update diagnostics
  * adope new landmark_manager
  * rename manager  arbiter
  * style(pre-commit): autofix
  * fix obsolete change
  * change yabloc relayed input topic
  * resolve merge conflict
  * adopt ar_tag_position for new ar tag map specification
  * rename sub_arbitr to stopper
  * apply pre-commit
  * add timeout for async parameter client
  * style(pre-commit): autofix
  * fix typo
  * refactor shared_data
  * rename yabloc_suspend_service
  * improve debug log
  * fix integration test
  * style(pre-commit): autofix
  * remove obsolete notation
  * fix ar_tag_based_localizer.launch.xml
  * again fix ar_tag_based_localizer.launch.xml
  * style(pre-commit): autofix
  * add sample data url
  * (review reflect) refactor launch
  * (review reflect) refactor launch about gnss_enabled
  * (review reflect) organize type alias accessibility
  * (review reflect) rename PoseEstimatorName to PoseEstimatorType
  * (review reflect) fix typo
  * style(pre-commit): autofix
  * fix pedantic warning of PCL
  * (review reflect) improve diag & suppress warning
  * (review reflect) create sub only when the corresponding estimator is running
  * rename eagleye_area to pose_estimator_area
  * vectormap based rule works well
  * move old rules to example_rule/
  * update README
  * improve some features
  * style(pre-commit): autofix
  * style(pre-commit): autofix
  * move some rules into example_rule & add new simple rule
  * apply pre-commit & update README
  * split CMake for example_rule
  * remove ar_tag_position & simplify example switching rule
  * add vector_map_based_rule test
  * add pcd_map_based_rule test
  * improve README
  * fix integration test.py
  * add test
  * refactor & update README
  * replace obsolete video
  * fix typo
  * Update README.md
  fix markdown (add one line just after <summary>)
  * use structures bindings
  * add many comments
  * remove obsolete include & alias
  * fix miss of eagleye output relay
  * fix 404 URL
  * remove obsolete args
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(twist2accel): rework parameters (`#6266 <https://github.com/autowarefoundation/autoware.universe/issues/6266>`_)
  * Added twist2accel.param.yaml
  * Added twist2accel.schema.json
  * Fixed README.md and description
  * style(pre-commit): autofix
  * Removed default parameters
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: always separate lidar preprocessing from pointcloud_container (`#6091 <https://github.com/autowarefoundation/autoware.universe/issues/6091>`_)
  * feat!: replace use_pointcloud_container
  * feat: remove from planning
  * fix: fix to remove all use_pointcloud_container
  * revert: revert change in planning.launch
  * revert: revert rename of use_pointcloud_container
  * fix: fix tier4_perception_launch to enable use_pointcloud_contaienr
  * fix: fix unnecessary change
  * fix: fix unnecessary change
  * refactor: remove trailing whitespace
  * revert other changes in perception
  * revert change in readme
  * feat: move glog to pointcloud_container.launch.py
  * revert: revert glog porting
  * style(pre-commit): autofix
  * fix: fix pre-commit
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add localization & mapping maintainers (`#6085 <https://github.com/autowarefoundation/autoware.universe/issues/6085>`_)
  * Added lm maintainers
  * Add more
  * Fixed maintainer
  ---------
* refactor(ndt_scan_matcher): fixed ndt_scan_matcher.launch.xml (`#6041 <https://github.com/autowarefoundation/autoware.universe/issues/6041>`_)
  Fixed ndt_scan_matcher.launch.xml
* refactor(ar_tag_based_localizer): refactor pub/sub and so on (`#5854 <https://github.com/autowarefoundation/autoware.universe/issues/5854>`_)
  * Fixed ar_tag_based_localizer pub/sub
  * Remove dependency on image_transport
  ---------
* refactor(localization_launch, ground_segmentation_launch): rename lidar topic (`#5781 <https://github.com/autowarefoundation/autoware.universe/issues/5781>`_)
  rename lidar topic
  Co-authored-by: yamato-ando <Yamato ANDO>
* feat(localization): add `pose_instability_detector` (`#5439 <https://github.com/autowarefoundation/autoware.universe/issues/5439>`_)
  * Added pose_instability_detector
  * Renamed files
  * Fixed parameter name
  * Fixed to launch
  * Fixed to run normally
  * Fixed to publish diagnostics
  * Fixed a variable name
  * Fixed Copyright
  * Added test
  * Added maintainer
  * Added maintainer
  * Removed log output
  * Modified test
  * Fixed comment
  * Added a test case
  * Added set_first_odometry\_
  * Refactored test
  * Fixed test
  * Fixed topic name
  * Fixed position
  * Added twist message2
  * Fixed launch
  * Updated README.md
  * style(pre-commit): autofix
  * Fixed as pointed out by clang-tidy
  * Renamed parameters
  * Fixed timer
  * Fixed README.md
  * Added debug publishers
  * Fixed parameters
  * style(pre-commit): autofix
  * Fixed tests
  * Changed the type of ekf_to_odom and add const
  * Fixed DiagnosticStatus
  * Changed odometry_data to std::optional
  * Refactored debug output in pose instability detector
  * style(pre-commit): autofix
  * Remove warning message for negative time
  difference in PoseInstabilityDetector
  * Updated rqt_runtime_monitor.png
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(geo_pose_projector): use geo_pose_projector in eagleye (`#5513 <https://github.com/autowarefoundation/autoware.universe/issues/5513>`_)
  * feat(tier4_geo_pose_projector): use tier4_geo_pose_projector in eagleye
  * style(pre-commit): autofix
  * fix(eagleye): split fix2pose
  * style(pre-commit): autofix
  * fix name: fuser -> fusion
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * update readme
  * style(pre-commit): autofix
  * add #include <string>
  * add rclcpp in dependency
  * style(pre-commit): autofix
  * add limitation in readme
  * style(pre-commit): autofix
  * update tier4_localization_launch
  * update tier4_localization_launch
  * rename package
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(eagleye): split fix2pose (`#5506 <https://github.com/autowarefoundation/autoware.universe/issues/5506>`_)
  * fix(eagleye): split fix2pose
  * style(pre-commit): autofix
  * fix name: fuser -> fusion
  * update package.xml
  * style(pre-commit): autofix
  * fix typo
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(landmark_based_localizer): refactored landmark_tf_caster (`#5414 <https://github.com/autowarefoundation/autoware.universe/issues/5414>`_)
  * Removed landmark_tf_caster node
  * Added maintainer
  * style(pre-commit): autofix
  * Renamed to landmark_parser
  * Added include<map>
  * style(pre-commit): autofix
  * Added publish_landmark_markers
  * Removed unused package.xml
  * Changed from depend to build_depend
  * Fixed a local variable name
  * Fixed Marker to MarkerArray
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(yabloc_image_processing): support both of  raw and compressed image input (`#5209 <https://github.com/autowarefoundation/autoware.universe/issues/5209>`_)
  * add raw image subscriber
  * update README
  * improve format and variable names
  ---------
* feat(pose_twist_estimator): automatically initialize pose only with gnss (`#5115 <https://github.com/autowarefoundation/autoware.universe/issues/5115>`_)
* fix(tier4_localization_launch):  fixed exec_depend (`#5075 <https://github.com/autowarefoundation/autoware.universe/issues/5075>`_)
  * Fixed exec_depend
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ar_tag_based_localizer): split the package `ar_tag_based_localizer` (`#5043 <https://github.com/autowarefoundation/autoware.universe/issues/5043>`_)
  * Fix package name
  * Removed utils
  * Renamed tag_tf_caster to landmark_tf_caster
  * Updated node_diagram
  * Fixed documents
  * style(pre-commit): autofix
  * Fixed the directory name
  * Fixed to split packages
  * Removed unused package dependency
  * style(pre-commit): autofix
  * Fixed directory structure
  * style(pre-commit): autofix
  * Fixed ArTagDetector to ArTagBasedLocalizer
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ar_tag_based_localizer): add ekf_pose subscriber (`#4946 <https://github.com/autowarefoundation/autoware.universe/issues/4946>`_)
  * Fixed qos
  * Fixed camera_frame\_
  * Fixed for awsim
  * Removed camera_frame
  * Fixed parameters
  * Fixed variable name
  * Updated README.md and added sample result
  * Updated README.md
  * Fixed distance_threshold to 13m
  * Implemented sub ekf_pose
  * style(pre-commit): autofix
  * Fixed the type of second to double
  * Fixed initializing
  * Fix to use rclcpp::Time and rclcpp::Duration
  * Added detail description about ekf_pose
  * style(pre-commit): autofix
  * Fixed nanoseconds
  * Added comments to param.yaml
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ar_tag_based_localizer): added small changes (`#4885 <https://github.com/autowarefoundation/autoware.universe/issues/4885>`_)
  * Fixed qos
  * Fixed camera_frame\_
  * Fixed for awsim
  * Removed camera_frame
  * Fixed parameters
  * Fixed variable name
  * Updated README.md and added sample result
  * Updated README.md
  * Fixed distance_threshold to 13m
  ---------
* feat(localization): add a new localization package `ar_tag_based_localizer` (`#4347 <https://github.com/autowarefoundation/autoware.universe/issues/4347>`_)
  * Added ar_tag_based_localizer
  * style(pre-commit): autofix
  * Added include
  * Fixed typo
  * style(pre-commit): autofix
  * Added comment
  * Updated license statements
  * Updated default topic names
  * Replaced "_2\_" to "_to\_"
  * Fixed tf_listener\_ shared_ptr to unique_ptr
  * Removed unused get_transform
  * Fixed alt text
  * Fixed topic name
  * Fixed default topic name of tag_tf_caster
  * Fixed AR Tag Based Localizer to work independently
  * Added principle
  * Fixed how to launch
  * Added link to sample data
  * Added sample_result.png
  * Update localization/ar_tag_based_localizer/README.md
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Update localization/ar_tag_based_localizer/README.md
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Fixed LaneLet2 to Lanelet2
  * style(pre-commit): autofix
  * Update localization/ar_tag_based_localizer/src/ar_tag_based_localizer_core.cpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * style(pre-commit): autofix
  * Update localization/ar_tag_based_localizer/config/tag_tf_caster.param.yaml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Added unit to range parameter
  * Removed std::pow
  * Removed marker_size\_ != -1
  * Fixed maintainer
  * Added ar_tag_based_localizer to tier4_localization_launch/package.xml
  * style(pre-commit): autofix
  * Fixed legend of node_diagram
  * style(pre-commit): autofix
  * Renamed range to distance_threshold
  * Fixed topic names in README.md
  * Fixed parameter input
  * Removed right_to_left\_
  * Added namespace ar_tag_based_localizer
  * Updated inputs/outputs
  * Fixed covariance
  * style(pre-commit): autofix
  * Added principle of tag_tf_caster
  * Removed ament_lint_auto
  * Fixed launch name
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* feat(yabloc_monitor): add yabloc_monitor (`#4395 <https://github.com/autowarefoundation/autoware.universe/issues/4395>`_)
  * feat(yabloc_monitor): add yabloc_monitor
  * style(pre-commit): autofix
  * add readme
  * style(pre-commit): autofix
  * update config
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * remove unnecessary part
  * remove todo
  * fix typo
  * remove unnecessary part
  * update readme
  * shorten function
  * reflect chatgpt
  * style(pre-commit): autofix
  * update
  * cland-tidy
  * style(pre-commit): autofix
  * update variable name
  * fix if name
  * use nullopt (and moved yabloc monitor namespace
  * fix readme
  * style(pre-commit): autofix
  * add dependency
  * style(pre-commit): autofix
  * reflect comment
  * update comment
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_localization_launch): change input/pointcloud param (`#4411 <https://github.com/autowarefoundation/autoware.universe/issues/4411>`_)
  * refactor(tier4_localization_launch): change input/pointcloud param
  * parameter renaming moved util.launch.py
* feat(yabloc): change namespace (`#4389 <https://github.com/autowarefoundation/autoware.universe/issues/4389>`_)
  * fix(yabloc): update namespace
  * fix
  ---------
* feat: use `pose_source` and `twist_source` for selecting localization methods (`#4257 <https://github.com/autowarefoundation/autoware.universe/issues/4257>`_)
  * feat(tier4_localization_launch): add pose_twist_estimator.launch.py
  * update format
  * update launcher
  * update pose_initailizer config
  * Move pose_initializer to pose_twist_estimator.launch.py, move yabloc namespace
  * use launch.xml instead of launch.py
  * Validated that all the configuration launches correctly (not performance eval yet)
  * Remove arg
  * style(pre-commit): autofix
  * Update eagleye param path
  * minor update
  * fix minor bugs
  * fix minor bugs
  * Introduce use_eagleye_twist args in eagleye_rt.launch.xml to control pose/twist relay nodes
  * Update pose_initializer input topic when using eagleye
  * Add eagleye dependency in tier4_localization_launch
  * Update tier4_localization_launch readme
  * style(pre-commit): autofix
  * Update svg
  * Update svg again (transparent background)
  * style(pre-commit): autofix
  * Update yabloc document
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(yabloc): add camera and vector map localization (`#3946 <https://github.com/autowarefoundation/autoware.universe/issues/3946>`_)
  * adopt scane_case to undistort, segment_filter
  * adopt scane_case to ground_server, ll2_decomposer
  * adopt scane_case to twist_converter, twist_estimator
  * adopt scane_case to validation packages
  * adopt scane_case tomodularized_particle_filter
  * adopt scane_case to gnss_particle_corrector
  * adopt scane_case to camera_particle_corrector
  * adopt scane_case to antishadow_corrector
  * adopt scane_case to particle_initializer
  * organize launch files
  * add twist_visualizer to validate odometry performance
  * use SE3::exp() to predict particles & modify linear noise model
  * stop to use LL2 to rectify initialpose2d
  * fix redundant computation in segment_accumulator
  * improve gnss_particle_corrector
  * fix segment_accumulator's bug
  * add doppler_converter
  * add xx2.launch.xml
  * add hsv_extractor
  * pickup other regions which have same color histogram
  * use additional region to filt line-segments
  * improve graph-segmentation
  * remove `truncate_pixel_threshold`
  * refactor graph_segmentator & segment_filter
  * add mahalanobis_distance_threshold in GNSS particle corrector
  * add extract_line_segments.hpp
  * use pcl::transformCloudWithNormals instead of  transform_cloud
  * filt accumulating segments by LL2
  * move herarchical_cost_map to common
  * apply positive feedback for accumulation
  * move transform_linesegments() to common pkg
  * refactor
  * use all projected lines for camera corrector
  * evaluate iffy linesegments
  * complete to unify ll2-assisted lsd clasification
  * add abs_cos2() which is more strict direction constraint
  * fix orientation initialization bug
  * publish doppler direction
  * TMP: add disable/enable switch for camera corrector
  * implement doppler orientation correction but it's disabled by default
  * speed up camera corrector
  * update ros params
  * implement kalman filter for ground tilt estimation
  * continuous height estimation works well?
  * estimate height cotiniously
  * use only linesegments which are at same height
  * add static_gyro_bias parameter
  * fix bug about overlay varidation
  * increse ll2 height marging in cost map generation
  * add static_gyro_bias in twist.launch.xml
  * load pcdless_init_area from ll2
  * add specified initialization area
  * add corrector_manager node to disable/enable camera_corrector
  * call service to disable camer_corrector from manager
  * load corrector disable area
  * overlay even if pose is not estiamted
  * publish camera corrector's status as string
  * add set_booL_panel for camera_corrector enable/disable
  * load bounding box from lanelet2
  * draw bounding box on cost map
  * remove at2,at1 from cost map
  * use cost_map::at() instread pf at2()
  * move cost map library from common to camera corrector
  * use logit for particle weighting but it does not work well
  * prob_to_logit() requires non-intuitive parameters
  * goodbye stupid parameters (max_raw_score & score_offset)
  * publish two scored pointclouds as debug
  * can handle unmapped areas
  * remove obsolete packages
  * update README.md
  * Update README.md
  * add image of how_to_launch
  * add node diagram in readme
  * add rviz_description.png in README
  * subscribe pose_with_cov & disconnect base_link <-> particle_pose
  * remove segment_accumulator & launch ekf_localizer from this project
  * add gnss_ekf_corrector
  * add camera_ekf_corrector package
  * subscribe ekf prediction & synch pose data
  * WIP: ready to implement UKF?
  * estimate weighted averaging as pose_estimator
  * basic algorithm is implemented but it does not work proparly
  * apply after_cov_gain\_
  * ekf corrector works a little bit appropriately
  * increase twist covariance for ekf
  * test probability theory
  * updat prob.py
  * implement de-bayesing but it loooks ugly
  * remove obsolete parameters
  * skip measurement publishing if travel distance is so short
  * use constant covariance because i dont understand what is correct
  * add submodule sample_vehicle_launch
  * TMP but it works
  * add ekf_trigger in particle_initializer.hpp
  * publish gnss markers & camera_est pubishes constant cov
  * back to pcd-less only launcher
  * add bayes_util package
  * apply de-bayesing for camera_ekf
  * some launch file update
  * organize launch files. we can choice mode from ekf/pekf/pf
  * organize particle_initializer
  * add swap_mode_adaptor WIP
  * use latest ekf in autoware & sample_vehicle
  * fix bug of swap_adalptor
  * fix FIX & FLOAT converter
  * fix septentrio doppler converter
  * move ekf packages to ekf directory
  * ignore corrector_manager
  * add standalone arg in launch files
  * update semseg_node
  * add camera_pose_initializer pkg
  * subscribe camera_info&tf and prepare semantic projection
  * project semantic image
  * create vector map image from ll2
  * create lane image from vector map
  * search the most match angle by non-zero pixels
  * camera based pose_initializer
  * move ekf packages into unstable
  * move ekf theory debugger
  * add tier4_autoware_msgs as submodule
  * move pose_initializer into initializer dir
  * add semse_msgs pkg
  * separate marker pub function
  * separate projection functions
  * add semseg_srv client
  * move sem-seg directory
  * camera pose initilizer works successfully
  * rectify covariance along the orientation
  * improve initialization parameters
  * take into account covariance of request
  * use lanelet direciton to compute init pose scores
  * semseg download model automatically
  * remove sample_vehicle_launch
  * add autoware_msgs
  * remove obsolete launch files
  * add standalone mode for direct initialization
  * fix fix_to_pose
  * update launch files
  * update rviz config
  * remove lidar_particle_corrector
  * remove Sophus from sunbmodule
  * rename submodule directory
  * update README and some sample images
  * update README.md
  * fix override_camera_frame_id bahaviors
  * fix some bugs (`#4 <https://github.com/autowarefoundation/autoware.universe/issues/4>`_)
  * fix: use initialpose from Rviz (`#6 <https://github.com/autowarefoundation/autoware.universe/issues/6>`_)
  * use initialpose from Rviz to init
  * add description about how-to-set-initialpose
  ---------
  * misc: add license (`#7 <https://github.com/autowarefoundation/autoware.universe/issues/7>`_)
  * WIP: add license description
  * add license description
  * add description about license in README
  ---------
  * add quick start demo (`#8 <https://github.com/autowarefoundation/autoware.universe/issues/8>`_)
  * refactor(launch) remove & update obsolete launch files (`#9 <https://github.com/autowarefoundation/autoware.universe/issues/9>`_)
  * delete obsolete launch files
  * update documents
  ---------
  * docs(readme): update architecture image (`#10 <https://github.com/autowarefoundation/autoware.universe/issues/10>`_)
  * replace architecture image in README
  * update some images
  ---------
  * refactor(pcdless_launc/scripts): remove unnecessary scripts (`#11 <https://github.com/autowarefoundation/autoware.universe/issues/11>`_)
  * remove not useful scripts
  * rename scripts &  add descriptions
  * little change
  * remove odaiba.rviz
  * grammer fix
  ---------
  * fix(pcdless_launch): fix a build bug
  * fix(twist_estimator): use velocity_report by default
  * fix bug
  * debugged, now works
  * update sample rosbag link (`#14 <https://github.com/autowarefoundation/autoware.universe/issues/14>`_)
  * feature(graph_segment, gnss_particle_corrector): make some features switchable (`#17 <https://github.com/autowarefoundation/autoware.universe/issues/17>`_)
  * make additional-graph-segment-pickup disablable
  * enlarge gnss_mahalanobis_distance_threshold in expressway.launch
  ---------
  * fix: minor fix for multi camera support (`#18 <https://github.com/autowarefoundation/autoware.universe/issues/18>`_)
  * fix: minor fix for multi camera support
  * update
  * update
  * fix typo
  ---------
  * refactor(retroactive_resampler): more readable (`#19 <https://github.com/autowarefoundation/autoware.universe/issues/19>`_)
  * make Hisotry class
  * use boost:adaptors::indexed()
  * add many comment in resampling()
  * does not use ConstSharedPtr
  * rename interface of resampler
  * circular_buffer is unnecessary
  ---------
  * refactor(mpf::predictor) resampling interval control in out of resampler (`#20 <https://github.com/autowarefoundation/autoware.universe/issues/20>`_)
  * resampling interval management should be done out of resample()
  * resampler class throw exeption rather than optional
  * split files for resampling_history
  * split files for experimental/suspention_adaptor
  ---------
  * refactor(mpf::predictor): just refactoring (`#21 <https://github.com/autowarefoundation/autoware.universe/issues/21>`_)
  * remove obsolete functions
  * remove test of predictor
  * remove remapping in pf.launch.xml for suspension_adapator
  * add some comments
  ---------
  * fix(twist_estimator): remove stop filter for velocity (`#23 <https://github.com/autowarefoundation/autoware.universe/issues/23>`_)
  * feat(pcdless_launch): add multi camera launcher (`#22 <https://github.com/autowarefoundation/autoware.universe/issues/22>`_)
  * feat(pcdless_launch): add multi camera launcher
  * minor fix
  ---------
  * refactor(CMakeListx.txt): just refactoring (`#24 <https://github.com/autowarefoundation/autoware.universe/issues/24>`_)
  * refactor imgproc/*/CMakeListx.txt
  * refactor initializer/*/CMakeListx.txt & add gnss_pose_initializer pkg
  * rename some files in twist/ & refactor pf/*/cmakelist
  * refactor validation/*/CMakeListx.txt
  * fix some obsolete executor name
  ---------
  * fix: rename lsd variables and files (`#26 <https://github.com/autowarefoundation/autoware.universe/issues/26>`_)
  * misc: reame pcdless to yabloc (`#25 <https://github.com/autowarefoundation/autoware.universe/issues/25>`_)
  * rename pcdless to yabloc
  * fix conflict miss
  ---------
  * visualize path (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * docs: update readme about particle filter (`#30 <https://github.com/autowarefoundation/autoware.universe/issues/30>`_)
  * update mpf/README.md
  * update gnss_corrector/README.md
  * update camera_corrector/README.md
  ---------
  * feat(segment_filter): publish images with lines and refactor (`#29 <https://github.com/autowarefoundation/autoware.universe/issues/29>`_)
  * feat(segment_filter): publish images with lines
  * update validation
  * update imgproc (reverted)
  * large change inclding refactoring
  * major update
  * revert rviz config
  * minor fix in name
  * add validation option
  * update architecture svg
  * rename validation.launch to overlay.launch
  * no throw runtime_error (unintentionaly applying format)
  ---------
  Co-authored-by: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  * catch runtime_error when particle id is invalid (`#31 <https://github.com/autowarefoundation/autoware.universe/issues/31>`_)
  * return if info is nullopt (`#32 <https://github.com/autowarefoundation/autoware.universe/issues/32>`_)
  * pose_buffer is sometimes empty (`#33 <https://github.com/autowarefoundation/autoware.universe/issues/33>`_)
  * use_yaw_of_initialpose (`#34 <https://github.com/autowarefoundation/autoware.universe/issues/34>`_)
  * feat(interface):  remove incompatible interface (`#35 <https://github.com/autowarefoundation/autoware.universe/issues/35>`_)
  * not use ublox_msg when run as autoware
  * remove twist/kalman/twist & use twist_estimator/twist_with_covariance
  * update particle_array stamp even if the time stamp seems wrong
  ---------
  * fix: suppress info/warn_stream (`#37 <https://github.com/autowarefoundation/autoware.universe/issues/37>`_)
  * does not stream undistortion time
  * improve warn stream when skip particle weighting
  * surpress frequency of  warnings during synchronized particle searching
  * fix camera_pose_initializer
  ---------
  * /switch must not be nice name (`#39 <https://github.com/autowarefoundation/autoware.universe/issues/39>`_)
  * misc(readme): update readme (`#41 <https://github.com/autowarefoundation/autoware.universe/issues/41>`_)
  * add youtube link and change thumbnail
  * improve input/output topics
  * quick start demo screen image
  * add abstruct architecture and detail architecture
  ---------
  * docs(rosdep): fix package.xml to ensure build success (`#44 <https://github.com/autowarefoundation/autoware.universe/issues/44>`_)
  * fix package.xml to success build
  * add 'rosdep install' in how-to-build
  ---------
  * add geographiclib in package.xml (`#46 <https://github.com/autowarefoundation/autoware.universe/issues/46>`_)
  * fix path search error in build stage (`#45 <https://github.com/autowarefoundation/autoware.universe/issues/45>`_)
  * fix path search error in build stage
  * fix https://github.com/tier4/YabLoc/pull/45#issuecomment-1546808419
  * Feature/remove submodule (`#47 <https://github.com/autowarefoundation/autoware.universe/issues/47>`_)
  * remove submodules
  * remove doppler converter
  ---------
  * feature: change node namespace to /localization/yabloc/** from /localization/** (`#48 <https://github.com/autowarefoundation/autoware.universe/issues/48>`_)
  * change node namespace
  * update namespace for autoware-mode
  * update namespace in multi_camera.launch
  ---------
  * removed unstable packages (`#49 <https://github.com/autowarefoundation/autoware.universe/issues/49>`_)
  * feature: add *.param.yaml to manage parameters (`#50 <https://github.com/autowarefoundation/autoware.universe/issues/50>`_)
  * make *.param.yaml in imgproc packages
  * make *.param.yaml in initializer packages
  * make *.param.yaml in map packages
  * make *.param.yaml in pf packages
  * make *.param.yaml in twist packages
  * fix expressway parameter
  * fix override_frame_id
  * remove default parameters
  * fix some remaining invalida parameters
  ---------
  * does not estimate twist (`#51 <https://github.com/autowarefoundation/autoware.universe/issues/51>`_)
  * feat(particle_initializer): merge particle_initializer into mpf (`#52 <https://github.com/autowarefoundation/autoware.universe/issues/52>`_)
  * feat(particle_initializer): merge particle_initializer to modulalized_particle_filter
  * remove particle_initializer
  * remove debug message
  * remove related parts
  * update readme
  * rename publishing topic
  ---------
  Co-authored-by: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  * fix: remove ll2_transition_area (`#54 <https://github.com/autowarefoundation/autoware.universe/issues/54>`_)
  * feature(initializer): combine some initializer packages (`#56 <https://github.com/autowarefoundation/autoware.universe/issues/56>`_)
  * combine some package about initializer
  * yabloc_pose_initializer works well
  * remove old initializer packages
  * semseg node can launch
  * fix bug
  * revert initializer mode
  ---------
  * feature(imgproc): reudce imgproc packages (`#57 <https://github.com/autowarefoundation/autoware.universe/issues/57>`_)
  * combine some imgproc packages
  * combine overlay monitors into imgproc
  ---------
  * feature(validation): remove validation packages (`#58 <https://github.com/autowarefoundation/autoware.universe/issues/58>`_)
  * remove validation packages
  * remove path visualization
  ---------
  * feature(pf): combine some packages related to particle filter (`#59 <https://github.com/autowarefoundation/autoware.universe/issues/59>`_)
  * create yabloc_particle_filter
  * combine gnss_particle_corrector
  * combine ll2_cost_map
  * combine camera_particle_corrector
  * fix launch files
  * split README & remove obsolete scripts
  * fix config path of multi_camera mode
  ---------
  * feature: combine map and twist packages (`#60 <https://github.com/autowarefoundation/autoware.universe/issues/60>`_)
  * removed some twist nodes & rename remains to yabloc_twist
  * fix launch files for yabloc_twist
  * move map packages to yabloc_common
  * WIP: I think its impossible
  * Revert "WIP: I think its impossible"
  This reverts commit 49da507bbf9abe8fcebed4d4df44ea5f4075f6d1.
  * remove map packages & fix some launch files
  ---------
  * removed obsolete packages
  * remove obsolete dot files
  * use tier4_loc_launch instead of yabloc_loc_launch
  * move launch files to each packages
  * remove yabloc_localization_launch
  * remove yabloc_launch
  * modify yabloc/README.md
  * update yabloc_common/README.md
  * update yabloc_imgproc README
  * update yabloc_particle_filter/README
  * update yabloc_pose_initializer/README
  * update README
  * use native from_bin_msg
  * use ifndef instead of pragma once in yabloc_common
  * use ifndef instead of pragma once in yabloc_imgproc & yabloc_pf
  * use ifndef instead of pragma once in yabloc_pose_initializer
  * style(pre-commit): autofix
  * use autoware_cmake & suppress build warning
  * repalce yabloc::Timer with  tier4_autoware_utils::StopWatch
  * replace 1.414 with std::sqrt(2)
  * style(pre-commit): autofix
  * removed redundant ament_cmake_auto
  * removed yabloc_common/timer.hpp
  * replaced low_pass_filter with autoware's lowpass_filter_1d
  * style(pre-commit): autofix
  * Squashed commit of the following:
  commit cb08e290cca5c00315a58a973ec068e559c9e0a9
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:30:09 2023 +0900
  removed ublox_msgs in gnss_particle_corrector
  commit c158133f184a43914ec5f929645a7869ef8d03be
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:24:19 2023 +0900
  removed obsolete yabloc_multi_camera.launch
  commit 10f578945dc257ece936ede097544bf008e5f48d
  Author: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
  Date:   Tue Jun 13 14:22:14 2023 +0900
  removed ublox_msgs in yabloc_pose_initializer
  * style(pre-commit): autofix
  * removed fix2mgrs & ublox_stamp
  * added ~/ at the top of topic name
  * removed use_sim_time in yabloc launch files
  * add architecture diagram in README
  * rename lsd_node to line_segment_detector
  * style(pre-commit): autofix
  * Update localization/yabloc/README.md
  fix typo
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * removed obsolete debug code in similar_area_searcher
  * removed suspension_adaptor which manages lifecycle of particle predictor
  * style(pre-commit): autofix
  * renamed semseg to SemanticSegmentation
  * style(pre-commit): autofix
  * fixed README.md to solve markdownlint
  * WIP: reflected cpplint's suggestion
  * reflected cpplint's suggestion
  * rename AbstParaticleFilter in config files
  * fixed typo
  * used autoware_lint_common
  * fixed miss git add
  * style(pre-commit): autofix
  * replaced lanelet_util by lanelet2_extension
  * replaced fast_math by tie4_autoware_utils
  * sort package.xml
  * renamed yabloc_imgproc with yabloc_image_processing
  * reflected some review comments
  * resolved some TODO
  * prioritize NDT if both NDT and YabLoc initializer enabled
  * changed localization_mode option names
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: kminoda <koji.minoda@tier4.jp>
  Co-authored-by: Akihiro Komori <akihiro.komori@unity3d.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* style: fix typos (`#3617 <https://github.com/autowarefoundation/autoware.universe/issues/3617>`_)
  * style: fix typos in documents
  * style: fix typos in package.xml
  * style: fix typos in launch files
  * style: fix typos in comments
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat: add gnss/imu localizer  (`#3063 <https://github.com/autowarefoundation/autoware.universe/issues/3063>`_)
  * Add gnss_imu_localizar
  * Fix twist switching bug
  * Fix spell and reformat
  * Parameterize directories with related launches
  * Fix mis-spell
  * Correction of characters not registered in the dictionary
  * Make ealeye_twist false
  * Delete unnecessary parts
  * Rename localization switching parameters
  * Rename twist_estimator_mode parameter pattern
  * Simplify conditional branching
  * Support for changes in pose_initializer
  * Fix problem of double eagleye activation
  * Fix unnecessary changes
  * Remove conditional branching by pose_estimatar_mode in system_error_monitor
  * Change launch directory structure
  * Remove unnecessary parameters and files
  * Fix indentations
  * Coding modifications based on conventions
  * Change the structure diagram in the package
  * Integrate map4_localization_component1,2
  * Add drawio.svg
  * Delete duplicate files
  * Change auther and add maintainer
  * Delete unnecessary modules in drawio
  * Fixing confusing sentences
  * Fine-tuning of drawio
  * Fix authomaintainerr
  * Rename ndt to ndt_scan_matcher
  * follow the naming convention
  * Add newlines to the end of files to fix end-of-file-fixer hook errors
  * List the packages that depend on map4_localization_launch correctly
  * Ran precommit locally
  ---------
* chore(tier4_localization_launch): add maintainer (`#3133 <https://github.com/autowarefoundation/autoware.universe/issues/3133>`_)
* chore(ekf_localizer): move parameters to its dedicated yaml file (`#3039 <https://github.com/autowarefoundation/autoware.universe/issues/3039>`_)
  * chores(ekf_localizer): move parameters to its dedicated yaml file
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(pose_initializer): enable pose initialization while running (only for sim) (`#3038 <https://github.com/autowarefoundation/autoware.universe/issues/3038>`_)
  * feat(pose_initializer): enable pose initialization while running (only for sim)
  * both logsim and psim params
  * only one pose_initializer_param_path arg
  * use two param files for pose_initializer
  ---------
* feat(pose_initilizer): support gnss/imu pose estimator (`#2904 <https://github.com/autowarefoundation/autoware.universe/issues/2904>`_)
  * Support GNSS/IMU pose estimator
  * style(pre-commit): autofix
  * Revert gnss/imu support
  * Support GNSS/IMU pose estimator
  * style(pre-commit): autofix
  * Separate EKF and NDT trigger modules
  * Integrate activate and deactivate into sendRequest
  * style(pre-commit): autofix
  * Change sendRequest function arguments
  * style(pre-commit): autofix
  * Remove unused conditional branches
  * Fix command name
  * Change to snake_case
  * Fix typos
  * Update localization/pose_initializer/src/pose_initializer/ekf_localization_trigger_module.cpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update localization/pose_initializer/src/pose_initializer/ndt_localization_trigger_module.cpp
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * Update copyright year
  * Set the copyright year of ekf_localization_module to 2022
  * Delete unnecessary conditional branches
  * Add ekf_enabled parameter
  * Add #include <string>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Ryohei Sasaki <ryohei.sasaki@map4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat(tier4_localization_launch): remove configs and move to autoware_launch (`#2537 <https://github.com/autowarefoundation/autoware.universe/issues/2537>`_)
  * feat(tier4_localization_launch): remove configs and move to autoware_launch
  * update readme
  * Update launch/tier4_localization_launch/README.md
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
  * fix order
  * remove config
  * update readme
  * pre-commit
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* feat(tier4_localization_launch): pass pc container to localization (`#2114 <https://github.com/autowarefoundation/autoware.universe/issues/2114>`_)
  * feature(tier4_localization_launch): pass pc container to localization
  * ci(pre-commit): autofix
  * feature(tier4_localization_launch): update util.launch.xml
  * feature(tier4_localization_launch): update use container param value
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ndt): remove ndt package (`#2053 <https://github.com/autowarefoundation/autoware.universe/issues/2053>`_)
  * first commit
  * CMakeLists.txt does not work........
  * build works
  * debugged
  * remove unnecessary parameter
  * ci(pre-commit): autofix
  * removed 'omp'-related words completely
  * ci(pre-commit): autofix
  * fixed param description of converged_param
  * remove OMPParams
  * removed unnecessary includes
  * removed default parameter from search_method
  * small fix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: add adapi dependency (`#1892 <https://github.com/autowarefoundation/autoware.universe/issues/1892>`_)
* feat(pose_initializer)!: support ad api (`#1500 <https://github.com/autowarefoundation/autoware.universe/issues/1500>`_)
  * feat(pose_initializer): support ad api
  * docs: update readme
  * fix: build error
  * fix: test
  * fix: auto format
  * fix: auto format
  * feat(autoware_ad_api_msgs): define localization interface
  * feat: update readme
  * fix: copyright
  * fix: main function
  * Add readme of localization message
  * feat: modify stop check time
  * fix: fix build error
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_localization_launch): manual sync with tier4/localization_launch (`#1442 <https://github.com/autowarefoundation/autoware.universe/issues/1442>`_)
  * feat(tier4_localization_launch): manual sync with tier4/localization_launch
  * ci(pre-commit): autofix
  * fix
  * revert modification
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ekf_localizer): rename biased pose topics (`#1787 <https://github.com/autowarefoundation/autoware.universe/issues/1787>`_)
  * fix(ekf_localizer): rename biased pose topics
  * Update topic descriptions in README
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* feat(default_ad_api): add localization api  (`#1431 <https://github.com/autowarefoundation/autoware.universe/issues/1431>`_)
  * feat(default_ad_api): add localization api
  * docs: add readme
  * feat: add auto initial pose
  * feat(autoware_ad_api_msgs): define localization interface
  * fix(default_ad_api): fix interface definition
  * feat(default_ad_api): modify interface version api to use spec package
  * feat(default_ad_api): modify interface version api to use spec package
  * fix: pre-commit
  * fix: pre-commit
  * fix: pre-commit
  * fix: copyright
  * feat: split helper package
  * fix: change topic name to local
  * fix: style
  * fix: style
  * fix: style
  * fix: remove needless keyword
  * feat: change api helper node namespace
  * fix: fix launch file path
* chore(localization packages, etc): modify maintainer and e-mail address (`#1661 <https://github.com/autowarefoundation/autoware.universe/issues/1661>`_)
  * chore(localization packages, etc): modify maintainer and e-mail address
  * remove indent
  * add authors
  * Update localization/ekf_localizer/package.xml
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * Update localization/localization_error_monitor/package.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * fix name
  * add author
  * add author
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* fix(ekf_localizer): enable enable_yaw_bias (`#1601 <https://github.com/autowarefoundation/autoware.universe/issues/1601>`_)
  * fix(ekf_localizer): enable enable_yaw_bias
  * remove proc_stddev_yaw_bias from ekf
  * ci(pre-commit): autofix
  * enlarge init covariance of yaw bias
  * ci(pre-commit): autofix
  * fixed minor bugs
  * change default parameter
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ndt_scan_matcher): fix default parameter to 0.0225 (`#1583 <https://github.com/autowarefoundation/autoware.universe/issues/1583>`_)
  * fix(ndt_scan_matcher): fix default parameter to 0.0225
  * added a sidenote
  * added a sidenote
* feat(localization_error_monitor): change subscribing topic type (`#1532 <https://github.com/autowarefoundation/autoware.universe/issues/1532>`_)
  * feat(localization_error_monitor): change subscribing topic type
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_localization_launch): declare param path argument (`#1404 <https://github.com/autowarefoundation/autoware.universe/issues/1404>`_)
  * first commit
  * added arguments in each launch files
  * finished implementation
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_localization_launch): change rectified pointcloud to outlier_filtered pointcloud (`#1365 <https://github.com/autowarefoundation/autoware.universe/issues/1365>`_)
* fix(tier4_localization_launch): add group tag (`#1237 <https://github.com/autowarefoundation/autoware.universe/issues/1237>`_)
  * fix(tier4_localization_launch): add group tag
  * add more args into group
* feat(localization_error_monitor): add a config file (`#1282 <https://github.com/autowarefoundation/autoware.universe/issues/1282>`_)
  * feat(localization_error_monitor): add a config file
  * ci(pre-commit): autofix
  * feat(localization_error_monitor): add a config file in tier4_localization_launch too
  * ci(pre-commit): autofix
  * debugged
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_localization_launch): remove unnecessary param from pose_twist_fusion_filter.launch (`#1224 <https://github.com/autowarefoundation/autoware.universe/issues/1224>`_)
* feat(ekf_localizer): allow multi sensor inputs in ekf_localizer (`#1027 <https://github.com/autowarefoundation/autoware.universe/issues/1027>`_)
  * first commit
  * ci(pre-commit): autofix
  * updated
  * deque to queue
  * ci(pre-commit): autofix
  * queue debugged
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * deque to queue
  * queue didn't support q.clear()...
  * for debug, and must be ignored later
  * ci(pre-commit): autofix
  * removed dummy variables
  * ci(pre-commit): autofix
  * run pre-commit
  * update readme
  * update readme
  * ci(pre-commit): autofix
  * reflected some review comments
  * reflected some review comments
  * added smoothing_steps param in pose_info and twist_info
  * ci(pre-commit): autofix
  * use withcovariance in PoseInfo & TwistInfo, now build works
  * ci(pre-commit): autofix
  * (not verified yet) update z, roll, pitch using 1D filter
  * ci(pre-commit): autofix
  * added TODO comments
  * ci(pre-commit): autofix
  * update initialization of simple1DFilter
  * fixed a bug (=NDT did not converge when launching logging_simulator)
  * debug
  * change gnss covariance, may have to be removed from PR
  * ci(pre-commit): autofix
  * removed unnecessary comments
  * added known issue
  * ci(pre-commit): autofix
  * change the default gnss covariance to the previous one
  * pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(distortion_corrector): use gyroscope for correcting LiDAR distortion (`#1120 <https://github.com/autowarefoundation/autoware.universe/issues/1120>`_)
  * first commit
  * ci(pre-commit): autofix
  * check if angular_velocity_queue\_ is empty or not
  * move vehicle velocity converter to sensing
  * ci(pre-commit): autofix
  * fix
  * ci(pre-commit): autofix
  * reflected reviews
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: regularized NDT matching (`#1006 <https://github.com/autowarefoundation/autoware.universe/issues/1006>`_)
  * add interface of gnss regularization in ndt class
  * gnss pose is applied to regularize NDT
  * add descriptions in ndt_scan_matcher/README
  * fix typo in README
  * applied formatter for README.md
  * rename and refine functions for regularization
  * fixed typo
  * add descriptions of regularization to README
  * modify README to visualize well
  * fixed descriptions about principle of regularization
  Co-authored-by: Kento Yabuuchi <kento.yabuuchi.2@tier4.jp>
* feat(twist2accel)!: add new package for estimating acceleration in localization module (`#1089 <https://github.com/autowarefoundation/autoware.universe/issues/1089>`_)
  * first commit
  * update launch arg names
  * use lowpassfilter in signalprocessing
  * fixed
  * add acceleration estimation
  * ci(pre-commit): autofix
  * fix readme and lisence
  * ci(pre-commit): autofix
  * fix readme
  * ci(pre-commit): autofix
  * added covariance values
  * removed unnecessary variable
  * rename acceleration_estimator -> twist2accel
  * ci(pre-commit): autofix
  * added future work
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat: added raw twist in gyro_odometer (`#676 <https://github.com/autowarefoundation/autoware.universe/issues/676>`_)
  * feat: added raw twist output from gyro_odometer
  * fix: prettier
* fix: localization and perception launch for tutorial (`#645 <https://github.com/autowarefoundation/autoware.universe/issues/645>`_)
  * fix: localization and perception launch for tutorial
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ndt_scan_matcher): add nearest voxel transfromation probability (`#364 <https://github.com/autowarefoundation/autoware.universe/issues/364>`_)
  * feat(ndt_scan_matcher): add nearest voxel transfromation probability
  * add calculateTransformationProbability funcs
  * add calculateTransformationProbability funcs
  * add converged_param_nearest_voxel_transformation_probability
  * fix error
  * refactoring convergence conditions
  * fix error
  * remove debug code
  * remove debug code
  * ci(pre-commit): autofix
  * fix typo
  * ci(pre-commit): autofix
  * rename likelihood
  * ci(pre-commit): autofix
  * avoid a warning unused parameter
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ndt_scan_matcher): add tolerance of initial pose (`#408 <https://github.com/autowarefoundation/autoware.universe/issues/408>`_)
  * feat(ndt_scan_matcher): add tolerance of initial pose
  * move codes
  * modify the default value
  * change the variable names
  * ci(pre-commit): autofix
  * fix typo
  * add depend fmt
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ndt_scan_matcher): add particles param (`#330 <https://github.com/autowarefoundation/autoware.universe/issues/330>`_)
  * feat(ndt_scan_matcher): add particles param
  * fix data type
  * ci(pre-commit): autofix
  * fix data type
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: remove unused param (`#291 <https://github.com/autowarefoundation/autoware.universe/issues/291>`_)
* fix: typo in localization util.launch.py (`#277 <https://github.com/autowarefoundation/autoware.universe/issues/277>`_)
* feat: add covariance param (`#281 <https://github.com/autowarefoundation/autoware.universe/issues/281>`_)
  * add covariance param
  * add description
  * add description
  * fix typo
  * refactor
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: change launch package name (`#186 <https://github.com/autowarefoundation/autoware.universe/issues/186>`_)
  * rename launch folder
  * autoware_launch -> tier4_autoware_launch
  * integration_launch -> tier4_integration_launch
  * map_launch -> tier4_map_launch
  * fix
  * planning_launch -> tier4_planning_launch
  * simulator_launch -> tier4_simulator_launch
  * control_launch -> tier4_control_launch
  * localization_launch -> tier4_localization_launch
  * perception_launch -> tier4_perception_launch
  * sensing_launch -> tier4_sensing_launch
  * system_launch -> tier4_system_launch
  * ci(pre-commit): autofix
  * vehicle_launch -> tier4_vehicle_launch
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: tanaka3 <ttatcoder@outlook.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: Kaan Çolak, Kenji Miyake, Kento Yabuuchi, Muhammed Yavuz Köseoğlu, SakodaShintaro, Shumpei Wakabayashi, Shunsuke Miura, TaikiYamada4, Takagi, Isamu, Takeshi Ishita, Tomoya Kimura, Vincent Richard, Xinyu Wang, Yamato Ando, YamatoAndo, Yukihiro Saito, kminoda, ryohei sasaki
