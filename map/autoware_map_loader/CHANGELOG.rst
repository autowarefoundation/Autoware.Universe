^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_map_loader
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
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
* fix(cpplint): include what you use - map (`#9568 <https://github.com/autowarefoundation/autoware.universe/issues/9568>`_)
* feat!: replace tier4_map_msgs with autoware_map_msgs for MapProjectorInfo (`#9392 <https://github.com/autowarefoundation/autoware.universe/issues/9392>`_)
* fix: fix package names in changelog files (`#9500 <https://github.com/autowarefoundation/autoware.universe/issues/9500>`_)
* update version
* refactor(map_loader)!: prefix package and namespace with autoware (`#8927 <https://github.com/autowarefoundation/autoware.universe/issues/8927>`_)
  * make lanelet2_map_visualization independent
  * remove unused files
  * remove unused package
  * fix package name
  * add autoware\_ prefix
  * add autoware to exec name
  * add autoware prefix
  * removed unnecessary dependency
  ---------
* refactor(map_loader)!: prefix package and namespace with autoware (`#8927 <https://github.com/autowarefoundation/autoware.universe/issues/8927>`_)
  * make lanelet2_map_visualization independent
  * remove unused files
  * remove unused package
  * fix package name
  * add autoware\_ prefix
  * add autoware to exec name
  * add autoware prefix
  * removed unnecessary dependency
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Masaki Baba, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(component_interface_specs): prefix package and namespace with autoware (`#9094 <https://github.com/autowarefoundation/autoware.universe/issues/9094>`_)
* refactor(ndt_scan_matcher)!: prefix package and namespace with autoware (`#8904 <https://github.com/autowarefoundation/autoware.universe/issues/8904>`_)
  add autoware\_ prefix
* docs(map_loader): update the link of pointcloud_divider (`#8823 <https://github.com/autowarefoundation/autoware.universe/issues/8823>`_)
  update link
* docs(map_loader): update the link of map_projection_loader (`#8825 <https://github.com/autowarefoundation/autoware.universe/issues/8825>`_)
  update the link of map_projection_loader
* chore(map_loader): update maintainer (`#8821 <https://github.com/autowarefoundation/autoware.universe/issues/8821>`_)
  update maintainer
* feat(map loader): visualize bus stop area and bicycle_lane (`#8777 <https://github.com/autowarefoundation/autoware.universe/issues/8777>`_)
* refactor(geography_utils): prefix package and namespace with autoware (`#7790 <https://github.com/autowarefoundation/autoware.universe/issues/7790>`_)
  * refactor(geography_utils): prefix package and namespace with autoware
  * move headers to include/autoware/
  ---------
* revert: revert "refactor(autoware_map_msgs): modify pcd metadata msg (`#7852 <https://github.com/autowarefoundation/autoware.universe/issues/7852>`_)" (`#8180 <https://github.com/autowarefoundation/autoware.universe/issues/8180>`_)
* refactor(autoware_map_msgs): modify pcd metadata msg (`#7852 <https://github.com/autowarefoundation/autoware.universe/issues/7852>`_)
* refactor(compare_map_segmentation): add package name prefix of autoware\_ (`#8005 <https://github.com/autowarefoundation/autoware.universe/issues/8005>`_)
  * refactor(compare_map_segmentation): add package name prefix of autoware\_
  * docs: update Readme
  ---------
* chore(localization, map): remove maintainer (`#7940 <https://github.com/autowarefoundation/autoware.universe/issues/7940>`_)
* feat(map_loader, route_handler)!: add format_version validation (`#7074 <https://github.com/autowarefoundation/autoware.universe/issues/7074>`_)
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* refactor(map_loader): apply static analysis (`#7845 <https://github.com/autowarefoundation/autoware.universe/issues/7845>`_)
  * refactor based on linter
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* style: updating the colors for the parking spaces and lot (`#7726 <https://github.com/autowarefoundation/autoware.universe/issues/7726>`_)
* feat(map_loader): add waypoints flag (`#7480 <https://github.com/autowarefoundation/autoware.universe/issues/7480>`_)
  * feat(map_loader): handle centelrine and waypoints
  * update README
  * fix doc
  * update schema
  * fix
  * fix
  ---------
* feat(map_loader): warn if some pcds from the metadata file are missing (`#7406 <https://github.com/autowarefoundation/autoware.universe/issues/7406>`_)
  * Examine if there are PCD segments found in the metadata file but are missing from the input pcd paths
  * style(pre-commit): autofix
  * Fixing CI
  * Fixing CI
  * Fixing CI
  * Fix CI related to map_loader
  * Removed try{} block from getPCDMetadata and redundant std::endl at the end of error messages
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(map): udpate maintainer (`#7405 <https://github.com/autowarefoundation/autoware.universe/issues/7405>`_)
  udpate maintainer
* fix(map_loader): add log output (`#7203 <https://github.com/autowarefoundation/autoware.universe/issues/7203>`_)
  add log output
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* feat!: replace autoware_auto_msgs with autoware_msgs for map modules (`#7244 <https://github.com/autowarefoundation/autoware.universe/issues/7244>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Anh Nguyen, Esteve Fernandez, Khalil Selyan, Mamoru Sobue, Masaki Baba, Ryohsuke Mitsudome, Takayuki Murooka, Yamato Ando, Yutaka Kondo, badai nguyen, kminoda

0.26.0 (2024-04-03)
-------------------
* fix(map_loader): fix warnings with single point cloud map metadata (`#6384 <https://github.com/autowarefoundation/autoware.universe/issues/6384>`_)
* fix(log-messages): reduce excessive log messages (`#5971 <https://github.com/autowarefoundation/autoware.universe/issues/5971>`_)
* chore(map_loader): rework parameters of map_loader (`#6199 <https://github.com/autowarefoundation/autoware.universe/issues/6199>`_)
  * Rework parameters of map_loader
  * style(pre-commit): autofix
  * Fixed typo in name of map_based_pediction.schema.json, which cause json-schema-check failed
  * Move path variables back to launch files
  * Update README.md
  * Undo the change in perception
  * Remove default values of declare_parameter from map_loader
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* revert(map_loader): revert the change error handling when pcd_metadata file (`#6294 <https://github.com/autowarefoundation/autoware.universe/issues/6294>`_)
  Revert "fix(map_loader): change error handling when pcd_metadata file not found (`#6227 <https://github.com/autowarefoundation/autoware.universe/issues/6227>`_)"
  This reverts commit 25bc636fe0f796c63daac60123aa6138146e515d.
* chore(lanelet2_map_loader): enrich error message (`#6245 <https://github.com/autowarefoundation/autoware.universe/issues/6245>`_)
* chore(map_loader): add maintainer (`#6232 <https://github.com/autowarefoundation/autoware.universe/issues/6232>`_)
* fix(map_loader): change error handling when pcd_metadata file not found (`#6227 <https://github.com/autowarefoundation/autoware.universe/issues/6227>`_)
  Changed error handling when pcd_metadata file not found
* chore: add localization & mapping maintainers (`#6085 <https://github.com/autowarefoundation/autoware.universe/issues/6085>`_)
  * Added lm maintainers
  * Add more
  * Fixed maintainer
  ---------
* fix(map_loader): show traffic light regulatory element id per lanelet (`#6028 <https://github.com/autowarefoundation/autoware.universe/issues/6028>`_)
  * fix(map_loader): show traffic light regulatory element id per lanelet
  * feat(map_loader): show traffic light id
  ---------
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* refactor(ndt_scan_matcher, map_loader): remove map_module (`#5873 <https://github.com/autowarefoundation/autoware.universe/issues/5873>`_)
  * Removed use_dynamic_map_loading
  * Removed enable_differential_load option
  * style(pre-commit): autofix
  * Fixed title
  * Emphasized lock scope
  * Removed pointcloud_map and  input_ekf_odom
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_loader): use dummy projector when using local coordinates (`#5866 <https://github.com/autowarefoundation/autoware.universe/issues/5866>`_)
  * feat(map_loader): use dummy projector when using local coordinates
  * fix build warning
  * fix runtime error
  * fix reverse function
  ---------
