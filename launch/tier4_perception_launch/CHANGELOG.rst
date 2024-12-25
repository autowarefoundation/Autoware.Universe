^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_perception_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* chore(tier4_perception_launch): enable to receive argument `centerpoint_model_name` from autoware_launch (`#9003 <https://github.com/autowarefoundation/autoware.universe/issues/9003>`_)
  * enable to receive arguments
  * adopt transfusion
  * add lidar_detection_model_type
  * style(pre-commit): autofix
  * integrate all in lidar_detection_model
  * separate name and config
  * remove transfusion change
  * add default config on pp and transfusion
  * change variable name for easy to read
  * change variable name
  * fix condition when default model name
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(tier4_perception_launch): remove duplicated parameter declaration (`#9031 <https://github.com/autowarefoundation/autoware.universe/issues/9031>`_)
* feat(tier4_perception_launch): enable to use multi camera on traffic light recognition (`#8676 <https://github.com/autowarefoundation/autoware.universe/issues/8676>`_)
  * main process
  * style(pre-commit): autofix
  * add exception if input is invalid
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(autoware_lidar_transfusion): split config (`#8205 <https://github.com/autowarefoundation/autoware.universe/issues/8205>`_)
  * refactor(autoware_lidar_transfusion): split config
  * style(pre-commit): autofix
  * chore(autoware_lidar_transfusion): bypass schema CI workflow
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* fix(tier4_perception_launch): launch namespace of `detection_by_tracker` (`#8702 <https://github.com/autowarefoundation/autoware.universe/issues/8702>`_)
  fix: namespace of detection_by_tracker do not need to have the prefix `autoware\_`
* refactor(perception/occupancy_grid_map_outlier_filter): rework parameters (`#6745 <https://github.com/autowarefoundation/autoware.universe/issues/6745>`_)
  * add param and schema file, edit readme
  * .
  * correct linter errors
  ---------
* fix(tier4_perception_launch): set `use_image_transport` in launch (`#8315 <https://github.com/autowarefoundation/autoware.universe/issues/8315>`_)
  set use_image_transport in launch
* refactor: image transport decompressor/autoware prefix (`#8197 <https://github.com/autowarefoundation/autoware.universe/issues/8197>`_)
  * refactor: add `autoware` namespace prefix to image_transport_decompressor
  * refactor(image_transport_decompressor): add `autoware` prefix to the package code
  * refactor: update package name in CODEOWNER
  * fix: merge main into the branch
  * refactor: update packages which depend on image_transport_decompressor
  * refactor(image_transport_decompressor): update README
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* refactor: traffic light arbiter/autoware prefix (`#8181 <https://github.com/autowarefoundation/autoware.universe/issues/8181>`_)
  * refactor(traffic_light_arbiter): apply `autoware` namespace to traffic_light_arbiter
  * refactor(traffic_light_arbiter): update the package name in CODEWONER
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(probabilistic_occupancy_grid_map, occupancy_grid_map_outlier_filter): add autoware\_ prefix to package name (`#8183 <https://github.com/autowarefoundation/autoware.universe/issues/8183>`_)
  * chore: fix package name probabilistic occupancy grid map
  * fix: solve launch error
  * chore: update occupancy_grid_map_outlier_filter
  * style(pre-commit): autofix
  * refactor: update package name to autoware_probabilistic_occupancy_grid_map on a test
  * refactor: rename folder of occupancy_grid_map_outlier_filter
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* refactor(elevation_map_loader): add package name prefix `autoware\_`, fix namespace and directory structure (`#7988 <https://github.com/autowarefoundation/autoware.universe/issues/7988>`_)
  * refactor: add namespace, remove unused dependencies, file structure
  chore: remove unused dependencies
  style(pre-commit): autofix
  * refactor: rename elevation_map_loader to autoware_elevation_map_loader
  Rename the `elevation_map_loader` package to `autoware_elevation_map_loader` to align with the Autoware naming convention.
  style(pre-commit): autofix
* refactor(tensorrt_yolox)!: fix namespace and directory structure (`#7992 <https://github.com/autowarefoundation/autoware.universe/issues/7992>`_)
  * refactor: add autoware namespace prefix to `tensorrt_yolox`
  * refactor: apply `autoware` namespace to tensorrt_yolox
  * chore: update CODEOWNERS
  * fix: resolve `yolox_tiny` to work
  ---------
* refactor(traffic_light\_*)!: add package name prefix of autoware\_ (`#8159 <https://github.com/autowarefoundation/autoware.universe/issues/8159>`_)
  * chore: rename traffic_light_fine_detector to autoware_traffic_light_fine_detector
  * chore: rename traffic_light_multi_camera_fusion to autoware_traffic_light_multi_camera_fusion
  * chore: rename traffic_light_occlusion_predictor to autoware_traffic_light_occlusion_predictor
  * chore: rename traffic_light_classifier to autoware_traffic_light_classifier
  * chore: rename traffic_light_map_based_detector to autoware_traffic_light_map_based_detector
  * chore: rename traffic_light_visualization to autoware_traffic_light_visualization
  ---------
* refactor(lidar_apollo_instance_segmentation)!: fix namespace and directory structure (`#7995 <https://github.com/autowarefoundation/autoware.universe/issues/7995>`_)
  * refactor: add autoware namespace prefix
  * chore: update CODEOWNERS
  * refactor: add `autoware` prefix
  ---------
* refactor(image_projection_based_fusion)!: add package name prefix of autoware\_ (`#8162 <https://github.com/autowarefoundation/autoware.universe/issues/8162>`_)
  refactor: rename image_projection_based_fusion to autoware_image_projection_based_fusion
* refactor(compare_map_segmentation): add package name prefix of autoware\_ (`#8005 <https://github.com/autowarefoundation/autoware.universe/issues/8005>`_)
  * refactor(compare_map_segmentation): add package name prefix of autoware\_
  * docs: update Readme
  ---------
* refactor(shape_estimation): add package name prefix of autoware\_ (`#7999 <https://github.com/autowarefoundation/autoware.universe/issues/7999>`_)
  * refactor(shape_estimation): add package name prefix of autoware\_
  * style(pre-commit): autofix
  * fix: mising prefix
  * fix: cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(ground_segmentation)!: add package name prefix of autoware\_ (`#8135 <https://github.com/autowarefoundation/autoware.universe/issues/8135>`_)
  * refactor(ground_segmentation): add package name prefix of autoware\_
  * fix: update prefix cmake
  ---------
* refactor(lidar_centerpoint)!: fix namespace and directory structure (`#8049 <https://github.com/autowarefoundation/autoware.universe/issues/8049>`_)
  * add prefix in lidar_centerpoint
  * add .gitignore
  * change include package name in image_projection_based fusion
  * fix
  * change in codeowner
  * delete package
  * style(pre-commit): autofix
  * style(pre-commit): autofix
  * solve conflict too
  * fix include file
  * fix typo in launch file
  * add prefix in README
  * fix bugs by conflict
  * style(pre-commit): autofix
  * change namespace from  to
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(detected_object_validation)!: add package name prefix of autoware\_ (`#8122 <https://github.com/autowarefoundation/autoware.universe/issues/8122>`_)
  refactor: rename detected_object_validation to autoware_detected_object_validation
* refactor(detected_object_feature_remover)!: add package name prefix of autoware\_ (`#8127 <https://github.com/autowarefoundation/autoware.universe/issues/8127>`_)
  refactor(detected_object_feature_remover): add package name prefix of autoware\_
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
* refactor(traffic_light_visualization): fix namespace and directory structure (`#7968 <https://github.com/autowarefoundation/autoware.universe/issues/7968>`_)
  * feat: namespace fix and directory structure
  * chore: Remove main.cpp and implement node by template
  ---------
* refactor(traffic_light_fine_detector): fix namespace and directory structure (`#7973 <https://github.com/autowarefoundation/autoware.universe/issues/7973>`_)
  * refactor: add autoware on the namespace
  * refactor: rename nodelet to node
  ---------
* refactor(lidar_transfusion)!: fix namespace and directory structure (`#8022 <https://github.com/autowarefoundation/autoware.universe/issues/8022>`_)
  * add prefix
  * add prefix in code owner
  * style(pre-commit): autofix
  * fix launcher
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Amadeusz Szymko <amadeusz.szymko.2@tier4.jp>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(euclidean_cluster): add package name prefix of autoware\_ (`#8003 <https://github.com/autowarefoundation/autoware.universe/issues/8003>`_)
  * refactor(euclidean_cluster): add package name prefix of autoware\_
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(traffic_light_classifier): fix namespace and directory structure (`#7970 <https://github.com/autowarefoundation/autoware.universe/issues/7970>`_)
  * refactor: update namespace for traffic light classifier code
  * refactor: directory structure
  ---------
* fix(tier4_perception_launch): delete unnecessary dependency (`#8101 <https://github.com/autowarefoundation/autoware.universe/issues/8101>`_)
  delete cluster merger
* refactor(multi_object_tracker)!: add package name prefix of autoware\_ (`#8083 <https://github.com/autowarefoundation/autoware.universe/issues/8083>`_)
  * refactor: rename multi_object_tracker package to autoware_multi_object_tracker
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(autoware_tracking_object_merger): move headers to include/autoware and rename package (`#7809 <https://github.com/autowarefoundation/autoware.universe/issues/7809>`_)
* refactor(autoware_object_merger): move headers to src and rename package (`#7804 <https://github.com/autowarefoundation/autoware.universe/issues/7804>`_)
* refactor(detection_by_tracker): add package name prefix of autoware\_ (`#7998 <https://github.com/autowarefoundation/autoware.universe/issues/7998>`_)
* refactor(raindrop_cluster_filter): add package name prefix of autoware\_ (`#8000 <https://github.com/autowarefoundation/autoware.universe/issues/8000>`_)
  * refactor(raindrop_cluster_filter): add package name prefix of autoware\_
  * fix: typo
  ---------
