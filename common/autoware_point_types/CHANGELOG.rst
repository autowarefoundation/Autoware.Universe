^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_point_types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
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
* chore: updated maintainers for the autoware_point_types package (`#7797 <https://github.com/autowarefoundation/autoware.universe/issues/7797>`_)
* docs(common): adding .pages file (`#7148 <https://github.com/autowarefoundation/autoware.universe/issues/7148>`_)
  * docs(common): adding .pages file
  * fix naming
  * fix naming
  * fix naming
  * include main page plus explanation to autoware tools
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kenzo Lobos Tsunekawa, Yutaka Kondo, Zulfaqar Azmi

0.26.0 (2024-04-03)
-------------------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* feat: add blockage diagnostics (`#461 <https://github.com/autowarefoundation/autoware.universe/issues/461>`_)
  * feat!: add blockage diagnostic
  * fix: typo
  * docs: add documentation
  * ci(pre-commit): autofix
  * fix: typo
  * ci(pre-commit): autofix
  * fix: typo
  * chore: add adjustable param
  * ci(pre-commit): autofix
  * feat!: add blockage diagnostic
  * fix: typo
  * docs: add documentation
  * ci(pre-commit): autofix
  * fix: typo
  * ci(pre-commit): autofix
  * fix: typo
  * chore: add adjustable param
  * ci(pre-commit): autofix
  * chore: rearrange header file
  * chore: fix typo
  * chore: rearrange header
  * fix: revert accident change
  * chore: fix typo
  * docs: add limits
  * chore: check overflow
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* ci: check include guard (`#438 <https://github.com/autowarefoundation/autoware.universe/issues/438>`_)
  * ci: check include guard
  * apply pre-commit
  * Update .pre-commit-config.yaml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix: pre-commit
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat: add point_types for wrapper (`#784 <https://github.com/autowarefoundation/autoware.universe/issues/784>`_) (`#215 <https://github.com/autowarefoundation/autoware.universe/issues/215>`_)
  * add point_types
  * Revert "add point_types"
  This reverts commit 5810000cd1cbd876bc22372e2bb74ccaca06187b.
  * create autoware_point_types pkg
  * add include
  * add cmath
  * fix author
  * fix bug
  * define epsilon as argument
  * add test
  * remove unnamed namespace
  * update test
  * fix test name
  * use std::max
  * change comparison method
  * remove unnencessary line
  * fix test
  * fix comparison method name
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
* Contributors: Daisuke Nishimatsu, Kenji Miyake, Maxime CLEMENT, Takagi, Isamu, Vincent Richard, badai nguyen