* chore(map_loader): visualize crosswalk id (`#5880 <https://github.com/autowarefoundation/autoware.universe/issues/5880>`_)
* chore: add maintainer in map packages (`#5865 <https://github.com/autowarefoundation/autoware.universe/issues/5865>`_)
  * add maintainer
  * modify map_tf_generator's maintainer
  ---------
* fix: add_ros_test to add_launch_test (`#5486 <https://github.com/autowarefoundation/autoware.universe/issues/5486>`_)
  * fix: add_ros_test to add_launch_test
  * fix ndt_scan_matcher
  ---------
* chore(map_loader): update readme (`#5468 <https://github.com/autowarefoundation/autoware.universe/issues/5468>`_)
  * chore(map_loader): update readme
  * make the annotation bold
  * fix
  ---------
* feat(map_loader): show intersection areas (`#5401 <https://github.com/autowarefoundation/autoware.universe/issues/5401>`_)
* feat(map_loader): display curbstone as marker array (`#4958 <https://github.com/autowarefoundation/autoware.universe/issues/4958>`_)
  display curbstone as marker array
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* refactor(map_packages): remove unused depend in pakcages.xml files (`#5172 <https://github.com/autowarefoundation/autoware.universe/issues/5172>`_)
  Co-authored-by: yamato-ando <Yamato ANDO>
* feat: support transverse mercator projection (`#4883 <https://github.com/autowarefoundation/autoware.universe/issues/4883>`_)
  * feat: support transverse mercator projection
  * fix some
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(geography_utils): add lanelet2_projector (`#4852 <https://github.com/autowarefoundation/autoware.universe/issues/4852>`_)
  * feat(geography_utils): add lanelet2_projector
  * style(pre-commit): autofix
  * update package.xml
  * style(pre-commit): autofix
  * edit static_centerline_optimizer
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: use constant string for map_projector_info (`#4789 <https://github.com/autowarefoundation/autoware.universe/issues/4789>`_)
  * feat: use constant string for map_projector_info
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat!: add vertical datum in map_projector_info (`#4708 <https://github.com/autowarefoundation/autoware.universe/issues/4708>`_)
  * resolve conflict
  * update
  * UTM -> LocalCartesianUTM
  * style(pre-commit): autofix
  * update
  * update readme
  * add altitude
  * style(pre-commit): autofix
  * update minor parts
  * add vertical datum for lanelet2
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat!: rename utm to local_cartesian_utm (`#4704 <https://github.com/autowarefoundation/autoware.universe/issues/4704>`_)
  * feat(map_projection_loader, map_loader): rename utm to local_cartesian_utm
  * fix readme
  * fix default ad api
  ---------
* feat!: rename map_projector_type to map_projector_info (`#4664 <https://github.com/autowarefoundation/autoware.universe/issues/4664>`_)
* fix(lanelet2_map_loader): fixed parameter declaration timing (`#4639 <https://github.com/autowarefoundation/autoware.universe/issues/4639>`_)
  Change parameter declaration timing
* fix(map_loader, map_projection_loader): use component interface specs (`#4585 <https://github.com/autowarefoundation/autoware.universe/issues/4585>`_)
  * feat(map): use component_interface_specs in map_projection_loader
  * update map_loader
  * style(pre-commit): autofix
  * feat: add dummy typo
  * update name
  * fix test
  * fix test
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_projection_loader): add map_projection_loader (`#3986 <https://github.com/autowarefoundation/autoware.universe/issues/3986>`_)
  * feat(map_projection_loader): add map_projection_loader
  * style(pre-commit): autofix
  * Update default algorithm
  * fix test
  * style(pre-commit): autofix
  * add readme
  * style(pre-commit): autofix
  * fix launch file and fix map_loader
  * style(pre-commit): autofix
  * update lanelet2
  * fill yaml file path
  * style(pre-commit): autofix
  * update readme
  * style(pre-commit): autofix
  * minor fix
  * style(pre-commit): autofix
  * fix test
  * style(pre-commit): autofix
  * add include guard
  * style(pre-commit): autofix
  * update test
  * update map_loader
  * style(pre-commit): autofix
  * update docs
  * style(pre-commit): autofix
  * update
  * add dependency
  * style(pre-commit): autofix
  * remove unnecessary parameter
  * update
  * update test
  * style(pre-commit): autofix
  * add url
  * enable python tests
  * style(pre-commit): autofix
  * small fix
  * fix grammar
  * remove transverse mercator
  * style(pre-commit): autofix
  * add rule in map
  * fix readme of map loader
  * remove transverse mercator for now
  * add utm
  * remove altitude from current projection loader
  * style(pre-commit): autofix
  * fix pre-commit
  * fix build error
  * fix: remove package.xml
  * fix clang-tidy
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* chore: add maintainer in localization and map packages (`#4501 <https://github.com/autowarefoundation/autoware.universe/issues/4501>`_)
* feat(goal_planner): add no_parking_area for goal search (`#3467 <https://github.com/autowarefoundation/autoware.universe/issues/3467>`_)
  * feat(behavior_path_planner): use no_parking_area for pull_over
  * support no_stopping_area
  ---------
* fix(map_loader): fix spell-check (`#4280 <https://github.com/autowarefoundation/autoware.universe/issues/4280>`_)
* feat(crosswalk): support crosswalk regulatory element (`#3939 <https://github.com/autowarefoundation/autoware.universe/issues/3939>`_)
  * feat(crosswalk): use regulatory element
  * feat(map_loader): show crosswalk areas
  ---------