* refactor(cluster_merger): add package name prefix of autoware\_ (`#8001 <https://github.com/autowarefoundation/autoware.universe/issues/8001>`_)
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
* refactor(compare_map_segmentation)!: fix namespace and directory structure (`#7910 <https://github.com/autowarefoundation/autoware.universe/issues/7910>`_)
  * feat: update namespace and directory structure for compare_map_segmentation code
  * refactor: update  directory structure
  * fix: add missing include
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add missing dependency (`#7919 <https://github.com/autowarefoundation/autoware.universe/issues/7919>`_)
  add raindrop_cluster_filter dependency
* feat: migrating pointcloud types (`#6996 <https://github.com/autowarefoundation/autoware.universe/issues/6996>`_)
  * feat: changed most of sensing to the new type
  * chore: started applying changes to the perception stack
  * feat: confirmed operation until centerpoint
  * feat: reverted to the original implementation of pointpainting
  * chore: forgot to push a header
  * feat: also implemented the changes for the subsample filters that were out of scope before
  * fix: some point type changes were missing from the latest merge from main
  * chore: removed unused code, added comments, and brought back a removed publish
  * chore: replaced pointcloud_raw for pointcloud_raw_ex to avoid extra processing time in the drivers
  * feat: added memory layout checks
  * chore: updated documentation regarding the point types
  * chore: added hyperlinks to the point definitions. will be valid only once the PR is merged
  * fix: fixed compilation due to moving the utilities files to the base library
  * chore: separated the utilities functions due to a dependency issue
  * chore: forgot that perception also uses the filter class
  * feature: adapted the undistortion tests to the new point type
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: badai nguyen <94814556+badai-nguyen@users.noreply.github.com>
* refactor(tier4_perception_launch): add maintainer to tier4_perception_launch (`#7893 <https://github.com/autowarefoundation/autoware.universe/issues/7893>`_)
  refactor: add maintainer to tier4_perception_launch
* feat(tier4_perception_launch): add image segmentation based pointcloud filter (`#7225 <https://github.com/autowarefoundation/autoware.universe/issues/7225>`_)
  * feat(tier4_perception_launch): add image segmentation based pointcloud filter
  * chore: typo
  * fix: detection launch
  * chore: add maintainer
  * Revert "chore: add maintainer"
  This reverts commit 5adfef6e9ca8196d3ba88ad574b2ba35489a5e49.
  ---------
* refactor(occupancy_grid_map_outlier_filter)!: fix namespace and directory structure (`#7748 <https://github.com/autowarefoundation/autoware.universe/issues/7748>`_)
  chore: update namespace and file structure
* refactor(ground_segmentation)!: fix namespace and directory structure (`#7744 <https://github.com/autowarefoundation/autoware.universe/issues/7744>`_)
  * refactor: update namespace in ground_segmentation files
  * refactor: update namespace in ground_segmentation files
  * refactor: update ground_segmentation namespace and file structure
  * style(pre-commit): autofix
  * refactor: update ground_segmentation plugin names scheme
  * refactor: update ransac tester
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ground_segmentation): fix bug  (`#7771 <https://github.com/autowarefoundation/autoware.universe/issues/7771>`_)
* feat(tier4_perception_launch): add missing arg use_multi_channel_tracker_merger (`#7705 <https://github.com/autowarefoundation/autoware.universe/issues/7705>`_)
  * feat(tier4_perception_launch): add missing arg use_multi_channel_tracker_merger
  * feat: add use_multi_channel_tracker_merger argument to simulator launch
  This commit adds the `use_multi_channel_tracker_merger` argument to the simulator launch file. The argument is set to `false` by default. This change enables the use of the multi-channel tracker merger in the simulator.
  ---------
* feat(tier4_perception_launch): enable multi channel tracker merger (`#7459 <https://github.com/autowarefoundation/autoware.universe/issues/7459>`_)
  * feat: introduce multi channel tracker merger
  feat: separate filters
  feat: filtering camera lidar fusion
  fix: object validator to modular
  fix: add missing config
  fix: radar only mode for both fusion mode
  fix
  style(pre-commit): autofix
  * fix: implement merger switching
  * chore: move pointcloud filter from detection to filter group
  * chore: define external and internal interfaces
  * fix: set output of camera-lidar in absolute path
  * chore: explicit object detection output
  * style(pre-commit): autofix
  * chore: update object detection input paths
  fix radar output
  * chore: update object detection input paths
  * fix: radar pipeline output
  * chore: update object detection input paths
  This commit updates the input paths for object detection. It ensures that the correct paths are used for the detection process.
  * style(pre-commit): autofix
  * fix: group to avoid argument mixture
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(tier4_perception_launch): perception launcher refactoring second round (`#7440 <https://github.com/autowarefoundation/autoware.universe/issues/7440>`_)
  * feat: separate filters
  * fix: object validator to modular
  * chore: remove default values from subsequent launch files
  * chore: group interfaces and junctions
  * Revert "chore: group interfaces and junctions"
  This reverts commit 9d723c33c260a9a0ac896bdf81c2a6ebeb981479.
  * chore: group interfaces and junctions
  * fix: radar input
  * fix: remove defaults from camera inputs
  * chore: rename camera args
  * chore: reorder
  * fix: remove defaults from lidar interface
  * Add use_pointcloud_map and use_validator arguments to detection.launch.xml
  * fix: remove default from validators and filters
  * fix: pointcloud container node name
  * style(pre-commit): autofix
  * Add use_low_intensity_cluster_filter argument to launch files
  * fix: on off detector and merger
  * fix: radar_far/objects default
  * fix: radar object filter parameter
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): autoupdate (`#7499 <https://github.com/autowarefoundation/autoware.universe/issues/7499>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* chore(tier4_perception_launch): perception launcher refactoring (`#7194 <https://github.com/autowarefoundation/autoware.universe/issues/7194>`_)
  * fix: reorder object merger launchers
  * fix: separate detection by tracker launch
  * fix: refactor tracking launch
  * style(pre-commit): autofix
  * fix: input pointcloud topic names, mot input channels
  * feat: separate filters
  * fix: object validator to modular
  * fix: implement filters on mergers
  * fix lidar only mode
  chore: simplify mode check
  * fix: fix a bug when use_radar_tracking_fusion is fault
  * fix: rename radar detector to filter
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): enable low_intensity_filter as default (`#7390 <https://github.com/autowarefoundation/autoware.universe/issues/7390>`_)
* refactor(crosswalk_traffic_light_estimator)!: add autoware\_ prefix (`#7365 <https://github.com/autowarefoundation/autoware.universe/issues/7365>`_)
  * add prefix
