^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_detected_object_validation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - perception (`#9569 <https://github.com/autowarefoundation/autoware.universe/issues/9569>`_)
* feat(object_lanelet_filter): add configurable margin for object lanel… (`#9240 <https://github.com/autowarefoundation/autoware.universe/issues/9240>`_)
  * feat(object_lanelet_filter): add configurable margin for object lanelet filter
  modified:   perception/autoware_detected_object_validation/src/lanelet_filter/lanelet_filter.cpp
  * feat(object_lanelet_filter): add condition to check wheter polygon is empty in debug mode
  * feat(object_lanelet_filter): fix cppcheck
  * fix: brig back missing type definition
  * feat: add stop watch for processing time in object lanelet filter
  * feat(object_lanelet_filter): remove extra comment line
  * feat(_object_lanelet_filter): udpate readme
  * style(pre-commit): autofix
  * Update perception/autoware_detected_object_validation/src/lanelet_filter/debug.cpp
  Co-authored-by: Shintaro Tomie <58775300+Shin-kyoto@users.noreply.github.com>
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Sebastian Zęderowski <szederowski@autonomous-systems.pl>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Shintaro Tomie <58775300+Shin-kyoto@users.noreply.github.com>
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_detected_object_validation): fix bugprone-incorrect-roundings (`#9220 <https://github.com/autowarefoundation/autoware.universe/issues/9220>`_)
  fix: bugprone-incorrect-roundings
* fix(autoware_detected_object_validation): fix clang-diagnostic-error (`#9215 <https://github.com/autowarefoundation/autoware.universe/issues/9215>`_)
  fix: clang-c-error
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Sebastian Zęderowski, Yutaka Kondo, kobayu858

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_detected_object_validation): fix bugprone-incorrect-roundings (`#9220 <https://github.com/autowarefoundation/autoware.universe/issues/9220>`_)
  fix: bugprone-incorrect-roundings
* fix(autoware_detected_object_validation): fix clang-diagnostic-error (`#9215 <https://github.com/autowarefoundation/autoware.universe/issues/9215>`_)
  fix: clang-c-error
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo, kobayu858

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* refactor(detected_object_validation): rework parameters (`#7750 <https://github.com/autowarefoundation/autoware.universe/issues/7750>`_)
  * refactor(detected_object_validation): rework parameters
  * style(pre-commit): autofix
  * Update perception/detected_object_validation/schema/object_lanelet_filter.schema.json
  Co-authored-by: badai nguyen  <94814556+badai-nguyen@users.noreply.github.com>
  * Update perception/detected_object_validation/schema/object_position_filter.schema.json
  Co-authored-by: badai nguyen  <94814556+badai-nguyen@users.noreply.github.com>
  * refactor(detected_object_validation): rework parameters
  * refactor(detected_object_validation): rework parameters
  * refactor(detected_object_validation): rework parameters
  * style(pre-commit): autofix
  * refactor(detected_object_validation): rework parameters
  * refactor(detected_object_validation): rework parameters
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_detected_object_validation): fix functionStatic (`#8482 <https://github.com/autowarefoundation/autoware.universe/issues/8482>`_)
  fix:functionStatic
* fix(autoware_detected_object_validation): fix cppcheck warnings of functionStatic (`#8256 <https://github.com/autowarefoundation/autoware.universe/issues/8256>`_)
  fix: deal with functionStatic warnings
* fix(autoware_detected_object_validation): fix functionConst (`#8285 <https://github.com/autowarefoundation/autoware.universe/issues/8285>`_)
  fix: functionConst
* perf(autoware_detected_object_validation): reduce lanelet_filter processing time  (`#8240 <https://github.com/autowarefoundation/autoware.universe/issues/8240>`_)
  * add local r-tree for fast searching
  change to _func\_\_
  add more debug
  use local rtree
  fix
  tmp update
  fix bug
  clean unused
  clean up
  * clean up
  * style(pre-commit): autofix
  * chore: Optimize object filtering and improve performance
  The code changes in `lanelet_filter.cpp` optimize the object filtering process by using the `empty()` function instead of checking the size of the `transformed_objects.objects` vector. This change improves performance and simplifies the code logic.
  Refactor the code to use `empty()` instead of `size()` for checking if the `transformed_objects.objects` vector is empty. This change improves readability and performance.
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
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
* refactor(detected_object_validation)!: add package name prefix of autoware\_ (`#8122 <https://github.com/autowarefoundation/autoware.universe/issues/8122>`_)
  refactor: rename detected_object_validation to autoware_detected_object_validation
* Contributors: Batuhan Beytekin, Esteve Fernandez, Hayate TOBA, Masaki Baba, Taekjin LEE, Yoshi Ri, Yutaka Kondo, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