* fix(map_loader): update readme for metadata (`#3919 <https://github.com/autowarefoundation/autoware.universe/issues/3919>`_)
  * fix(map_loader): update readme for metadata
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * add not applicable rules
  * style(pre-commit): autofix
  * fix
  * update
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(map_loader): handle enable_selected_load correctly (`#3920 <https://github.com/autowarefoundation/autoware.universe/issues/3920>`_)
  * fix(map_loader): update readme for metadata
  * fix(map_loader): handle enable_selected_load flag correctly
  * style(pre-commit): autofix
  * revert readme
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_loader): use cylindrical area for map loader (`#3863 <https://github.com/autowarefoundation/autoware.universe/issues/3863>`_)
  * feat(map_loader): use cylindrical area for query instead of spherical area
  * update
  * style(pre-commit): autofix
  * update AreaInfo
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_loader): add publish map projector info (`#3200 <https://github.com/autowarefoundation/autoware.universe/issues/3200>`_)
  * add publish mgrs grid
  * fix publish wrong grid code when there is no mgrs code in lanelet
  * Revert "fix publish wrong grid code when there is no mgrs code in lanelet"
  This reverts commit 10023662abba56bcf395d899f787b7bbed4e8fd4.
  * temp fix for emtpy coordinate
  * add UTM support
  * add local projector support
  * remove check coordinate 0,0
  * Revert "add local projector support"
  This reverts commit 91e6921718695031a2a08e2109bca0b61ab54e89.
  * add local publish
  ---------
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* fix(map_loader): re-align lanelet borders after overwriting coordinates (`#3825 <https://github.com/autowarefoundation/autoware.universe/issues/3825>`_)
* fix(map_loader): fix readme (`#3667 <https://github.com/autowarefoundation/autoware.universe/issues/3667>`_)
* feat(map_loader): visualize hatched road markings (`#3639 <https://github.com/autowarefoundation/autoware.universe/issues/3639>`_)
  * feat(map_loader): visualize hatched road markings
  * update
  ---------
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
* feat(map_loader): add local map projector (`#3492 <https://github.com/autowarefoundation/autoware.universe/issues/3492>`_)
  * feat(map_loader): add local map projector
  * update README
  * update readme
  * use the same naming standard
  ---------
* feat(map_loader): add selected map loader (`#3286 <https://github.com/autowarefoundation/autoware.universe/issues/3286>`_)
  * add id based map loader
  * add metadata publisher
  * feat(map_loader): add support for sequential_map_loading
  * feat(map_loader): add support for selected_map_loader and structure of metadata
  * feat(map_loader): turn off selected_map_loading as default setting
  * feat(map_loader): update map_loader corresponding to autoware_map_msgs update
  * docs(map_loader): add description of selected pcd load server and pcd metadata publisher
  * style(pre-commit): autofix
  * feat(map_loader): change onServiceGetSelectedPointCloudMap into const function
  ---------
  Co-authored-by: Shin-kyoto <58775300+Shin-kyoto@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(map_loader): fix a bug that occurs when loading multiple pcds (`#3274 <https://github.com/autowarefoundation/autoware.universe/issues/3274>`_)
  * fix(map_loader): fix a bug that occurs when loading multiple pcds
  * fix
  ---------
* feat(map_loader): add grid coordinates for partial/differential map load (`#3205 <https://github.com/autowarefoundation/autoware.universe/issues/3205>`_)
  * feat(map_loader): add grid coordinates for partial/differential map load
  * style(pre-commit): autofix
  * update readme
  * remove unnecessary line
  * update arguments in readme
  * slightly updated directory structure in readme
  * update readme
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(map_loader): address deprecated warning for some environment (`#3188 <https://github.com/autowarefoundation/autoware.universe/issues/3188>`_)
  fix(map_loader): address deprecated warning for some version
* test(map_loader): add a ROS 2 test (`#3170 <https://github.com/autowarefoundation/autoware.universe/issues/3170>`_)
  * chore(map_loader): add a ROS 2 test
  * style(pre-commit): autofix
  * debug
  * style(pre-commit): autofix
  * added other tests too
  * style(pre-commit): autofix
  * fix pre-commit
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(tier4_map_launch): add lanelet2 config files to tier4_map_launch (`#2670 <https://github.com/autowarefoundation/autoware.universe/issues/2670>`_)
  * chore(tier4_map_launch): add lanelet2 config files to tier4_map_launch
  Update launch/tier4_map_launch/launch/map.launch.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  delete  lanelet2_map_projector type in launch
  remove config path
  * chore(tier4_map_launch): fix lanelet launch name
  ---------
* ci(pre-commit): autoupdate (`#2819 <https://github.com/autowarefoundation/autoware.universe/issues/2819>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(ndt_scan_matcher): dynamic map loading (`#2339 <https://github.com/autowarefoundation/autoware.universe/issues/2339>`_)
  * first commit
  * ci(pre-commit): autofix
  * import map update module in core
  * ci(pre-commit): autofix
  * minor fixes. Now map update module launches!!!
  * ci(pre-commit): autofix
  * debugged
  * revert unnecessary fix
  * minor fixes
  * update launch file
  * update comment
  * ci(pre-commit): autofix
  * update comment
  * update comment
  * ci(pre-commit): autofix
  * update comment
  * ci(pre-commit): autofix
  * update for ndt_omp
  * changed parameter names
  * ci(pre-commit): autofix
  * apply pre-commit-
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * simplify client implementation
  * remove unnecessary comments
  * ci(pre-commit): autofix
  * removed unused member variables
  * set default use_dynamic_map_loading to true
  * changed readme
  * ci(pre-commit): autofix
  * reflected comments
  * use std::optional instead of shared_ptr
  * ci(pre-commit): autofix
  * fix parameter description
  * revert launch output config
  * change default subscriber name
  * remove unnecessary setInputSource
  * add gif
  * ci(pre-commit): autofix
  * minor fix
  * Update localization/ndt_scan_matcher/src/map_update_module.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * update literals
  * update map_loader default parameters
  * update readme
  * ci(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* fix(lanelet2_map_loader): delete unused parameters (`#2761 <https://github.com/autowarefoundation/autoware.universe/issues/2761>`_)
  * fix(lanelet2_map_loader): delete unused parameters
  * Update lanelet2_map_loader.launch.xml
* fix(map_loader): apply clang-tidy (`#2668 <https://github.com/autowarefoundation/autoware.universe/issues/2668>`_)
  * fix(map_loader): apply clang-tidy
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(behavior_velocity_planner): add speed bump module (`#647 <https://github.com/autowarefoundation/autoware.universe/issues/647>`_)
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
* feat(map_loader): add differential map loading interface (`#2417 <https://github.com/autowarefoundation/autoware.universe/issues/2417>`_)
  * first commit
  * ci(pre-commit): autofix
  * added module load in _node.cpp
  * ci(pre-commit): autofix
  * create pcd metadata dict when either of the flag is true
  * ci(pre-commit): autofix
  * fix readme
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_loader): add downsampled pointcloud publisher (`#2418 <https://github.com/autowarefoundation/autoware.universe/issues/2418>`_)
  * first commit
  * debugged
  * update readme
  * update param in tier4_map_launch
  * debug
  * debugged
  * Now build works
  * ci(pre-commit): autofix
  * set default param to false
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_loader): add partial map loading interface in pointcloud_map_loader (`#1938 <https://github.com/autowarefoundation/autoware.universe/issues/1938>`_)
  * first commit
  * reverted unnecessary modification
  * ci(pre-commit): autofix
  * renamed some classes
  * ci(pre-commit): autofix
  * move autoware_map_msgs to autoware_msgs repos
  * catch up with the modification in autoware_map_msgs
  * ci(pre-commit): autofix
  * aligned with autoware_map_msgs change (differential/partial modules seperation)
  * ci(pre-commit): autofix
  * debugged
  * debugged
  * added min-max info and others
  * ci(pre-commit): autofix
  * minor fix
  * already_loaded -> cached
  * ci(pre-commit): autofix
  * load\_ -> get\_
  * ci(pre-commit): autofix
  * resolve pre-commit
  * ci(pre-commit): autofix
  * minor fix
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * update readme
  * minor fix in readme
  * grammarly
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * fix copyright
  * fix launch file
  * remove leaf_size param
  * removed unnecessary things
  * removed downsample for now
  * removed differential_map_loader for this PR (would make another PR for this)
  * ci(pre-commit): autofix
  * removed differential_map_loader, debugged
  * ci(pre-commit): autofix
  * removed leaf_size description
  * ci(pre-commit): autofix
  * refactor sphereAndBoxOverlapExists
  * ci(pre-commit): autofix
  * added test for sphereAndBoxOverlapExists
  * ci(pre-commit): autofix
  * remove downsample function for now
  * remove fmt from target_link_libraries in test
  * minor fix in cmakelists.txt
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(map_loader): modularization (`#2243 <https://github.com/autowarefoundation/autoware.universe/issues/2243>`_)
  * refactor(map_loader): modularization
  * ci(pre-commit): autofix
  * simplified
  * removed autoware_msgs dependency (not yet necessary at this moment)
  * ci(pre-commit): autofix
  * remove unnecessary changes
  * pre-commit
  * ci(pre-commit): autofix
  * edit copyright
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(map_loader): add maintainer (`#2245 <https://github.com/autowarefoundation/autoware.universe/issues/2245>`_)
  * chore(map_loader): add maintainer
  * remove miyake-san