* chore(tier4_perception_launch): rename autoware_map_based_prediction_depend (`#7395 <https://github.com/autowarefoundation/autoware.universe/issues/7395>`_)
* refactor(map_based_prediction): prefix map based prediction (`#7391 <https://github.com/autowarefoundation/autoware.universe/issues/7391>`_)
* feat(lidar_transfusion): add lidar_transfusion 3D detection package (`#6890 <https://github.com/autowarefoundation/autoware.universe/issues/6890>`_)
  * feat(lidar_transfusion): add lidar_transfusion 3D detection package
  * style(pre-commit): autofix
  * style(lidar_transfusion): cpplint
  * style(lidar_transfusion): cspell
  * fix(lidar_transfusion): CUDA mem allocation & inference input
  * style(pre-commit): autofix
  * fix(lidar_transfusion): arrays size
  * style(pre-commit): autofix
  * chore(lidar_transfusion): update maintainers
  Co-authored-by: Satoshi Tanaka <16330533+scepter914@users.noreply.github.com>
  * fix(lidar_transfusion): array size & grid idx
  * chore(lidar_transfusion): update maintainer email
  * chore: added transfusion to the respective launchers
  * refactor(lidar_transfusion): rename config
  * refactor(lidar_transfusion): callback access specifier
  * refactor(lidar_transfusion): pointers initialziation
  * refactor(lidar_transfusion): change macros for constexpr
  * refactor(lidar_transfusion): consts & uniform initialization
  * refactor(lidar_transfusion): change to unique ptr & uniform initialization
  * style(pre-commit): autofix
  * refactor(lidar_transfusion): use of config params
  * refactor(lidar_transfusion): remove unnecessary condition
  * style(lidar_transfusion): switch naming (CPU to HOST)
  * refactor(lidar_transfusion): remove redundant device sync
  * style(lidar_transfusion): intensity naming
  * feat(lidar_transfusion): full network shape validation
  * feat(lidar_transfusion): validate objects' orientation in host processing
  * feat(lidar_transfusion): add json schema
  * style(pre-commit): autofix
  * style(lidar_transfusion): affine matrix naming
  * style(lidar_transfusion): transformed point naming
  * refactor(lidar_transfusion): add param descriptor & arrays size check
  * style(lidar_transfusion): affine matrix naming
  * feat(lidar_transfusion): caching cloud input as device ptr
  * fix(lidar_transfusion): logging
  * chore(tier4_perception_launch): revert to centerpoint
  * fix(lidar_transfusion): typo
  * docs(lidar_transfusion): use hook for param description
  * fix(lidar_transfusion): interpret eigen matrix as col major
  * feat(lidar_transfusion): update to autware_msgs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* feat!: replace autoware_auto_msgs with autoware_msgs for launch files (`#7242 <https://github.com/autowarefoundation/autoware.universe/issues/7242>`_)
  * feat!: replace autoware_auto_msgs with autoware_msgs for launch files
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  ---------
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* feat(multi_object_tracker): multi object input (`#6820 <https://github.com/autowarefoundation/autoware.universe/issues/6820>`_)
  * refactor: frequently used types, namespace
  * test: multiple inputs
  * feat: check latest measurement time
  * feat: define input manager class
  * feat: interval measures
  * feat: store and sort inputs PoC
  * chore: rename classes
  * feat: object collector
  * impl input manager, no subscribe
  * fix: subscribe and trigger callback
  * fix: subscriber and callbacks are working
  * fix: callback object is fixed, tracker is working
  * fix: get object time argument revise
  * feat: back to periodic publish, analyze input latency and timings
  * fix: enable timing debugger
  * fix: separate object interval function
  * feat: prepare message triggered process
  * feat: trigger tracker by main message arrive
  * chore: clean-up, set namespace
  * feat: object lists with detector index
  * feat: define input channel struct
  * fix: define type for object list
  * feat: add channel wise existence probability
  * fix: relocate debugger
  * fix: total existence logic change
  * feat: publishing object debug info, need to fix marker id
  * feat: indexing marker step 1
  * fix: uuid management
  * feat: association line fix
  * feat: print channel names
  * feat: association lines are color-coded
  * fix: association debug marker bugfix
  * style(pre-commit): autofix
  * feat: add option for debug marker
  * feat: skip time statistics update in case of outlier
  * feat: auto-tune latency band
  * feat: pre-defined channels, select on launcher
  * feat: add input channels
  * fix: remove marker idx map
  * fix: to do not miss the latest message of the target stream
  * fix: remove priority, separate timing optimization
  * fix: time interval bug fix
  * chore: refactoring timing state update
  * fix: set parameters optionally
  * feat: revise object time range logic
  * fix: launcher to set input channels
  * fix: exempt spell check 'pointpainting'
  * feat: remove expected interval
  * feat: implement spawn switch
  * fix: remove debug messages
  * chore: update readme
  * fix: change tentative object topic
  * Revert "fix: remove debug messages"
  This reverts commit 725a49ee6c382f73b54fe50bf9077aca6049e199.
  * fix: reset times when jumps to past
  * fix: check if interval is negative
  * fix: missing config, default value
  * fix: remove debug messages
  * fix: change no-object message level
  * Update perception/multi_object_tracker/include/multi_object_tracker/debugger/debug_object.hpp
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  * chore: Update copyright to uppercase
  * chore: fix readme links to config files
  * chore: move and rename uuid functions
  * chore: fix debug topic to use node name
  * chore: express meaning of threshold
  * feat: revise decay rate, update function
  * fix: define constants with explanation
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* feat(tier4_perception_launch): fix typo error (`#6999 <https://github.com/autowarefoundation/autoware.universe/issues/6999>`_)
  * feat: downsample perception input pointcloud
  * fix: add group if to switch downsample node
  * fix: add test and exec depend
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * chore: refactor perception.launch.xml
  * fix: fix name
  ---------
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* feat(tier4_perception_launch): downsample perception input pointcloud (`#6886 <https://github.com/autowarefoundation/autoware.universe/issues/6886>`_)
  * feat: downsample perception input pointcloud
  * fix: add group if to switch downsample node
  * fix: add test and exec depend
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * chore: refactor perception.launch.xml
  ---------
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* feat: add low_intensity_cluster_filter (`#6850 <https://github.com/autowarefoundation/autoware.universe/issues/6850>`_)
  * feat: add low_intensity_cluster_filter
  * chore: typo
  * fix: build test error
  ---------
* fix(voxel_grid_downsample_filter): add intensity field (`#6849 <https://github.com/autowarefoundation/autoware.universe/issues/6849>`_)
  fix(downsample_filter): add intensity field
* fix(lidar_centerpoint): add param file for centerpoint_tiny (`#6901 <https://github.com/autowarefoundation/autoware.universe/issues/6901>`_)
* refactor(centerpoint, pointpainting): rearrange parameters for ML models and packages (`#6591 <https://github.com/autowarefoundation/autoware.universe/issues/6591>`_)
  * refactor: lidar_centerpoint
  * refactor: pointpainting
  * chore: fix launch
  * chore: fix launch
  * chore: rearrange params
  * fix: json-schema-check error
  * fix: default param
  * refactor: rename param file
  * chore: typo
  * fix: align centerpoint param namespace with pointpainting
  * fix(centerpoint): add schema json
  * fix(pointpainting): fix schema json typo
  * style(pre-commit): autofix
  * docs: update pointpainting fusion doc
  * docs: update lidar centerpoint doc
  * fix: change omp param
  * fix:change twist and variance to model params
  * fix: keep build_only in launch
  * fix: schema check
  * chore: temporary remove schema required
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): change traffic light recognition pipeline (`#6879 <https://github.com/autowarefoundation/autoware.universe/issues/6879>`_)
  style(pre-commit): autofix
  refactor: topic name
* feat(perception_online_evaluator): add use_perception_online_evaluator option and disable it by default (`#6861 <https://github.com/autowarefoundation/autoware.universe/issues/6861>`_)
* feat(lidar_centerpoint): output the covariance of pose and twist (`#6573 <https://github.com/autowarefoundation/autoware.universe/issues/6573>`_)
  * feat: postprocess variance
  * feat: output variance
  * feat: add has_variance to config
  * fix: single_inference node
  * style(pre-commit): autofix
  * fix: add to pointpainting param
  * Update perception/lidar_centerpoint/src/node.cpp
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
  * Update perception/image_projection_based_fusion/src/pointpainting_fusion/node.cpp
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
  * Update perception/lidar_centerpoint/src/node.cpp
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
  * fix: add options
  * fix: avoid powf
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/detector/lidar_dnn_detector.launch.xml
  Co-authored-by: Taekjin LEE <technolojin@gmail.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
  Co-authored-by: Taekjin LEE <technolojin@gmail.com>
* fix(ground_segmentation launch): fix topic name conflict in additional_lidars option (`#6801 <https://github.com/autowarefoundation/autoware.universe/issues/6801>`_)
  fix(ground_segmentation launch): fix topic name conflict when using additional lidars
* Contributors: Amadeusz Szymko, Esteve Fernandez, Kenzo Lobos Tsunekawa, Kosuke Takeuchi, Kotaro Uetake, Mamoru Sobue, Manato Hirabayashi, Masato Saeki, Mehmet Emin BAŞOĞLU, Ryohsuke Mitsudome, Shunsuke Miura, Taekjin LEE, Tao Zhong, Yoshi Ri, Yuki TAKAGI, Yutaka Kondo, awf-autoware-bot[bot], badai nguyen, oguzkaganozt

0.26.0 (2024-04-03)
-------------------
* feat(probabilistic_occupancy_grid_map): add synchronized ogm fusion node (`#5485 <https://github.com/autowarefoundation/autoware.universe/issues/5485>`_)
  * add synchronized ogm fusion node
  * add launch test for grid map fusion node
  * fix test cases input msg error
  * change default fusion parameter
  * rename parameter for ogm fusion
  * feat: add multi_lidar_ogm generation method
  * enable ogm creation launcher in tier4_perception_launch to call multi_lidar ogm creation
  * fix: change ogm fusion node pub policy to reliable
  * fix: fix to use lidar frame as scan frame
  * fix: launcher node
  * feat: update param name
  * chore: fix ogm pointcloud subscription
  * feat: enable to publish pipeline latency
  ---------
* chore(ground_segmentation_launch): change max_z of cropbox filter to vehicle_height (`#6549 <https://github.com/autowarefoundation/autoware.universe/issues/6549>`_)
  * chore(ground_segmentation_launch): change max_z of cropbox filter to vehicle_height
  * fix: typo
  ---------
* chore(ground_segmentation): rename topic and node (`#6536 <https://github.com/autowarefoundation/autoware.universe/issues/6536>`_)
  * chore(ground_segmentation): rename topic and node
  * docs: update synchronized_grid_map_fusion
  ---------
* feat(perception_online_evaluator): add perception_online_evaluator (`#6493 <https://github.com/autowarefoundation/autoware.universe/issues/6493>`_)
  * feat(perception_evaluator): add perception_evaluator
  tmp
  update
  add
  add
  add
  update
  clean up
  change time horizon
  * fix build werror
  * fix topic name
  * clean up
  * rename to perception_online_evaluator
  * refactor: remove timer
  * feat: add test
  * fix: ci check
  ---------
