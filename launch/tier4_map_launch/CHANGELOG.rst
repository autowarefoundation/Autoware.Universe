^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_map_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Masaki Baba, Ryohsuke Mitsudome, Yutaka Kondo

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
* fix(tier4_map_launch): remove parameter use_intra_process (`#9138 <https://github.com/autowarefoundation/autoware.universe/issues/9138>`_)
* refactor(map_projection_loader)!: prefix package and namespace with autoware (`#8420 <https://github.com/autowarefoundation/autoware.universe/issues/8420>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* fix(map_tf_genrator): fix map launch prefix (`#8598 <https://github.com/autowarefoundation/autoware.universe/issues/8598>`_)
  fix(map_tf_geenrator): fix map launch prefix
* refactor(map_tf_generator)!: prefix package and namespace with autoware (`#8422 <https://github.com/autowarefoundation/autoware.universe/issues/8422>`_)
  * add autoware\_ prefix
  * add autoware\_ prefix
  ---------
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* chore(localization, map): remove maintainer (`#7940 <https://github.com/autowarefoundation/autoware.universe/issues/7940>`_)
* chore(map): udpate maintainer (`#7405 <https://github.com/autowarefoundation/autoware.universe/issues/7405>`_)
  udpate maintainer
* fix(tier4_map_launch): change log output (`#7289 <https://github.com/autowarefoundation/autoware.universe/issues/7289>`_)
  change log output from screen to both
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* Contributors: Kosuke Takeuchi, Masaki Baba, Yamato Ando, Yutaka Kondo, cyn-liu, kminoda

0.26.0 (2024-04-03)
-------------------
* refactor(map_tf_generator): rework parameters (`#6233 <https://github.com/autowarefoundation/autoware.universe/issues/6233>`_)
  * refactor(map_tf_generator): rework parameters
  * add config
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(map_projection_loader): rework parameters (`#6253 <https://github.com/autowarefoundation/autoware.universe/issues/6253>`_)
  * Extract all params in map_projection_loader to map_projection_loader.param.yaml
  Added launch argument map_projection_loader_param_path to map.launch.xml
  * Added map_projection_loader.schema.json.
  Modified README.md of map_projection_loader to read the schema.json file to construct the table of parameters
  * style(pre-commit): autofix
  * Fixed typo
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kotaro Yoshimoto <pythagora.yoshimoto@gmail.com>
* chore(tier4_map_launch): add maintainer (`#6284 <https://github.com/autowarefoundation/autoware.universe/issues/6284>`_)
* refactor(map_launch): use map.launch.xml instead of map.launch.py (`#6185 <https://github.com/autowarefoundation/autoware.universe/issues/6185>`_)
  * refactor(map_launch): use map.launch.xml instead of map.launch.py
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add maintainer in map packages (`#5865 <https://github.com/autowarefoundation/autoware.universe/issues/5865>`_)
  * add maintainer
  * modify map_tf_generator's maintainer
  ---------
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
* chore(tier4_map_launch): add lanelet2 config files to tier4_map_launch (`#2670 <https://github.com/autowarefoundation/autoware.universe/issues/2670>`_)
  * chore(tier4_map_launch): add lanelet2 config files to tier4_map_launch
  Update launch/tier4_map_launch/launch/map.launch.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  delete  lanelet2_map_projector type in launch
  remove config path
  * chore(tier4_map_launch): fix lanelet launch name
  ---------
* refactor(tier4_map_launch): remove unused config (`#2722 <https://github.com/autowarefoundation/autoware.universe/issues/2722>`_)
  * refactor(tier4_map_launch): remove unused config
  * load lanelet2 parameter from upper level
  * revert the addition of lanelet2 param
* revert(tier4_map_launch): move config back to autoware.universe (`#2561 <https://github.com/autowarefoundation/autoware.universe/issues/2561>`_)
  * revert(tier4_map_launch): move config back to autoware.universe
  * fix map.launch.xml
* feat(tier4_map_launch): remove configs and move to autoware_launch (`#2538 <https://github.com/autowarefoundation/autoware.universe/issues/2538>`_)
  * feat(tier4_map_launch): remove configs and move to autoware_launch
  * update readme
  * fix readme
  * remove config
  * update readme
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
* chore(tier4_map_launch): add maintainers (`#2416 <https://github.com/autowarefoundation/autoware.universe/issues/2416>`_)
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* feat(map_tf_generator)!: launching planning_simulator without pointcloud map (`#1216 <https://github.com/autowarefoundation/autoware.universe/issues/1216>`_)
  * feat(map_tf_generator): add vector map tf generator
  * fix(ad_service_state_monitor): rm unused cofig param
  * chore: change launching vector_map_tf_generator
  * docs: update readme
  * refactor: rename map_tf_generator -> pcd_map_tf_generator
  * fix: build error
  * Update map/map_tf_generator/Readme.md
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update map/map_tf_generator/src/vector_map_tf_generator_node.cpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update map/map_tf_generator/Readme.md
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update map/map_tf_generator/Readme.md
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat: add parameter argument for lanelet2_map_loader (`#954 <https://github.com/autowarefoundation/autoware.universe/issues/954>`_)
  * feat: add parameter argument for lanelet2_map_loader
  * feat: add comment
* refactor: tier4_map_launch (`#953 <https://github.com/autowarefoundation/autoware.universe/issues/953>`_)
  * refactor: tier4_map_launch
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
* Contributors: Hiroki OTA, Kenji Miyake, Kento Yabuuchi, Shumpei Wakabayashi, TaikiYamada4, Takayuki Murooka, Tomoya Kimura, Vincent Richard, Yamato Ando, Yukihiro Saito, kminoda, melike, melike tanrikulu