* feat(map_loader): make some functions static (`#2014 <https://github.com/autowarefoundation/autoware.universe/issues/2014>`_)
  * feat(map_loader): make some functions static
  * make publisher alive after constructor
* refactor(map_loader): split to member functions (`#1941 <https://github.com/autowarefoundation/autoware.universe/issues/1941>`_)
* chore(planning/control packages): organized authors and maintainers (`#1610 <https://github.com/autowarefoundation/autoware.universe/issues/1610>`_)
  * organized planning authors and maintainers
  * organized control authors and maintainers
  * fix typo
  * fix colcon test
  * fix
  Update control/external_cmd_selector/package.xml
  Update control/vehicle_cmd_gate/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/motion_velocity_smoother/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/freespace_planner/package.xml
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix
  * fix
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat: add vector map inside area filter (`#1530 <https://github.com/autowarefoundation/autoware.universe/issues/1530>`_)
  * feat: add no detection area filter
  * ci(pre-commit): autofix
  * chore: add documents
  * pre-commit fix
  * remove comments
  * fix comments
  * refactor condition to launch points filter
  * fix container name
  * ci(pre-commit): autofix
  * chore: add visualization for no obstacle segmentation area
  * feat: allow any tags to be given by launch arguments
  * chore: remove unnecessary includes
  * feat: move the polygon removing function to util and use it
  * chore: move the place and change the name of node
  * chore: pre-commit fix
  * chore: remove unnecessary using
  * chore: modify container name
  * chore: fix comments
  * chore: fix comments
  * chore: use output arguments for a large data
  * chore: using namespace of PolygonCgal for readability
  * feat: add functions for multiple polygons
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* test(map_loader): add launch test for the 'lanelet2_map_loader' node (`#1056 <https://github.com/autowarefoundation/autoware.universe/issues/1056>`_)
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat: add parameter argument for lanelet2_map_loader (`#954 <https://github.com/autowarefoundation/autoware.universe/issues/954>`_)
  * feat: add parameter argument for lanelet2_map_loader
  * feat: add comment
* fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader (`#942 <https://github.com/autowarefoundation/autoware.universe/issues/942>`_)
  * fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader
  * fix(map_loader): remove c_str
  * fix(map_loader): replace c_str to string
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(map_loader): modify build error in rolling (`#777 <https://github.com/autowarefoundation/autoware.universe/issues/777>`_)
* fix(map_loader): map_loader package not working in UTM coordinates (`#627 <https://github.com/autowarefoundation/autoware.universe/issues/627>`_)
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * fix(map_loader): add UTM projector to map_loader package
  * fix(map_loader): update config
  * fix(map_loader): update lanelet2_map_loader_node.cpp inlude structure
  * fix(map_loader): update include structure
  * fix(map_loader): add map_projector_type parameter to map.launch.py
  * fix(map_loader): update map.launch.py
  * fix(map_loader): update map.launch.py
  * fix(map_loader): update map.launch.py
  * fix(map_loader): update map.launch.py
  * Update lanelet2_map_loader_node.cpp
  Co-authored-by: M. Fatih Cırıt <xmfcx@users.noreply.github.com>
  * fix launch file
  * ci(pre-commit): autofix
  * Update launch/tier4_map_launch/launch/map.launch.py
  Co-authored-by: Berkay <brkay54@gmail.com>
  * ci(pre-commit): autofix
  * update for merge error
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: M. Fatih Cırıt <xmfcx@users.noreply.github.com>
  Co-authored-by: Berkay <brkay54@gmail.com>
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(lanelet2_extension,map_loader): add guard_rail wall fence as lanelet tag (`#478 <https://github.com/autowarefoundation/autoware.universe/issues/478>`_)
  * feat(lanelet2_extension): add guard_rails fence wall as lanelet tag
  * feat(map_loader): add visualization for partion lanelet