* chore(image_projection_based_fusion): rename debug topics (`#6418 <https://github.com/autowarefoundation/autoware.universe/issues/6418>`_)
  * chore(image_projection_based_fusion): rename debug topics
  * style(pre-commit): autofix
  * fix: roi_pointcloud_fusion namespace
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: remove `tensorrt_yolo` from package dependencies in launcher (`#6377 <https://github.com/autowarefoundation/autoware.universe/issues/6377>`_)
* chore(traffic_light_map_based_detector): rework parameters (`#6200 <https://github.com/autowarefoundation/autoware.universe/issues/6200>`_)
  * chore: use config
  * chore: use config
  * fix: revert min_timestamp_offset
  * fix: revert min_timestamp_offset
  * fix: delete param
  * style(pre-commit): autofix
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  * revert: revert change in min&max timestamp offset
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: kminoda <koji.minoda@tier4.jp>
* feat(tensorrt_yolo): remove package (`#6361 <https://github.com/autowarefoundation/autoware.universe/issues/6361>`_)
  * feat(tensorrt_yolo): remove package
  * remove tensorrt_yolo inclusion
  * feat: add multiple yolox launcher
  ---------
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
* chore(traffic_light_fine_detector_and_classifier): rework parameters (`#6216 <https://github.com/autowarefoundation/autoware.universe/issues/6216>`_)
  * chore: use config
  * style(pre-commit): autofix
  * chore: move build only back
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(object_merger): rework parameters (`#6160 <https://github.com/autowarefoundation/autoware.universe/issues/6160>`_)
  * chore(object_merger): parametrize some parameters
  * style(pre-commit): autofix
  * revert priority_mode
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(radar_object_tracker): move tracker config directory to parameter yaml (`#6250 <https://github.com/autowarefoundation/autoware.universe/issues/6250>`_)
  * chore: move tracker config directory to parameter yaml
  * fix: add allow_substs to fix error
  * fix: use radar tracking parameter from autoware_launch
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* feat: remove use_pointcloud_container (`#6115 <https://github.com/autowarefoundation/autoware.universe/issues/6115>`_)
  * feat!: remove use_pointcloud_container
  * fix pre-commit
  * fix: completely remove use_pointcloud_container after merge main
  * fix: set use_pointcloud_container = true
  * revert: revert change in probabilistic_occupancy_grid_map
  * revert change in launcher of ogm
  ---------
* chore(lidar_centerpoint): rework parameters (`#6167 <https://github.com/autowarefoundation/autoware.universe/issues/6167>`_)
  * chore(lidar_centerpoint): use config
  * revert unnecessary fix
  * fix: revert build_only option
  * docs: update readme
  * style(pre-commit): autofix
  * fix: add pr url
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* feat(detection): add container option (`#6228 <https://github.com/autowarefoundation/autoware.universe/issues/6228>`_)
  * feat(lidar_centerpoint,image_projection_based_fusion): add pointcloud_container option
  * revert lidar_perception_model
  * style(pre-commit): autofix
  * fix: add options
  * fix: fix default param
  * update node name
  * fix: fix IfCondition
  * fix pointpainting namespace
  * fix: fix launch args
  * fix(euclidean_cluster): do not launch individual container when use_pointcloud_container is true
  * fix(euclidean_cluster): fix launch condition
  * fix(euclidean_cluster): fix launch condition
  * Update perception/lidar_centerpoint/launch/lidar_centerpoint.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* chore(tier4_perception_launch): fix arg name radar lanelet filter (`#6215 <https://github.com/autowarefoundation/autoware.universe/issues/6215>`_)
* chore(radar_crossing_objects_noise_filter): add config file (`#6210 <https://github.com/autowarefoundation/autoware.universe/issues/6210>`_)
  * chore(radar_crossing_objects_noise_filter): add config file
  * bug fix
  * merge main branch
  ---------
* chore(radar_object_clustering): fix config arg name (`#6214 <https://github.com/autowarefoundation/autoware.universe/issues/6214>`_)
* chore(object_velocity_splitter): rework parameters (`#6158 <https://github.com/autowarefoundation/autoware.universe/issues/6158>`_)
  * chore(object_velocity_splitter): add param file
  * fix
  * fix arg name
  * fix: update launch param handling
  ---------
* fix(tier4_perception_launch): fix a bug in `#6159 <https://github.com/autowarefoundation/autoware.universe/issues/6159>`_ (`#6203 <https://github.com/autowarefoundation/autoware.universe/issues/6203>`_)
* chore(object_range_splitter): rework parameters (`#6159 <https://github.com/autowarefoundation/autoware.universe/issues/6159>`_)
  * chore(object_range_splitter): add param file
  * fix arg name
  * feat: use param file from autoware.launch
  ---------
* refactor(tier4_perception_launch): refactor object_recognition/detection launcher  (`#6152 <https://github.com/autowarefoundation/autoware.universe/issues/6152>`_)
  * refactor: align mode parameters
  * refactor: cluster detector and merger
  * refactor: separate object merger launches
  * refactor: radar detector module
  * refactor: lidar detector modules
  * chore: fix mis spell, align typo, clean-up
  ---------
* chore(pointcloud_container): move glog_component to autoware_launch (`#6114 <https://github.com/autowarefoundation/autoware.universe/issues/6114>`_)
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
* fix(pointpainting): fix param path declaration (`#6106 <https://github.com/autowarefoundation/autoware.universe/issues/6106>`_)
  * fix(pointpainting): fix param path declaration
  * remove pointpainting_model_name
  * revert: revert unnecessary change
  ---------
* fix(image_projection_based_fusion): re-organize the parameters for image projection fusion (`#6075 <https://github.com/autowarefoundation/autoware.universe/issues/6075>`_)
  re-organize the parameters for image projection fusion
* feat(probabilistic_occupancy_grid_map): add grid map fusion node (`#5993 <https://github.com/autowarefoundation/autoware.universe/issues/5993>`_)
  * add synchronized ogm fusion node
  * add launch test for grid map fusion node
  * fix test cases input msg error
  * change default fusion parameter
  * rename parameter for ogm fusion
  * feat: add multi_lidar_ogm generation method
  * enable ogm creation launcher in tier4_perception_launch to call multi_lidar ogm creation
  * fix: change ogm fusion node pub policy to reliable
  * chore: remove files outof scope with divied PR
  ---------
* feat(crosswalk_traffic_light): add detector and classifier for pedestrian traffic light  (`#5871 <https://github.com/autowarefoundation/autoware.universe/issues/5871>`_)
  * add: crosswalk traffic light recognition
  * fix: set conf=0 when occluded
  * fix: clean code
  * fix: refactor
  * fix: occlusion predictor
  * fix: output not detected signals as unknown
  * Revert "fix: output not detected signals as unknown"
  This reverts commit 7a166596e760d7eb037570e28106dcd105860567.
  * Revert "fix: occlusion predictor"
  This reverts commit 47d8cdd7fee8b4432f7a440f87bc35b50a8bc897.
  * fix: occlusion predictor
  * fix: clean debug code
  * style(pre-commit): autofix
  * fix: launch file
  * fix: set max angle range for different type
  * fix: precommit
  * fix: cancel the judge of flashing for estimated crosswalk traffic light
  * delete: not necessary judgement on label
  * Update perception/traffic_light_classifier/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/include/crosswalk_traffic_light_estimator/node.hpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * style(pre-commit): autofix
  * fix: topic names and message attribute name
  * style(pre-commit): autofix
  * fix: model names
  * style(pre-commit): autofix
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * fix: argument position
  * fix: set classifier type in launch file
  * fix: function and parameter name
  * fix: func name
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_map_based_detector/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * style(pre-commit): autofix
  * fix: move max angle range to config
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * fix: model name
  * fix: conflict
  * fix: precommit
  * fix: CI test
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
* feat: add support of overwriting signals if harsh backlight is detected (`#5852 <https://github.com/autowarefoundation/autoware.universe/issues/5852>`_)
  * feat: add support of overwriting signals if backlit is detected
  * feat: remove default parameter in nodelet and update lauch for composable node
  * docs: update README
  * docs: update README
  * feat: update confidence to 0.0 corresponding signals overwritten by unkonwn
  ---------
* chore: add glog_component for pointcloud_container (`#5716 <https://github.com/autowarefoundation/autoware.universe/issues/5716>`_)
* refactor(localization_launch, ground_segmentation_launch): rename lidar topic (`#5781 <https://github.com/autowarefoundation/autoware.universe/issues/5781>`_)
  rename lidar topic
  Co-authored-by: yamato-ando <Yamato ANDO>
* fix: add missing param on perception launch: (`#5812 <https://github.com/autowarefoundation/autoware.universe/issues/5812>`_)
  detection_by_tracker_param_path was missing
* refactor(multi_object_tracker): put node parameters to yaml file (`#5769 <https://github.com/autowarefoundation/autoware.universe/issues/5769>`_)
  * rework multi object tracker parameters
  * update README
  * rework radar tracker parameter too
  ---------
* refactor(tier4_perception_launch): refactor perception launcher (`#5630 <https://github.com/autowarefoundation/autoware.universe/issues/5630>`_)
* chore(tier4_perception_launcher): remove launch parameter default of detection_by_tracker (`#5664 <https://github.com/autowarefoundation/autoware.universe/issues/5664>`_)
  * chore(tier4_perception_launcher): remove launch parameter default
  * chore: typo
  ---------
* feat(radar_object_tracker): Change to use `use_radar_tracking_fusion` as true (`#5605 <https://github.com/autowarefoundation/autoware.universe/issues/5605>`_)
* refactor(radar_object_clustering): move radar object clustering parameter to param file (`#5451 <https://github.com/autowarefoundation/autoware.universe/issues/5451>`_)
  * move radar object clustering parameter to param file
  * remove default parameter settings and fix cmakelists
  ---------
