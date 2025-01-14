^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_api_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* Contributors: Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* chore: set log level of debug printing in rviz plugin to DEBUG (`#5996 <https://github.com/autowarefoundation/autoware.universe/issues/5996>`_)
* docs: add readme for interface packages (`#4235 <https://github.com/autowarefoundation/autoware.universe/issues/4235>`_)
  add readme for interface packages
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* refactor(tier4_api_utils): apply clang-tidy (`#1663 <https://github.com/autowarefoundation/autoware.universe/issues/1663>`_)
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: sync files (`#629 <https://github.com/autowarefoundation/autoware.universe/issues/629>`_)
  * chore: sync files
  * ci(pre-commit): autofix
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* Contributors: Hiroki OTA, Kenji Miyake, Maxime CLEMENT, Takagi, Isamu, Takayuki Murooka, Tomoya Kimura, Vincent Richard, awf-autoware-bot[bot]
