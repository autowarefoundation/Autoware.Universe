^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package velodyne_monitor
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
* feat(velodyne_monitor): componentize node (`#7201 <https://github.com/autowarefoundation/autoware.universe/issues/7201>`_)
* Contributors: Takagi, Isamu, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* refactor(system-velodyne-monitor): rework parameters (`#5667 <https://github.com/autowarefoundation/autoware.universe/issues/5667>`_)
  system-velodyne-monitor
* docs(velodyne_monitor): rename readme to README (`#4224 <https://github.com/autowarefoundation/autoware.universe/issues/4224>`_)
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(velodyne_monitor): add fmt package to dependencies (`#3069 <https://github.com/autowarefoundation/autoware.universe/issues/3069>`_)
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
* docs(velodyne_monitor): add known limits (`#1802 <https://github.com/autowarefoundation/autoware.universe/issues/1802>`_)
  * docs(velodyne_monitor): add known limits
  * Update Readme.md
* fix(velodyne_monitor): add temp_hot\_*** to 20 (`#1744 <https://github.com/autowarefoundation/autoware.universe/issues/1744>`_)
  fix(velodyne_monitor): add 20 to temp_hot\_***
* fix(velodyne monitor): fix warning and error threshold of hot temperature (`#1623 <https://github.com/autowarefoundation/autoware.universe/issues/1623>`_)
  * fix(velodyne_monitor): fix warning and error threshold of hot temperature
  * doc: update README
  * doc: fix typo
  * doc: update README
  * feat: add config file for each model
  * Update Readme.md
  * Update Readme.md
  * fix: typo
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add velodyne_monitor package (`#17 <https://github.com/autowarefoundation/autoware.universe/issues/17>`_)
  * Ros2 v0.8.0 velodyne monitor (`#285 <https://github.com/autowarefoundation/autoware.universe/issues/285>`_)
  * Rename ROS-related .yaml to .param.yaml (`#352 <https://github.com/autowarefoundation/autoware.universe/issues/352>`_)
  * Rename ROS-related .yaml to .param.yaml
  * Remove prefix 'default\_' of yaml files
  * Rename vehicle_info.yaml to vehicle_info.param.yaml
  * Rename diagnostic_aggregator's param files
  * Fix overlooked parameters
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Unify Apache-2.0 license name (`#1242 <https://github.com/autowarefoundation/autoware.universe/issues/1242>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Add exception handling for extract_json() (`#1779 <https://github.com/autowarefoundation/autoware.universe/issues/1779>`_)
  * Add exception handling for extract_json()
  * Add diagnostics error when catching exception
  Co-authored-by: Takayuki AKAMINE <takayuki.akamine@tier4.jp>
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * Fix compiler warnings (`#1837 <https://github.com/autowarefoundation/autoware.universe/issues/1837>`_)
  * Fix -Wunused-private-field
  * Fix -Wunused-variable
  * Fix -Wformat-security
  * Fix -Winvalid-constexpr
  * Fix -Wdelete-non-abstract-non-virtual-dtor
  * Fix -Wdelete-abstract-non-virtual-dtor
  * Fix -Winconsistent-missing-override
  * Fix -Wrange-loop-construct
  * Fix "invalid application of 'sizeof' to an incomplete type"
  * Ignore -Wgnu-anonymous-struct and -Wnested-anon-types
  * Fix lint
  * Ignore -Wno-deprecated-declarations in CUDA-related packages
  * Fix mistake
  * Fix -Wunused-parameter
  * Remove duplicated update (`#2072 <https://github.com/autowarefoundation/autoware.universe/issues/2072>`_) (`#2084 <https://github.com/autowarefoundation/autoware.universe/issues/2084>`_)
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Fix velodyne monitor config file variable name (`#2090 <https://github.com/autowarefoundation/autoware.universe/issues/2090>`_) (`#2092 <https://github.com/autowarefoundation/autoware.universe/issues/2092>`_)
  Co-authored-by: j4tfwm6z <proj-jpntaxi@tier4.jp>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: j4tfwm6z <proj-jpntaxi@tier4.jp>
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
  * remove COLCON_IGNORE in system_packages and map_tf_generator (`#532 <https://github.com/autowarefoundation/autoware.universe/issues/532>`_)
  * [Velodyne monitor]add readme (`#570 <https://github.com/autowarefoundation/autoware.universe/issues/570>`_)
  * add readme
  * change the description
  * Update system/velodyne_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/velodyne_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * Update system/velodyne_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * velodyne -> velodyne lidar
  * Update system/velodyne_monitor/Readme.md
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update system/velodyne_monitor/Readme.md
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: Takayuki AKAMINE <takayuki.akamine@tier4.jp>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: autoware-iv-sync-ci[bot] <87871706+autoware-iv-sync-ci[bot]@users.noreply.github.com>
  Co-authored-by: j4tfwm6z <proj-jpntaxi@tier4.jp>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* Contributors: David Wong, Hiroki OTA, Kenji Miyake, Takamasa Horibe, Tomoya Kimura, Vincent Richard, karishma1911