* build(tier4_perception_launch): add tracking_object_merger (`#5602 <https://github.com/autowarefoundation/autoware.universe/issues/5602>`_)
* fix(detection_by_tracker): add ignore option for each label (`#5473 <https://github.com/autowarefoundation/autoware.universe/issues/5473>`_)
  * fix(detection_by_tracker): add ignore for each class
  * fix: launch
  ---------
* feat(tier4_perception_launch): add parameter to control detection_by_tracker on/off (`#5313 <https://github.com/autowarefoundation/autoware.universe/issues/5313>`_)
  * add parameter to control detection_by_tracker on/off
  * style(pre-commit): autofix
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* fix(tracking_object_merger): fix unintended error in radar tracking merger (`#5328 <https://github.com/autowarefoundation/autoware.universe/issues/5328>`_)
  * fix: fix tracking merger node
  * fix: unintended condition setting
  ---------
* feat(tier4_perception_launch): add radar far object integration in tracking stage (`#5269 <https://github.com/autowarefoundation/autoware.universe/issues/5269>`_)
  * update tracking/perception launch
  * switch tracker launcher mode with argument
  * update prediction to switch by radar_long_range_integration paramter
  * make radar far object integration switchable between detection/tracking
  * fix camera lidar radar fusion flow when 'tracking' is used.
  * fix spelling and appearance
  * reconstruct topic flow when use tracking to merge far object detection and near object detection
  * fix input topic miss in tracking.launch
  * fix comment in camera_lidar_radar fusion
  * refactor: rename and remove paramters in prediction.launch
  * refactor: rename merger control variable from string to bool
  ---------
* fix(image_projection_based_fusion): add iou_x use in long range for roi_cluster_fusion (`#5148 <https://github.com/autowarefoundation/autoware.universe/issues/5148>`_)
  * fix: add iou_x for long range obj
  * fix: add launch file param
  * chore: fix unexpect calc iou in long range
  * fix: multi iou usable
  * chore: typo
  * docs: update readme
  * chore: refactor
  ---------
* fix(tier4_perception_launch): fix faraway detection to reduce calculation cost (`#5233 <https://github.com/autowarefoundation/autoware.universe/issues/5233>`_)
  * fix(tier4_perception_launch): fix node order in radar_based_detection.launch
  * fix comment out unused node
  ---------
* fix(detected_object_validation): change the points_num of the validator to be set class by class (`#5177 <https://github.com/autowarefoundation/autoware.universe/issues/5177>`_)
  * fix: add param for each object class
  * fix: add missing classes param
  * fix: launch file
  * fix: typo
  * chore: refactor
  ---------
* feat(perception_launch): add data_path arg to perception launch (`#5069 <https://github.com/autowarefoundation/autoware.universe/issues/5069>`_)
  * feat(perception_launch): add var data_path to perception.launch
  * feat(perception_launch): update default center_point_model_path
  ---------
* fix(tier4_perception_launch): add parameters for light weight radar fusion and fix launch order (`#5166 <https://github.com/autowarefoundation/autoware.universe/issues/5166>`_)
  * fix(tier4_perception_launch): add parameters for light weight radar fusion and fix launch order
  * style(pre-commit): autofix
  * add far_object_merger_sync_queue_size param for package arg
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(pointcloud_preprocessor): organize input twist topic (`#5125 <https://github.com/autowarefoundation/autoware.universe/issues/5125>`_)
  * fix(pointcloud_preprocessor): organize input twist topic (`#25 <https://github.com/autowarefoundation/autoware.universe/issues/25>`_)
  * fix(pointcloud_preprocessor): organize input twist topic
  * style(pre-commit): autofix
  * fix build bug
  * fix format error
  * style(pre-commit): autofix
  * fix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * minor fixes
  * style(pre-commit): autofix
  * add warning
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): add object_merger of far_objects to fusion for Camera-LiDAR-Radar fusion (`#5026 <https://github.com/autowarefoundation/autoware.universe/issues/5026>`_)
  * fix(tier4_perception_launch): add object_merger of far_objects to fusion for Camera-LiDAR-Radar fusion
  * fix conflict
  ---------
* refactor(perception): rearrange clustering pipeline (`#4999 <https://github.com/autowarefoundation/autoware.universe/issues/4999>`_)
  * fix: change downsample filter
  * fix: remove downsamle after compare map
  * fix: add low range cropbox
  * refactor: use_pointcloud_map
  * chore: refactor
  * fix: add roi based clustering option
  * chore: change node name
  * fix: launch argument pasrer
  ---------
* fix(tier4_perception_launch): camera lidar fusion launch (`#4983 <https://github.com/autowarefoundation/autoware.universe/issues/4983>`_)
  fix: camera lidar fusion launch
* feat(image_projection_based_fusion): add roi based clustering for small unknown object detection (`#4681 <https://github.com/autowarefoundation/autoware.universe/issues/4681>`_)
  * feat: add roi_pointcloud_fusion node
  fix: postprocess
  fix: launch file
  chores: refactor
  fix: closest cluster
  * chores: refactor
  * docs: add readme
  * fix: add missed parameter declare
  * fix: add center transform
  * fix: typos in launch
  * docs: update docs
  * fix: change roi pointcloud fusion output to clusters
  * fix: add cluster debug roi pointcloud fusion
  * fix: use IoU_x in roi cluster fusion
  * feat: add cluster merger package
  * fix: camera lidar launch
  * style(pre-commit): autofix
  * fix: cluster merger
  * fix: roi cluster fusion unknown object fix
  * chore: typo
  * docs: add readme cluster_merger
  * docs: update roi pointcloud fusion readme
  * chore: typo
  * fix: multiple definition bug
  * chore: refactor
  * docs: update docs
  * chore: refactor
  * chore: pre-commit
  * fix: update camera_lidar_radar mode launch
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(crosswalk_traffic_light_estimator): rework parameters (`#4699 <https://github.com/autowarefoundation/autoware.universe/issues/4699>`_)
  * refactor the configuration files of the node crosswalk_traffic_light_estimator according to the new ROS node config guideline.
  update the parameter information in the README.md
  * style(pre-commit): autofix
  * fix the xml pre-check issue
  * delete the xml declaration to fix the xml pre-check issue
  * Modify the CMakeLists.txt file to enalbe /config directory sharing when building the package.
  * Update the bound for schema file.
  * add crosswalk_traffic_light_estimator_param_file to traffic_light.launch.xml
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
* fix(crosswalk_traffic_light_estimator): move crosswalk after fusion (`#4734 <https://github.com/autowarefoundation/autoware.universe/issues/4734>`_)
  * fix: move crosswalk after fusion
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  * Rename TrafficLight to TrafficSignal
  * change input to be considered as the regulatory-element
  ---------
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
* chore: add TLR model args to launch files (`#4805 <https://github.com/autowarefoundation/autoware.universe/issues/4805>`_)
* fix(tier4_percetion_launch): fix order of Camera-Lidar-Radar fusion pipeline (`#4779 <https://github.com/autowarefoundation/autoware.universe/issues/4779>`_)
  * fix(tier4_percetion_launch): fix order of Camera-Lidar-Radar fusion pipeline
  * fix clustering update
  * fix from Camera-LidAR fusion
  * refactor
  * refactor
  * fix merge
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/camera_lidar_radar_fusion_based_detection.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(launch): add missing launch args and defaults to lidar_based_detection.launch.xml (`#4596 <https://github.com/autowarefoundation/autoware.universe/issues/4596>`_)
  * Update lidar_based_detection.launch.xml
  Some launch arguments were missing. These arguments and their defaults were added.
  * changed default of objects_filter_method
  changed default of the "objects_filter_method" to "lanelet_filter" as requested.
  ---------
* feat(tier4_perception_launch): lower the detection by tracker priority to suppress yaw oscillation (`#4690 <https://github.com/autowarefoundation/autoware.universe/issues/4690>`_)
  lower the detection by tracker priority to suppress yaw oscillation
* feat(image_projection_based_fusion): add objects filter by rois (`#4546 <https://github.com/autowarefoundation/autoware.universe/issues/4546>`_)
  * tmp
  style(pre-commit): autofix
  update
  style(pre-commit): autofix
  * fix: fix association bug
  * feat: add prob_threshold for each class
  * feat: use class label association between roi and object
  * feat: add to tier4_perception_launch
  * chore: disable debug_mode
  * docs: update params
  * fix: apply suggestion
  * chore: update prob_thresholds of bicycle
  * feat: add thresut_distance for each class
  * docs: add thrust_distances
  * style(pre-commit): autofix
  * chore: remove unnecessary variable
  * chore: rename to trust
  * style(pre-commit): autofix
  * chore: add param
  * Update perception/image_projection_based_fusion/config/roi_detected_object_fusion.param.yaml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* refactor(detected_object_validation): add an option for filtering and validation (`#4402 <https://github.com/autowarefoundation/autoware.universe/issues/4402>`_)
  * init commit
  * update occupancy_grid_map path
  * update argument names
  * correct radar launch objects_filter_method name
  * remove radar option
  ---------
* refactor(traffic_light_arbiter): read parameters from config file (`#4454 <https://github.com/autowarefoundation/autoware.universe/issues/4454>`_)
* fix(compare_map_segmentation): change to using kinematic_state topic (`#4448 <https://github.com/autowarefoundation/autoware.universe/issues/4448>`_)
* chore(tier4_perception_launch): fix typo (`#4406 <https://github.com/autowarefoundation/autoware.universe/issues/4406>`_)
  * fix(tier4_perception_launch): fix typo
  * fix typo
  ---------
