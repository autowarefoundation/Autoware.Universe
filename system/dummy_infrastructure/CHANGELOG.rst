^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package dummy_infrastructure
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(dummy_infrastructure): rework parameters (`#5275 <https://github.com/autowarefoundation/autoware.universe/issues/5275>`_)
* Contributors: Yuntianyi Chen, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
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
* build(dummy_infrastructure): add build dependency (`#2739 <https://github.com/autowarefoundation/autoware.universe/issues/2739>`_)
* feat(dummy_infrastructure): change to multiple virtual signal state outputs (`#1717 <https://github.com/autowarefoundation/autoware.universe/issues/1717>`_)
  * Added parameter to dummy_infrastructure.param.yaml
  * Modified dummy infrastructure
  * Modified dummy infrastructure for multiple commands
  * update dummy_infrastructure
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci: check include guard (`#438 <https://github.com/autowarefoundation/autoware.universe/issues/438>`_)
  * ci: check include guard
  * apply pre-commit
  * Update .pre-commit-config.yaml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: pre-commit
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add dummy_infrastructure package (`#19 <https://github.com/autowarefoundation/autoware.universe/issues/19>`_)
  * Feature/add virtual traffic light planner (`#1588 <https://github.com/autowarefoundation/autoware.universe/issues/1588>`_)
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
  * delete COLCON_IGNORE (`#540 <https://github.com/autowarefoundation/autoware.universe/issues/540>`_)
  * add readme [dummy infrastructure] (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
  * add readme dummy infra
  * fix lint
  * update readme
  Co-authored-by: taikitanaka3 <taiki.tanaka@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <taiki.tanaka@tier4.jp>
* Contributors: Esteve Fernandez, Kenji Miyake, Takagi, Isamu, Tomoya Kimura, Vincent Richard, Yohei Mishina