* feat: rename existing packages name starting with autoware to different names (`#180 <https://github.com/autowarefoundation/autoware.universe/issues/180>`_)
  * autoware_api_utils -> tier4_api_utils
  * autoware_debug_tools -> tier4_debug_tools
  * autoware_error_monitor -> system_error_monitor
  * autoware_utils -> tier4_autoware_utils
  * autoware_global_parameter_loader -> global_parameter_loader
  * autoware_iv_auto_msgs_converter -> tier4_auto_msgs_converter
  * autoware_joy_controller -> joy_controller
  * autoware_error_monitor -> system_error_monitor(launch)
  * autoware_state_monitor -> ad_service_state_monitor
  * autoware_web_controller -> web_controller
  * remove autoware_version
  * remove autoware_rosbag_recorder
  * autoware\_*_rviz_plugin -> tier4\_*_rviz_plugin
  * fix ad_service_state_monitor
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* refactor: remove unnecessary messages (`#133 <https://github.com/autowarefoundation/autoware.universe/issues/133>`_)
  * remove ControlCommand.msg and ControlCommandStamped.msg
  * remove BatteryStatus.msg RawControlCommand.msg RawVehicleCommand.msg VehicleCommand.msg
  * remove traffic_light_recognition msgs
  * remove unnecessary autoware_planning_msgs
  * remove unnecessary build_depends
  * remove unnecessary autoware_system_msgs
  * remove autoware_lanelet2_msgs
  * fix map loader README
  * fix external_cmd_converter README
  * refactor: remove autoware_perception_msgs
  * refactor: remove unnecessary include files
  * fix: detection_by_tracker README
  * ci(pre-commit): autofix
  * refactor: remove autoware_vehicle_msgs
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * fix: each messages
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: move elevation map loader (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_) (`#136 <https://github.com/autowarefoundation/autoware.universe/issues/136>`_)
  * feat: Move elevation map loader (`#740 <https://github.com/autowarefoundation/autoware.universe/issues/740>`_)
  * Update perception/elevation_map_loader/README.md
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * Update perception/elevation_map_loader/README.md
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  * Update perception/elevation_map_loader/README.md
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* feat: add pcd map hash generator (`#745 <https://github.com/autowarefoundation/autoware.universe/issues/745>`_) (`#130 <https://github.com/autowarefoundation/autoware.universe/issues/130>`_)
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* feat: add map packages (`#8 <https://github.com/autowarefoundation/autoware.universe/issues/8>`_)
  * release v0.4.0
  * add resolution param in lanelet2_extension (`#760 <https://github.com/autowarefoundation/autoware.universe/issues/760>`_)
  * Fix/extend drivable area beyond goal (`#781 <https://github.com/autowarefoundation/autoware.universe/issues/781>`_)
  * update llt2 extention query func
  * extend drivable area over goal point
  * apply clang
  * update get preeceeding func
  * update preceeding func in lanechange
  * update comment
  * Fix intersection preceeding lane query (`#807 <https://github.com/autowarefoundation/autoware.universe/issues/807>`_)
  * modified interseciton module to add lanelets in intersection to objective lanelets due to change in getPreceedingLaneletSequences()
  * update comment
  * Install executables in lanelet2_map_preprocessor (`#834 <https://github.com/autowarefoundation/autoware.universe/issues/834>`_)
  * remove ROS1 packages temporarily
  * Revert "remove ROS1 packages temporarily"
  This reverts commit 3290a8b9e92c9eae05d9159c8b9fd56ca8935c01.
  * add COLCON_IGNORE to ros1 packages
  * Rename launch files to launch.xml (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * port map_tf_generator (`#32 <https://github.com/autowarefoundation/autoware.universe/issues/32>`_)
  * port map_tf_generator
  * add missing dependency
  * fix pointor, tf_broadcaster, add compile option
  * use ament_auto
  * Port lanelet2 extension (`#36 <https://github.com/autowarefoundation/autoware.universe/issues/36>`_)
  * remove COLCON_IGNORE
  * port to ROS2
  * minor fix
  * fix CI
  * remove unnecessary semi-colon
  * fix library to executable for lanelet2_extension_sample and autoware_lanelet2_validation
  * fix usage for ROS2
  * fix usage message and parameter declaration
  * fix getting map_file parameter
  * Port map loader (`#44 <https://github.com/autowarefoundation/autoware.universe/issues/44>`_)
  * port map_loader to ROS2
  * fix unintended change
  * Update map/map_loader/CMakeLists.txt
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * Add geometry2 to repos (`#76 <https://github.com/autowarefoundation/autoware.universe/issues/76>`_)
  * add geometry2 package temporarily until new release
  * trigger-ci
  * add tf2 dependency to the packages that use tf2_geometry_msgs
  * Revert "Add geometry2 to repos (`#76 <https://github.com/autowarefoundation/autoware.universe/issues/76>`_)" (`#96 <https://github.com/autowarefoundation/autoware.universe/issues/96>`_)
  * Revert "Add geometry2 to repos (`#76 <https://github.com/autowarefoundation/autoware.universe/issues/76>`_)"
  This reverts commit 7dbe25ed5ff7d5f413fda567dcc77a70c79a7826.
  * Re-add tf2 dependencies
  * Revert "Re-add tf2 dependencies"
  This reverts commit e23b0c8b0826cf9518924d33349f9de34b4975df.
  * Debug CI pipeline
  * Revert "Debug CI pipeline"
  This reverts commit 58f1eba550360d82c08230552abfb64b33b23e0f.
  * Explicitly install known versions of the geometry packages
  * No need to skip tf2 packages anymore
  Co-authored-by: Esteve Fernandez <esteve@apache.org>
  * Rename h files to hpp (`#142 <https://github.com/autowarefoundation/autoware.universe/issues/142>`_)
  * Change includes
  * Rename files
  * Adjustments to make things compile
  * Other packages
  * Adjust copyright notice on 532 out of 699 source files (`#143 <https://github.com/autowarefoundation/autoware.universe/issues/143>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * Run uncrustify on the entire Pilot.Auto codebase (`#151 <https://github.com/autowarefoundation/autoware.universe/issues/151>`_)
  * Run uncrustify on the entire Pilot.Auto codebase
  * Exclude open PRs
  * fixing trasient_local in ROS2 packages (`#160 <https://github.com/autowarefoundation/autoware.universe/issues/160>`_)
  * added linters to lanelet1_extension (`#170 <https://github.com/autowarefoundation/autoware.universe/issues/170>`_)
  * adding linters to map_loader (`#171 <https://github.com/autowarefoundation/autoware.universe/issues/171>`_)
  * adding linters to map_tf_generator (`#172 <https://github.com/autowarefoundation/autoware.universe/issues/172>`_)
  * apply env_var to  use_sim_time (`#222 <https://github.com/autowarefoundation/autoware.universe/issues/222>`_)
  * Ros2 v0.8.0 map loader and lanelet2 extension (`#279 <https://github.com/autowarefoundation/autoware.universe/issues/279>`_)
  * Ros2 v0.8 fix typo of "preceding" (`#323 <https://github.com/autowarefoundation/autoware.universe/issues/323>`_)
  * Fix typo of getPrecedingLaneletSequences
  * Fix comment
  * Fix rviz2 low FPS (`#390 <https://github.com/autowarefoundation/autoware.universe/issues/390>`_)
  * add nullptr check when publish concatenate data (`#369 <https://github.com/autowarefoundation/autoware.universe/issues/369>`_)
  * Add warning msg when concat pointcloud is not published (`#388 <https://github.com/autowarefoundation/autoware.universe/issues/388>`_)
  * Change lineString2Marker
  * Change trafficLight2TriangleMarker
  * Change laneletDirectionAsMarker
  * Remove debug code
  * Fix linter problems
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * [map_loader] modify colors for lane markers for better visualization (`#398 <https://github.com/autowarefoundation/autoware.universe/issues/398>`_)
  * fix empty marker (`#423 <https://github.com/autowarefoundation/autoware.universe/issues/423>`_)
  * Fix typo in map module (`#437 <https://github.com/autowarefoundation/autoware.universe/issues/437>`_)
  * add license (`#443 <https://github.com/autowarefoundation/autoware.universe/issues/443>`_)
  * avoid pushing empty marker (`#441 <https://github.com/autowarefoundation/autoware.universe/issues/441>`_)
  * avoid pushing empty marker
  * size0 -> empty
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Sync public repo (`#1228 <https://github.com/autowarefoundation/autoware.universe/issues/1228>`_)
  * [simple_planning_simulator] add readme (`#424 <https://github.com/autowarefoundation/autoware.universe/issues/424>`_)
  * add readme of simple_planning_simulator
  * Update simulator/simple_planning_simulator/README.md
  * set transit_margin_time to intersect. planner (`#460 <https://github.com/autowarefoundation/autoware.universe/issues/460>`_)
  * Fix pose2twist (`#462 <https://github.com/autowarefoundation/autoware.universe/issues/462>`_)
  * Ros2 vehicle info param server (`#447 <https://github.com/autowarefoundation/autoware.universe/issues/447>`_)
  * add vehicle_info_param_server
  * update vehicle info
  * apply format
  * fix bug
  * skip unnecessary search
  * delete vehicle param file
  * fix bug
  * Ros2 fix topic name part2 (`#425 <https://github.com/autowarefoundation/autoware.universe/issues/425>`_)
  * Fix topic name of traffic_light_classifier
  * Fix topic name of traffic_light_visualization
  * Fix topic name of traffic_light_ssd_fine_detector
  * Fix topic name of traffic_light_map_based_detector
  * Fix lint traffic_light_recognition
  * Fix lint traffic_light_ssd_fine_detector
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_classifier
  * Fix lint traffic_light_ssd_fine_detector
  * Fix issues in hdd_reader (`#466 <https://github.com/autowarefoundation/autoware.universe/issues/466>`_)
  * Fix some issues detected by Coverity Scan and Clang-Tidy
  * Update launch command
  * Add more `close(new_sock)`
  * Simplify the definitions of struct
  * fix: re-construct laneletMapLayer for reindex RTree (`#463 <https://github.com/autowarefoundation/autoware.universe/issues/463>`_)
  * Rviz overlay render fix (`#461 <https://github.com/autowarefoundation/autoware.universe/issues/461>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * back to RTD as superclass
  * Rviz overlay render in update (`#465 <https://github.com/autowarefoundation/autoware.universe/issues/465>`_)
  * Moved painiting in SteeringAngle plugin to update()
  * super class now back to MFD
  * uncrustified
  * acquire data in mutex
  * removed unnecessary includes and some dead code
  * Adepted remaining vehicle plugin classes to render-in-update concept. Returned to MFD superclass
  * restored RTD superclass
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  * Revert "fix: re-construct laneletMapLayer for reindex RTree (`#463 <https://github.com/autowarefoundation/autoware.universe/issues/463>`_)" (`#1229 <https://github.com/autowarefoundation/autoware.universe/issues/1229>`_)
  This reverts commit d2ecdfe4c58cb4544c9a3ee84947b36b7ee54421.
  * add pcd file check (`#1232 <https://github.com/autowarefoundation/autoware.universe/issues/1232>`_)
  * add pcd file check
  * add space
  * add &
  * use namespace
  * Unify Apache-2.0 license name (`#1242 <https://github.com/autowarefoundation/autoware.universe/issues/1242>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Map components (`#1311 <https://github.com/autowarefoundation/autoware.universe/issues/1311>`_)
  * Make pointcloud map loader component
  * Make lanelet2 map loader component
  * Make map tf generator component
  * Apply lint
  * Rename parameter for lanelet2 map path
  * Fix license
  * Add comment for filesystem
  * Fix variable name for glob
  * Fix dependency for query (`#1519 <https://github.com/autowarefoundation/autoware.universe/issues/1519>`_)
  * Fix a small bug (`#1644 <https://github.com/autowarefoundation/autoware.universe/issues/1644>`_)
  * Fix minor flaws detected by Clang-Tidy (`#1647 <https://github.com/autowarefoundation/autoware.universe/issues/1647>`_)
  - misc-throw-by-value-catch-by-reference
  - cppcoreguidelines-init-variables
  - readability-isolate-declaration
  * Add pre-commit (`#1560 <https://github.com/autowarefoundation/autoware.universe/issues/1560>`_)
  * add pre-commit
  * add pre-commit-config
  * add additional settings for private repository
  * use default pre-commit-config
  * update pre-commit setting
  * Ignore whitespace for line breaks in markdown
  * Update .github/workflows/pre-commit.yml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * exclude svg
  * remove pretty-format-json
  * add double-quote-string-fixer
  * consider COLCON_IGNORE file when seaching modified package
  * format file
  * pre-commit fixes
  * Update pre-commit.yml
  * Update .pre-commit-config.yaml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Porting traffic light viz (`#1284 <https://github.com/autowarefoundation/autoware.universe/issues/1284>`_)
  * Feature/traffic light viz (`#1001 <https://github.com/autowarefoundation/autoware.universe/issues/1001>`_)
  * add tl map viz
  * bug fix
  * update map visualizer
  * add launch
  * add install in cmake
  * remove unused file
  * fix build error
  * Fix lint
  * Fix typo
  * Fix topic name and qos
  * Replace deprecated duration api
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  * Add markdownlint and prettier (`#1661 <https://github.com/autowarefoundation/autoware.universe/issues/1661>`_)
  * Add markdownlint and prettier
  * Ignore .param.yaml
  * Apply format
  * Feature/compare elevation map (`#1488 <https://github.com/autowarefoundation/autoware.universe/issues/1488>`_)
  * suppress warnings for declare parameters (`#1724 <https://github.com/autowarefoundation/autoware.universe/issues/1724>`_)
  * fix for lanelet2_extension
  * fix for traffic light ssd fine detector
  * fix for topic_state_monitor
  * fix for dummy diag publisher
  * fix for remote cmd converter
  * fix for vehicle_info_util
  * fix for multi object tracker
  * fix for freespace planner
  * fix for autoware_error_monitor
  * add Werror for multi object tracker
  * fix for multi object tracker
  * add Werror for liraffic light ssd fine detector
  * add Werror for topic state monitor
  * add Werror
  * add Werror
  * add Werror
  * add Werror
  * fix style
  * suppress warnings for map (`#1773 <https://github.com/autowarefoundation/autoware.universe/issues/1773>`_)
  * add compile option
  * fix error
  * add compile option
  * add maybe unused
  * fix sign-compare
  * delete unused
  * add parentheses
  * fix for uncrusify
  * Fix typo
  * use U
  * use U
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Fix clang warnings (`#1859 <https://github.com/autowarefoundation/autoware.universe/issues/1859>`_)
  * Fix -Wreturn-std-move
  * Fix -Wunused-private-field
  * Ignore -Wnonportable-include-path for mussp
  * Fix -Wunused-const-variable
  * Fix "can not be used when making a shared object"
  * Sync v1.3.0 (`#1909 <https://github.com/autowarefoundation/autoware.universe/issues/1909>`_)
  * Add elevation_map to autoware_state_monitor (`#1907 <https://github.com/autowarefoundation/autoware.universe/issues/1907>`_)
  * Disable saving elevation map temporarily (`#1906 <https://github.com/autowarefoundation/autoware.universe/issues/1906>`_)
  * Fix typos in README of map_loader (`#1923 <https://github.com/autowarefoundation/autoware.universe/issues/1923>`_)
  * Fix typos in README of map_loader
  * Apply Prettier
  * fix some typos (`#1941 <https://github.com/autowarefoundation/autoware.universe/issues/1941>`_)
  * fix some typos
  * fix typo
  * Fix typo
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Add autoware api (`#1979 <https://github.com/autowarefoundation/autoware.universe/issues/1979>`_)
  * Invoke code formatter at pre-commit (`#1935 <https://github.com/autowarefoundation/autoware.universe/issues/1935>`_)
  * Run ament_uncrustify at pre-commit
  * Reformat existing files
  * Fix copyright and cpplint errors
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  * Save elevation_map with pcd md5sum (`#1988 <https://github.com/autowarefoundation/autoware.universe/issues/1988>`_)
  * Save elevation_map with pcd md5sum
  * Update sample launch
  * Fix cpplint
  * Use hash-library instead of openssl
  * Use call by reference
  * Apply format
  * Set CMAKE_CXX_STANDARD 17
  * Save input_pcd.json and shorten directory name when loading multiple pcd
  * Remove erasing last \_
  * Modify concatenating file path
  * Apply Format
  * Add hash_library_vendor to build_depends.repos
  * Modify include way
  * Change function and variable names
  * Use return
  * Remove unnecessary input variable
  * Use unique_ptr
  * Rename digestMd5 to digest_md5
  * Modify variable name
  * Remove file.close()
  * Use hash of json
  * Read hash of json directory
  * Add newline to package.xml
  * Add isPcdFile
  * Fix pre-commit
  * Use icPcdFile when giving file of pcd
  * Feature/add virtual traffic light planner (`#1588 <https://github.com/autowarefoundation/autoware.universe/issues/1588>`_)
  * Fix deprecated constant of transient local (`#1994 <https://github.com/autowarefoundation/autoware.universe/issues/1994>`_)
  * Fix lint errors in lanelet2_extension (`#2028 <https://github.com/autowarefoundation/autoware.universe/issues/2028>`_)
  * add sort-package-xml hook in pre-commit (`#1881 <https://github.com/autowarefoundation/autoware.universe/issues/1881>`_)
  * add sort xml hook in pre-commit
  * change retval to exit_status
  * rename
  * add prettier plugin-xml
  * use early return
  * add license note
  * add tier4 license
  * restore prettier
  * change license order
  * move local hooks to public repo
  * move prettier-xml to pre-commit-hooks-ros
  * update version for bug-fix
  * apply pre-commit
  * Revert "[map_loader] modify colors for lane markers for better visualization (`#398 <https://github.com/autowarefoundation/autoware.universe/issues/398>`_)" (`#2063 <https://github.com/autowarefoundation/autoware.universe/issues/2063>`_)
  This reverts commit 046dc9a770bf03fb8813ddf6aa1b2f05e9357b67.
  * Fix elevation_map_loader downsample (`#2055 <https://github.com/autowarefoundation/autoware.universe/issues/2055>`_)
  * Add elevation_map data dir (`#2093 <https://github.com/autowarefoundation/autoware.universe/issues/2093>`_)
  * Minor fixes of map_loader's README (`#2116 <https://github.com/autowarefoundation/autoware.universe/issues/2116>`_)
  * Minor fixes of map_loader's README
  * Fix map_loader run command
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
  * Fix elevation_map hash due to mutiple slashes of pcd path (`#2192 <https://github.com/autowarefoundation/autoware.universe/issues/2192>`_)
  * Fix elevation_map hash due to mutiple slashes of pcd path
  * Use filesystem lexically_normal
  * Fix broken links of images on lanelet2_extension docs (`#2206 <https://github.com/autowarefoundation/autoware.universe/issues/2206>`_)
  * Add lanelet XML API (`#2262 <https://github.com/autowarefoundation/autoware.universe/issues/2262>`_)
  * show traffic light id marker (`#1554 <https://github.com/autowarefoundation/autoware.universe/issues/1554>`_) (`#1678 <https://github.com/autowarefoundation/autoware.universe/issues/1678>`_)
  * show traffic light id
  * fix typo
  Co-authored-by: satoshi-ota <satoshi.ota@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: satoshi-ota <satoshi.ota@gmail.com>
  * Feature/porting behavior path planner (`#1645 <https://github.com/autowarefoundation/autoware.universe/issues/1645>`_)
  * Add behavior path planner pkg with Lane Change (`#1525 <https://github.com/autowarefoundation/autoware.universe/issues/1525>`_)
  * add lanelet extension funcs
  * add planning msgs for FOA
  * add behavior_path_planner pkg
  * apply clang format
  * add error handling for config load failure
  * replace word: foa with remote control
  * add readme
  * use pointer for return value of path
  * fix hz
  * remove debug print
  * remove shide-shift & avoidance related files
  * Clip path by goal
  * add build depend for behavior tree cpp
  * temporally disable lint test in lanelet2_extension
  Co-authored-by: rej55 <rej55.g@gmail.com>
  * Add avoidance module in behavior_path_planner (`#1528 <https://github.com/autowarefoundation/autoware.universe/issues/1528>`_)
  * Revert "remove shide-shift & avoidance related files"
  This reverts commit d819ea0291fca251012e4b9ffd16de3896830aa2.
  * refactor findNewShiftPoint func
  * remove duplicated decleration
  * fix barkward length issue
  - add clipPathLenght func in avoidance
  * refactor:
  - translate english
  - minor modification for traffic distance
  * support debug marker in behavior_path_planner
  * clean up side shift module
  * change topic name
  * remove japanese
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/include/behavior_path_planner/scene_module/side_shift/side_shift_module.hpp
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix typo
  * remove unused var
  * adress reviewer comments:
  - add const for variables
  - add comment
  - fix typo
  * fix typo
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * Replace behavior_path utilities with autoware_utils (`#1532 <https://github.com/autowarefoundation/autoware.universe/issues/1532>`_)
  * replace calcDistance
  * replace arange
  * replave convertToEigenPt with autoware_utils::fromMsg
  * replace normalizeRadian
  * cosmetic change
  * import `#1526 <https://github.com/autowarefoundation/autoware.universe/issues/1526>`_ into behavior path planner (`#1531 <https://github.com/autowarefoundation/autoware.universe/issues/1531>`_)
  * Fix/behavior path empty path output guard (`#1536 <https://github.com/autowarefoundation/autoware.universe/issues/1536>`_)
  * add guard
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/src/behavior_path_planner.cpp
  * fix lateral jerk calculation (`#1549 <https://github.com/autowarefoundation/autoware.universe/issues/1549>`_)
  * fix: error handling on exception in behavior_path_planner (`#1551 <https://github.com/autowarefoundation/autoware.universe/issues/1551>`_)
  * Fix ignore too steep avoidance path (`#1550 <https://github.com/autowarefoundation/autoware.universe/issues/1550>`_)
  * ignore too steep path
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/src/scene_module/avoidance/avoidance_module.cpp
  * parametrize lateral jerk limit
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/include/behavior_path_planner/scene_module/avoidance/avoidance_module.hpp
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * use offsetNoThrow and add error log (`#1615 <https://github.com/autowarefoundation/autoware.universe/issues/1615>`_)
  * Ignore object ahead goal for avoidance (`#1618 <https://github.com/autowarefoundation/autoware.universe/issues/1618>`_)
  * Ignore object ahead goal for avoidance
  * Add flag
  * Fix position of definition of goal_pose
  * Fix arclength calculation
  * Fix position of definition of goal_pose
  * fix intersection stop line (`#1636 <https://github.com/autowarefoundation/autoware.universe/issues/1636>`_)
  * fix intersection stop line
  * fix typo
  * add document (`#1635 <https://github.com/autowarefoundation/autoware.universe/issues/1635>`_)
  * Port behavior path planner to ros2
  * Apply lint
  * Fix typo
  * Fix map qos
  * debug slope calculation in behavior (`#1566 <https://github.com/autowarefoundation/autoware.universe/issues/1566>`_)
  * update
  * update
  * revert change of autoware_utils
  * define getPose in behavior_path_planner
  * update
  * update
  * update
  * update
  * interpolate z in obstacle_avoidance_planner
  * update velocity controller
  * fix detection area and scene
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_path_planner/src/utilities.cpp
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * update comment in velocity controller
  * remove debug print
  * update
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Address review: Fix config file name
  * pre-commit fixes
  * Fix redeclaring parameters
  * Add missing tf2 geometry function
  * Apply lint
  * Fix rclcpp Time initialization
  * Use now() instead of msg stamp
  * Use throttle output in getExpandedLanelet
  * Add missing const
  * Fix lint
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: rej55 <rej55.g@gmail.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  * change type of traffic light marker (SPHERE_LIST->SPHERE) (`#1789 <https://github.com/autowarefoundation/autoware.universe/issues/1789>`_)
  * fix alpha (`#1797 <https://github.com/autowarefoundation/autoware.universe/issues/1797>`_)
  * Feature/improve intersection detection area (`#1958 <https://github.com/autowarefoundation/autoware.universe/issues/1958>`_)
  * exclude ego_lanes from detection_area
  * add empty handling
  * remove unused function
  * Fix for uncrustify
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Apply format (`#1999 <https://github.com/autowarefoundation/autoware.universe/issues/1999>`_)
  Fix cpplint
  * Feature/expand drivable area (`#1812 <https://github.com/autowarefoundation/autoware.universe/issues/1812>`_)
  * check if ego lane has adjacent lane or not
  * expand drivable area by using lanelet
  * remove unnecessary operator
  * use extra drivable area
  * fix variable names
  * fix indent
  * get polygon by id
  * fix variable name
  * remove redundant logic
  * update area name
  * disable expand by default
  Co-authored-by: satoshi-ota <satoshi.ota@gmail.com>
  * add shoulder road lanelets (`#2121 <https://github.com/autowarefoundation/autoware.universe/issues/2121>`_)
  * add shoulder lanelets
  * Update map/lanelet2_extension/lib/query.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update map/lanelet2_extension/lib/visualization.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update map/lanelet2_extension/include/lanelet2_extension/visualization/visualization.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update map/lanelet2_extension/include/lanelet2_extension/visualization/visualization.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update map/lanelet2_extension/lib/visualization.cpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Feature/no stopping area reg element (`#2144 <https://github.com/autowarefoundation/autoware.universe/issues/2144>`_)
  * add no stopping area to ll2
  * add no stopping area visualization
  * add no stopping area marker to RVIZ
  * make no stopping area stop line as optional
  * Update map/map_loader/src/lanelet2_map_loader/lanelet2_map_visualization_node.cpp
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Add document for new map format (`#1778 <https://github.com/autowarefoundation/autoware.universe/issues/1778>`_)
  * add roadside lane doc
  * fix typo
  * fix typo
  * fix typo
  * fix typo
  * Add markdown lint
  * add reason for new subtype definition
  * fix typo
  Co-authored-by: kyoichi <kyoichi.sugahara@tier4.jp>
  * Change formatter to clang-format and black (`#2332 <https://github.com/autowarefoundation/autoware.universe/issues/2332>`_)
  * Revert "Temporarily comment out pre-commit hooks"
  This reverts commit 748e9cdb145ce12f8b520bcbd97f5ff899fc28a3.
  * Replace ament_lint_common with autoware_lint_common
  * Remove ament_cmake_uncrustify and ament_clang_format
  * Apply Black
  * Apply clang-format
  * Fix build errors
  * Fix for cpplint
  * Fix include double quotes to angle brackets
  * Apply clang-format
  * Fix build errors
  * Add COLCON_IGNORE (`#500 <https://github.com/autowarefoundation/autoware.universe/issues/500>`_)
  * port lanelet2_extension (`#483 <https://github.com/autowarefoundation/autoware.universe/issues/483>`_)
  * port with auto_msgs
  * remove COLCON_IGNORE
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  * port map loader (`#508 <https://github.com/autowarefoundation/autoware.universe/issues/508>`_)
  * remove COLCON_IGNORE in system_packages and map_tf_generator (`#532 <https://github.com/autowarefoundation/autoware.universe/issues/532>`_)
  * add readme (`#561 <https://github.com/autowarefoundation/autoware.universe/issues/561>`_)
  * fix old description
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Kosuke Murakami <kosuke.murakami@tier4.jp>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Esteve Fernandez <esteve@apache.org>
  Co-authored-by: nik-tier4 <71747268+nik-tier4@users.noreply.github.com>
  Co-authored-by: isamu-takagi <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Makoto Tokunaga <vios-fish@users.noreply.github.com>
  Co-authored-by: Adam Dąbrowski <adam.dabrowski@robotec.ai>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: satoshi-ota <satoshi.ota@gmail.com>
  Co-authored-by: rej55 <rej55.g@gmail.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Sugatyon <32741405+Sugatyon@users.noreply.github.com>
  Co-authored-by: kyoichi <kyoichi.sugahara@tier4.jp>
  Co-authored-by: Takayuki Murooka <takayuki.murooka@tier4.jp>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* Contributors: Ahmed Ebrahim, Anh Nguyen, Daisuke Nishimatsu, Hiroki OTA, Kah Hooi Tan, Kenji Miyake, Kento Yabuuchi, Kosuke Takeuchi, M. Fatih Cırıt, Maxime CLEMENT, Ryohsuke Mitsudome, RyuYamamoto, SakodaShintaro, Satoshi OTA, Shohei Sakai, Takagi, Isamu, Takamasa Horibe, Takayuki Murooka, Takeshi Miura, Tomohito ANDO, Tomoya Kimura, Vincent Richard, Yamato Ando, Yukihiro Saito, beyzanurkaya, kminoda, melike, melike tanrikulu, pre-commit-ci[bot], taikitanaka3