* fix(traffic_light): fix traffic_light_arbiter pipeline (`#4393 <https://github.com/autowarefoundation/autoware.universe/issues/4393>`_)
  * fix(traffic_light): fix traffic_light_arbiter pipeline
  * style(pre-commit): autofix
  * fix: output topic name
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(euclidean_cluster): add disuse downsample in clustering pipeline (`#4385 <https://github.com/autowarefoundation/autoware.universe/issues/4385>`_)
  * fix: add unuse downsample launch option
  * fix: add default param for downsample option
  * fix typo
  ---------
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
* fix(compare_map_segmentation): add option to reduce distance_threshold in z axis (`#4243 <https://github.com/autowarefoundation/autoware.universe/issues/4243>`_)
  * fix(compare_map_segmentation): keep low level pointcloud
  * fix: add option to compare lower neighbor points
  * docs: readme update
  * fix: add param to launch
  * Revert "fix(compare_map_segmentation): keep low level pointcloud"
  This reverts commit eb07f954a7ca26a558c211a7a195d73147d5784c.
  * fix: reduce z distance of low level neighbor point
  * fix: reduce voxel leaf size in z axis
  * fix: change param type
  ---------
* refactor(image_projection_based_fusion): update rois topic names definitions (`#4356 <https://github.com/autowarefoundation/autoware.universe/issues/4356>`_)
* refactor(image_projection_based_fusion): read lidar models parameters from autoware_launch (`#4278 <https://github.com/autowarefoundation/autoware.universe/issues/4278>`_)
  * init commit
  * add centerpoints param
  * add detection_class_remapper.param.yaml
  * remove unused centerpoint param path
  ---------
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
* feat(tier4_perception_launch): add radar tracking node to launcher (`#4361 <https://github.com/autowarefoundation/autoware.universe/issues/4361>`_)
  * update tracking/perception launch
  * switch tracker launcher mode with argument
  * add radar tracker dependency
  ---------
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* feat(tier4_perception_launch): add radar faraway detection  (`#4330 <https://github.com/autowarefoundation/autoware.universe/issues/4330>`_)
  * feat(tier4_perception_launch): add radar faraway detection
  * apply pre-commit
  * fix unused param
  * rename launch name
  * add exec depends
  ---------
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
* refactor(object_merger): read parameters from autoware_launch (`#4339 <https://github.com/autowarefoundation/autoware.universe/issues/4339>`_)
  init commit
* refactor(map_based_prediction): read parameters from autoware_launch (`#4337 <https://github.com/autowarefoundation/autoware.universe/issues/4337>`_)
  init commit
* refactor(euclidean clustering): read parameters from autoware_launch (`#4262 <https://github.com/autowarefoundation/autoware.universe/issues/4262>`_)
  * update clustering param path
  * update param paths
  * style(pre-commit): autofix
  * add missing parameter paths
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: separate traffic_light_utils from perception_utils (`#4207 <https://github.com/autowarefoundation/autoware.universe/issues/4207>`_)
  * separate traffic_light_utils from perception_utils
  * style(pre-commit): autofix
  * fix namespace bug
  * remove unnecessary dependency
  * rename rest of perception_utils to object_recognition_utils
  * fix bug
  * rename for added radar_object_clustering
  * delete redundant namespace
  * Update common/perception_utils/include/perception_utils/prime_synchronizer.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Correct the failure in the previous merge.
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* feat(tier4_perception_launch): update traffic light launch (`#4176 <https://github.com/autowarefoundation/autoware.universe/issues/4176>`_)
  * first commit
  * add image number arg
  * style(pre-commit): autofix
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  * Update launch/tier4_perception_launch/launch/traffic_light_recognition/traffic_light.launch.xml
  * add traffic light namespace to fusion
  * add tlr fusion only mode and camera number arg
  * change to include traffic_light_arbiter launch
  * delete relay topic type
  ---------
  Co-authored-by: Shunsuke Miura <shunsuke.miura@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* feat(traffic_light): improved traffic_light_map_based_detector and new traffic_light_fine_detector package (`#4084 <https://github.com/autowarefoundation/autoware.universe/issues/4084>`_)
  * update traffic_light_map_based_detector traffic_light_classifier traffic_light_fine_detector traffic_light_multi_camera_fusion
  * replace autoware_auto_perception_msgs with tier4_perception_msgs
  ---------
* refactor(occpuancy grid map): move param to yaml (`#4038 <https://github.com/autowarefoundation/autoware.universe/issues/4038>`_)
* fix(tier4_perception_launch): fix camera_lidar_radar_fusion_based_detection (`#3950 <https://github.com/autowarefoundation/autoware.universe/issues/3950>`_)
  * fix: launch arguments
  * chore: revert arg
  ---------
* fix(tier4_perception_launch): sync param path (`#3713 <https://github.com/autowarefoundation/autoware.universe/issues/3713>`_)
  * fix(tier4_perception_launch):modify sync_param_path reading method
  * fix(tier4_perception_launch): fix image_number used for testing
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): fix image_number description (`#3686 <https://github.com/autowarefoundation/autoware.universe/issues/3686>`_)
* feat(traffic_light_ssd_fine_detector): add support of ssd trained by mmdetection (`#3485 <https://github.com/autowarefoundation/autoware.universe/issues/3485>`_)
  * feat: update to allow out-of-order for scores and boxes
  * feat: add GatherTopk plugin
  * feat: add GridPriors plugin
  * feat: update interface
  * docs: update document
  * feat: update parameter names
  * fix: resolve to normalize output boxes
  * refactor: refactoring paramters
  * chore: update Tier IV to TIER IV
  * feat: update launch parameter to dnn_header_type
  * feat: update to use getTensorShape
  * remove unused params
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  ---------
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* fix(tier4_perception_launch): fix duplicated topic name (`#3645 <https://github.com/autowarefoundation/autoware.universe/issues/3645>`_)
  * fix(tier4_perception_launch): fix dublicated topic name
  * chore: rename topic
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(compare_map_segmentation): update voxel_based for dynamic map loader with map grid coordinate (`#3277 <https://github.com/autowarefoundation/autoware.universe/issues/3277>`_)
  * fix: change map grid searching
  * refactoring
  * fix: reload map after initilization
  * fix: check point on map grid boundary
  * refactoring
  * refactorng
  * refactoring
  * chore: remove unuse header
  * fix: use initialization_state through component interface
  * fix: add metadata into pointcloud map cell
  * chore: update debug param
  * fix: using localization interface
  * fix: add launch missing param
  * fix: deprecated component interface declaration
  * chore: typo
  * docs: correct parameter description
  ---------
* refactor(occupancy_grid_map): add occupancy_grid_map method/param var to launcher (`#3393 <https://github.com/autowarefoundation/autoware.universe/issues/3393>`_)
  * add occcupancy_grid_map method/param var to launcher
  * added CODEOWNER
  * Revert "added CODEOWNER"
  This reverts commit 2213c2956af19580d0a7788680aab321675aab3b.
  * add maintainer
  ---------
* feat(elevation_map_loader): add support for seleceted_map_loader (`#3344 <https://github.com/autowarefoundation/autoware.universe/issues/3344>`_)
  * feat(elevation_map_loader): add support for sequential_map_loading
  * fix(elevation_map_loader): fix bug
  * feat(elevation_map_loader): make it possible to adjust the number of PCD maps loaded at once when using sequential map loading
  * feat(elevation_map_loader): change default value of use_lane_filter as false
  * fix(elevation_map_loader): fix typo
  * refactor(elevation_map_loader): Add a range of param. And refactor receiveMap.
  * feat(elevation_map_loader): Change info level log into debug level log with throttle. And remove abbreviation
  ---------
* feat(tier4_perception_launch): refactored occupancy_grid_map launcher (`#3058 <https://github.com/autowarefoundation/autoware.universe/issues/3058>`_)
  * rebase on to master
  add scan_frame and raytrace center
  * rebase to main
  * fix config and launch file
  * fixed laserscan based launcher
  * add filter func to extract obstacle pc in sensor
  * add switchable launcher
  * back to pointcloud based method
  and fix missing }
  * remove unused launch.py
  * fix: fix and refactor launch.py
  * document: update README
  * enable to change origins by lanch args
  ---------
* chore(tier4_perception_launch): add custom parameters for roi_cluster_fusion (`#3281 <https://github.com/autowarefoundation/autoware.universe/issues/3281>`_)
* fix(tier4_perception_launch): add missing parameter for voxel based compare map filter (`#3251 <https://github.com/autowarefoundation/autoware.universe/issues/3251>`_)
* feat(compare_map_segmentation): add dynamic map loading for voxel_based_compare_map_filter (`#3087 <https://github.com/autowarefoundation/autoware.universe/issues/3087>`_)
  * feat: add interface to dynamic loader
  * refactor: refactoring
  * refactor: refactoring
  * refactor: refactoring
  * docs: update readme
  * chore: add default param and todo
  * chore: typo
  * chore: typo
  * fix: remove unnecessary neighbor voxels calculation
  * fix: add neighbor map_cell checking
  * fix: neighbor map grid check
  ---------
* feat(elevation_map_loader): use polygon iterator to speed up (`#2885 <https://github.com/autowarefoundation/autoware.universe/issues/2885>`_)
  * use grid_map::PolygonIterator instead of grid_map::GridMapIterator
  * formatting
  * use use_lane_filter option
  * delete unused use-lane-filter option
  * change use_lane_filter to True, clarify the scope
  * change to use grid_map_utils::PolygonIterator
  * Add lane margin parameter
  * use boost geometry buffer to expand lanes
  * Change use_lane_filter param default to false
  * update README
  ---------
