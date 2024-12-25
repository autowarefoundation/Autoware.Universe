^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package bluetooth_monitor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - system (`#9573 <https://github.com/autowarefoundation/autoware.universe/issues/9573>`_)
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

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
* fix(docs): fix file name for bluetooth monitor schema (`#8308 <https://github.com/autowarefoundation/autoware.universe/issues/8308>`_)
  * fix file name for schema
  * the variable name should be addresses instead
  ---------
* fix(bluetooth_monitor): fix unreadVariable (`#8371 <https://github.com/autowarefoundation/autoware.universe/issues/8371>`_)
  fix:unreadVariable
* fix(bluetooth_monitor): apply cppcheck-suppress for cstyleCast (`#7869 <https://github.com/autowarefoundation/autoware.universe/issues/7869>`_)
* Contributors: Koichi98, Yutaka Kondo, Yuxuan Liu, kobayu858

0.26.0 (2024-04-03)
-------------------
* refactor(bluetooth_monitor): rework parameters (`#5239 <https://github.com/autowarefoundation/autoware.universe/issues/5239>`_)
  * refactor(bluetooth_monitor): rework parameters
  * style(pre-commit): autofix
  * doc(bluetooth_monitor): fix default for integer parameters
  * doc(bluetooth_monitor): add default for parameter addresses
  * style(pre-commit): autofix
  * doc(bluetooth_monitor): fix parameter description
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore: sync files (`#3227 <https://github.com/autowarefoundation/autoware.universe/issues/3227>`_)
  * chore: sync files
  * style(pre-commit): autofix
  ---------
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* build(bluetooth_monitor): add build dependency (`#2738 <https://github.com/autowarefoundation/autoware.universe/issues/2738>`_)
* ci(pre-commit): format SVG files (`#2172 <https://github.com/autowarefoundation/autoware.universe/issues/2172>`_)
  * ci(pre-commit): format SVG files
  * ci(pre-commit): autofix
  * apply pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(bluetooth_monitor): run bluetooth monitor with new parameter (`#1111 <https://github.com/autowarefoundation/autoware.universe/issues/1111>`_)
  * feat(bluetooth_monitor): run bluetooth monitor with new parameter
  * ci(pre-commit): autofix
  * Fixed a build error in humble
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(bluetooth_monitor): add functionality to monitor Bluetooth connection (`#862 <https://github.com/autowarefoundation/autoware.universe/issues/862>`_)
  * feat(bluetooth_monitor): add functionality to monitor Bluetooth connection
  * ci(pre-commit): autofix
  * Fixed a typo
  * Add a dependency
  * Fixed pre-commit errors
  * ci(pre-commit): autofix
  * Fixed pre-commit errors
  * Fixed uncrustify errors
  * ci(pre-commit): autofix
  * use autoware_cmake
  * Fixed license, Fixed CMakeLists.txt, and Use register_node_macro
  * Fixed license
  * Fixed link title
  * changed the way to run l2ping
  * ci(pre-commit): autofix
  * fixed clang tidy error and  removed unnecessary dependencies in CMakeLists.txt
  * corrected dependency in package.xml
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kenji Miyake, Vincent Richard, Yuqi Huai, awf-autoware-bot[bot], ito-san