* bugfix(tier4_simulator_launch): fix occupancy grid map not appearing problem in psim  (`#3081 <https://github.com/autowarefoundation/autoware.universe/issues/3081>`_)
  * fixed psim occupancy grid map problem
  * fix parameter designation
  ---------
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* fix(tier4_perception_launch): fix config path (`#3078 <https://github.com/autowarefoundation/autoware.universe/issues/3078>`_)
  * fix(tier4_perception_launch): fix config path
  * use pointcloud_based_occupancy_grid_map.launch.py in tier4_simulator_launch
  ---------
* feat(probablisitic_occupancy_grid_map): add scan_frame option for gridmap generation (`#3032 <https://github.com/autowarefoundation/autoware.universe/issues/3032>`_)
  * add scan_frame and raytrace center
  * add scan frame to laserscan based method
  * update readme
  * fix typo
  * update laucher in perception_launch
  * fix config and launch file
  * fixed laserscan based launcher
  ---------
* fix(tier4_perception_launch): remove unnecessary node (`#2941 <https://github.com/autowarefoundation/autoware.universe/issues/2941>`_)
* fix(tier4_perception_launch): fix typo (`#2926 <https://github.com/autowarefoundation/autoware.universe/issues/2926>`_)
* feat(tier4_perception_launch): update cam/lidar detection architecture (`#2845 <https://github.com/autowarefoundation/autoware.universe/issues/2845>`_)
  * feat(tier4_perception_launch): update cam/lidar detection architecture
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): autoupdate (`#2819 <https://github.com/autowarefoundation/autoware.universe/issues/2819>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(ground_segmentation): fix unuse_time_series_filter bug (`#2824 <https://github.com/autowarefoundation/autoware.universe/issues/2824>`_)
* feat(tier4_perception_launch): add option for euclidean lidar detection model (`#842 <https://github.com/autowarefoundation/autoware.universe/issues/842>`_)
  feat(tier4_perception_launch): add euclidean lidar detection model
* fix(tier4_perception_launch): sync with tier4/autoware_launch (`#2568 <https://github.com/autowarefoundation/autoware.universe/issues/2568>`_)
  * fix(tier4_perception_launch): sync with tier4/autoware_launch
  * move centerpoint configs to perception.launch.xml
* feat(tier4_perception_launch): change the merge priority of roi_cluster_fusion to the lowest (`#2522 <https://github.com/autowarefoundation/autoware.universe/issues/2522>`_)
* feat(tier4_perception_launch): remove configs and move to autoware_launch (`#2539 <https://github.com/autowarefoundation/autoware.universe/issues/2539>`_)
  * feat(tier4_perception_launch): remove configs and move to autoware_launch
  * update readme
  * remove config
  * update readme
* fix(ground segmentation): change crop box range and add processing time (`#2260 <https://github.com/autowarefoundation/autoware.universe/issues/2260>`_)
  * fix(ground segmentation): change crop box range
  * chore(ground_segmentation): add processing time
* feat(tier4_perception_launch): sync perception launch to autoware_launch (`#2168 <https://github.com/autowarefoundation/autoware.universe/issues/2168>`_)
  * sync launch file from tier4 autoware launch
  * sync tlr launcher
  * ci(pre-commit): autofix
  * sync launch file from tier4 autoware launch
  * sync tlr launcher
  * ci(pre-commit): autofix
  * fix exec_depend in package.xml
  * Sync traffic light node
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(multiframe-pointpainting): add multi-sweep pointpainting (`#2124 <https://github.com/autowarefoundation/autoware.universe/issues/2124>`_)
  * feat: multiframe-pointpainting
  * ci(pre-commit): autofix
  * fix: retrieve changes of classremap
  * fix(image_projection_based_fusion): fix input to quaternion (`#1933 <https://github.com/autowarefoundation/autoware.universe/issues/1933>`_)
  * add: launch files
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): fix missing container argument (`#2087 <https://github.com/autowarefoundation/autoware.universe/issues/2087>`_)
  * fix(tier4_perception_launch): fix missing container argument
  * fix(tier4_perception_launch): rm unused param
* chore: fix typos (`#2140 <https://github.com/autowarefoundation/autoware.universe/issues/2140>`_)
  * chore: fix typos
  * chore: remove names in NOTE
* feat: use tracker shape size in detection by tracker (`#1683 <https://github.com/autowarefoundation/autoware.universe/issues/1683>`_)
  * support ref size in detection by tracker
  * add priority mode in object_merger
  * update launch
  * update launch
  * change to confidence mode
  * change variable name
  * Update perception/shape_estimation/lib/corrector/utils.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * ci(pre-commit): autofix
  * refactor
  * ci(pre-commit): autofix
  * :put_litter_in_its_place:
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(multi_object_tracking): enable delay compensation (`#1349 <https://github.com/autowarefoundation/autoware.universe/issues/1349>`_)
* fix(ground segmentation): add elevation grid ground filter (`#1899 <https://github.com/autowarefoundation/autoware.universe/issues/1899>`_)
  * fix: add grid elevation scan ground filter
  * chore: typo
  * fix: merge with scan ground filter
  * ci(pre-commit): autofix
  * chore: update docs
  * chore: remove debug variables
  * chore: add switchable param for grid scan mode
  * chore: typo
  * chore: refactoring
  * ci(pre-commit): autofix
  * chore: refactoring
  * chore: typo
  * chore: refactoring
  * chore: refactoring
  * chore: refactoring
  * chore: refactoring
  * chore: refactoring
  * chore: typo
  * docs: update docs
  * ci(pre-commit): autofix
  * chore: typo
  * chores: typo
  * chore: typo
  * docs: update
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_perception_launch): add enable_fine_detection_option (`#1991 <https://github.com/autowarefoundation/autoware.universe/issues/1991>`_)
  * feat(tier4_perception_launch): add enable_fine_detection_option
  * chore: rename
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_perception_launch): add arg to swtich lidar_centerpoint model (`#1865 <https://github.com/autowarefoundation/autoware.universe/issues/1865>`_)
* feat(multi_object_tracker): update bus size (`#1887 <https://github.com/autowarefoundation/autoware.universe/issues/1887>`_)
* refactor(lidar_centerpoint): change default threshold params (`#1874 <https://github.com/autowarefoundation/autoware.universe/issues/1874>`_)
* feat(multi_object_tracker): increase max-area for truck and trailer (`#1710 <https://github.com/autowarefoundation/autoware.universe/issues/1710>`_)
  * feat(multi_object_tracker): increase max-area for truck
  * feat: change truck and trailer max-area gate params
  * feat: change trailer params
* fix(tier4_perception_launch): add input/pointcloud to ground-segmentation (`#1833 <https://github.com/autowarefoundation/autoware.universe/issues/1833>`_)
* feat(radar_object_fusion_to_detected_object): enable confidence compensation in radar fusion (`#1755 <https://github.com/autowarefoundation/autoware.universe/issues/1755>`_)
  * update parameter
  * feature(radar_fusion_to_detected_object): add debug topic
  * feat(tier4_perception_launch): enable confidence compensation in radar fusion
  * add compensate probability paramter
  * fix parameter
  * update paramter
  * update paramter
  * fix parameter
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_perception_launch): make lanelet object filter optional (`#1698 <https://github.com/autowarefoundation/autoware.universe/issues/1698>`_)
  * feat(tier4_perception_launch): make lanelet object filter optional
  * feat(tier4_perception_launch): fix arg
  * feat(tier4_perception_launch): fix argument var
  * feat(tier4_perception_launch): add new parameter
  Co-authored-by: Kaan Colak <kcolak@leodrive.ai>
* fix(ground_filter): remove base_frame and fix ray_ground_filter  (`#1614 <https://github.com/autowarefoundation/autoware.universe/issues/1614>`_)
  * fix(ray_ground_filter): cannot remove ground pcl
  * fix: remove base_frame
  * docs: update docs
  * chores: remove unnecessary calculation
  * chores: remove unnecessary calculation
  * docs: update parameters
  * docs: update parameters
* fix(tier4_perception_launch): remove duplicated namespace of clustering in camera-lidar-fusion mode (`#1655 <https://github.com/autowarefoundation/autoware.universe/issues/1655>`_)
* feat(tier4_perception_launch): change unknown max area (`#1484 <https://github.com/autowarefoundation/autoware.universe/issues/1484>`_)
* fix(tier4_perception_launch): fix error of tier4_perception_launch_param_path (`#1445 <https://github.com/autowarefoundation/autoware.universe/issues/1445>`_)
* feat(tier4_perception_launch): declare param path argument (`#1394 <https://github.com/autowarefoundation/autoware.universe/issues/1394>`_)
  * feat(tier4_perception_launch): declare param path argument
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * fix ci error
  * fix ci error
* feature: update and fix detection launch (`#1340 <https://github.com/autowarefoundation/autoware.universe/issues/1340>`_)
  * cosmetic change
  * fix bug
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(tier4_perception_launch): add group tag (`#1238 <https://github.com/autowarefoundation/autoware.universe/issues/1238>`_)
  * fix(tier4_perception_launch): add group tag
  * fix missing tag
* fix(tier4_perception_launch): pass pointcloud_container params to pointcloud_map_filter in detection module (`#1312 <https://github.com/autowarefoundation/autoware.universe/issues/1312>`_)
  * fix(tier4_perception_launch): pass pointcloud_container to detection module
  * fix(tier4_perception_launch): container name in detection
* feat(tier4_perception_launch): add object filter params to tier4_perception_launch (`#1322 <https://github.com/autowarefoundation/autoware.universe/issues/1322>`_)
  * Add params to tier4_perception_launch
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: avoid same name nodes in detection module (`#1301 <https://github.com/autowarefoundation/autoware.universe/issues/1301>`_)
  * fix: avoid same name nodes in detection module
  * add node_name of object_association_merger
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/camera_lidar_fusion_based_detection.launch.xml
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/camera_lidar_fusion_based_detection.launch.xml
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/lidar_based_detection.launch.xml
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  * apply pre-commit
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
* Feature/radar fusion launch (`#1294 <https://github.com/autowarefoundation/autoware.universe/issues/1294>`_)
  * feat(tier4_perception_launch): add radar launcher (`#1263 <https://github.com/autowarefoundation/autoware.universe/issues/1263>`_)
  * feat(tier4_perception_launch): add radar launcher
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix reviewed by `#1263 <https://github.com/autowarefoundation/autoware.universe/issues/1263>`_
  * fix format
  * fix default arg
  * Revert "fix default arg"
  This reverts commit 72b2690dc8cbd91fa5b14da091f4027c2c5fa661.
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(tier4_perception_launch): revert `#1263 <https://github.com/autowarefoundation/autoware.universe/issues/1263>`_ (`#1285 <https://github.com/autowarefoundation/autoware.universe/issues/1285>`_)
* feat(tier4_perception_launch): add radar launcher (`#1263 <https://github.com/autowarefoundation/autoware.universe/issues/1263>`_)
  * feat(tier4_perception_launch): add radar launcher
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: change iou param of multi object  tracking (`#1267 <https://github.com/autowarefoundation/autoware.universe/issues/1267>`_)
* feat(object_filter): add detected object filter (`#1221 <https://github.com/autowarefoundation/autoware.universe/issues/1221>`_)
  * Add detected object filter
  * Refactor class name
  * Add readme
  * ADd lanelet filter option
  * change default parameter
  * refactor
  * Update readme
  * change detection launch
  * ADd unknown only option
  * Update launcher
  * Fix bug
  * Move object filter into detected_object_validator
  * ci(pre-commit): autofix
  * Add config parameter yaml for position filter
  * Add config for each class
  * ci(pre-commit): autofix
  * Fix config
  * Use shape instead of position
  * Update read me
  * Use disjoint instead of intersects
  * ci(pre-commit): autofix
  * Fix typo, remove debug code.
  * Use shared_ptr
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add option to use validator node in detection module (`#1233 <https://github.com/autowarefoundation/autoware.universe/issues/1233>`_)
  * feat: add option to use validator node in detection module
  * fix
  * remove use_validator option in detection/perception.launch
  * fix
* feat: change tracking param (`#1161 <https://github.com/autowarefoundation/autoware.universe/issues/1161>`_)
* feat: unknown objects from perception (`#870 <https://github.com/autowarefoundation/autoware.universe/issues/870>`_)
  * initial commit
  * change param
  * modify launch
  * ci(pre-commit): autofix
  * modify camera lidar fusion launch
  * update config
  * Update perception/detection_by_tracker/include/detection_by_tracker/utils.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update perception/detection_by_tracker/src/utils.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update perception/multi_object_tracker/src/utils/utils.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update perception/multi_object_tracker/include/multi_object_tracker/utils/utils.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update perception/multi_object_tracker/src/multi_object_tracker_core.cpp
  * modify for pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* feat: change data association param (`#1158 <https://github.com/autowarefoundation/autoware.universe/issues/1158>`_)
* fix(tier4_perception_launch): add missing dependencies in package.xml (`#1024 <https://github.com/autowarefoundation/autoware.universe/issues/1024>`_)
* feat: use multithread for traffic light container as default (`#995 <https://github.com/autowarefoundation/autoware.universe/issues/995>`_)
* fix: delete unused arg (`#988 <https://github.com/autowarefoundation/autoware.universe/issues/988>`_)
  * fix: delete unused arg
  * rename: detection_preprocessor -> pointcloud_map_filter
* fix(tier4_perception_launch): rename pkg name (`#981 <https://github.com/autowarefoundation/autoware.universe/issues/981>`_)
* feat: add down sample filter before detection module (`#961 <https://github.com/autowarefoundation/autoware.universe/issues/961>`_)
  * feat: add down sample filter before detection module
  * fix format
  * change comment
  * add output topic
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/detection_preprocess.launch.py
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/detection_preprocess.launch.py
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * ci(pre-commit): autofix
  * fix pre-commit
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: remove deprecated package in prediction launch (`#875 <https://github.com/autowarefoundation/autoware.universe/issues/875>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat: change ogm default launch (`#735 <https://github.com/autowarefoundation/autoware.universe/issues/735>`_)
* feat(scan_ground_filter): change launch option and threshold (`#670 <https://github.com/autowarefoundation/autoware.universe/issues/670>`_)
  * Add care for near but high points
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(image_projection_based_fusion, roi_cluster_fusion): roi and obstacle fusion method (`#548 <https://github.com/autowarefoundation/autoware.universe/issues/548>`_)
  * feat: init image_projection_based_fusion package
  * feat: debugger
  * feat: port roi_cluster_fusion to image_projection_based_fusion
  * feat: project detected_objects onto image
  * feat: update detected_object.classification
  * fix: add reset_cluster_semantic_type of roi_cluster_fusion
  * refactor: organize code
  * feat: add cylinderToVertices
  * feat: get transform_stamped at the fixed stamp
  * feat: not miss outside points of object on the image
  * chore: change the name of Copyright
  * kfeat: use image_projection_based_fusion instead of roi_cluster_fusion
  * docs: add roi_cluster_fusion and roi_detected_object_fusion
  * ci(pre-commit): autofix
  * docs: fix typo
  * refactor: rename function
  * refactor: delete member variables of input/output msg
  * fix: change when to clear a debugger
  * ci(pre-commit): autofix
  * refactor: use pre-increment
  * refactor: use range-based for loop
  * chore: add maintainer
  * feat: change the output in perception_launch
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: pointcloud based probabilistic occupancy grid map (`#624 <https://github.com/autowarefoundation/autoware.universe/issues/624>`_)
  * initial commit
  * ci(pre-commit): autofix
  * change param
  * update readme
  * add launch
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * fix typo
  * update readme
  * ci(pre-commit): autofix
  * cosmetic change
  * add single frame mode
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: localization and perception launch for tutorial (`#645 <https://github.com/autowarefoundation/autoware.universe/issues/645>`_)
  * fix: localization and perception launch for tutorial
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: launch detected object validator (`#585 <https://github.com/autowarefoundation/autoware.universe/issues/585>`_)
  * fix bug
  * add validator in launch
  * bug fix
  * modify arg
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/lidar_based_detection.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/camera_lidar_fusion_based_detection.launch.xml
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
* fix(multi_object_tracker): data association parameter (`#541 <https://github.com/autowarefoundation/autoware.universe/issues/541>`_)
  * sort matrix
  * ANIMAL->TRAILER
  * apply change to another file
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(multi_object_tracker): add iou gate (`#483 <https://github.com/autowarefoundation/autoware.universe/issues/483>`_)
  * add iou gate
  * ci(pre-commit): autofix
  * cosmetic change
  * fix bug
  * ci(pre-commit): autofix
  * fix bug
  * fix tier4_launch
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: detection launch in perception_launch (`#506 <https://github.com/autowarefoundation/autoware.universe/issues/506>`_)
* fix: integration miss related to camera lidar fusion (`#481 <https://github.com/autowarefoundation/autoware.universe/issues/481>`_)
  * fix integration miss
  * bug fix
  * add detection by tracker
  * Update launch/tier4_perception_launch/launch/object_recognition/detection/camera_lidar_fusion_based_detection.launch.xml
* fix(dummy_perception): fix to use launch at perception launch (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * fix(dummy_perception): fix to use launch file in perception launch
  * fix(tier4_perception_launch): fix angle increment for occupancy grid
* fix: change the default mode of perception.launch (`#409 <https://github.com/autowarefoundation/autoware.universe/issues/409>`_)
  * fix: change the default mode of perception.launch
  * chore: remove unnecessary comments
  * chore: remove default values
* ci: update .yamllint.yaml (`#229 <https://github.com/autowarefoundation/autoware.universe/issues/229>`_)
  * ci: update .yamllint.yaml
  * chore: fix for yamllint
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
* Contributors: Alexey Panferov, Alireza Moayyedi, Ismet Atabay, Kaan Çolak, Kenji Miyake, Kosuke Takeuchi, Kotaro Uetake, Mamoru Sobue, Mingyu1991, Ryohsuke Mitsudome, Satoshi Tanaka, Shinnosuke Hirakawa, Shintaro Tomie, Shumpei Wakabayashi, Shunsuke Miura, Taekjin LEE, Takagi, Isamu, Takayuki Murooka, Takeshi Miura, Tao Zhong, Tomohito ANDO, Tomoya Kimura, Vincent Richard, Xinyu Wang, Yamato Ando, Yoshi Ri, Yukihiro Saito, Yuntianyi Chen, Yusuke Muramatsu, badai nguyen, beginningfan, kminoda, pre-commit-ci[bot], taikitanaka3
