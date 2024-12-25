^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_compare_map_segmentation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* chore(compare_map_segmentation): rename defined type (`#9181 <https://github.com/autowarefoundation/autoware.universe/issues/9181>`_)
* fix(cpplint): include what you use - perception (`#9569 <https://github.com/autowarefoundation/autoware.universe/issues/9569>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(compare_map_segmentation): add maintainer (`#9371 <https://github.com/autowarefoundation/autoware.universe/issues/9371>`_)
* fix(compare_map_segmentation): timer period mismatched with parameter (`#9259 <https://github.com/autowarefoundation/autoware.universe/issues/9259>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(autoware_compare_map_segmentation): fix cppcheck constVariableReference (`#9196 <https://github.com/autowarefoundation/autoware.universe/issues/9196>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo, badai nguyen

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
* fix(autoware_compare_map_segmentation): fix cppcheck constVariableReference (`#9196 <https://github.com/autowarefoundation/autoware.universe/issues/9196>`_)
* Contributors: Esteve Fernandez, Ryuta Kambe, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
* refactor(autoware_compare_map_segmentation): resolve clang-tidy error in autoware_compare_map_segmentation (`#9162 <https://github.com/autowarefoundation/autoware.universe/issues/9162>`_)
  * refactor(autoware_compare_map_segmentation): resolve clang-tidy error in autoware_compare_map_segmentation
  * style(pre-commit): autofix
  * include message_filters as SYSTEM
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(compare_map_segmentation): add missing mutex lock (`#9097 <https://github.com/autowarefoundation/autoware.universe/issues/9097>`_)
  * fix(compare_map_segmentation): missing mutux
  * chore: rename mutex\_
  * fix: remove unnecessary mutex
  * fix: typos
  * chore: minimize mutex scope
  * chore: change to lock_guard
  * fix: check tree initialization
  * fix: memory ordering
  * fix: replace all static_map_loader_mutex\_
  ---------
* fix(compare_map_segmentation): throw runtime error when using non-split map pointcloud for DynamicMapLoader (`#9024 <https://github.com/autowarefoundation/autoware.universe/issues/9024>`_)
  * fix(compare_map_segmentation): throw runtime error when using non-split map pointcloud for DynamicMapLoader
  * chore: typo
  * fix: launch
  * Update perception/autoware_compare_map_segmentation/schema/voxel_distance_based_compare_map_filter.schema.json
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
  * fix: change to RCLCPP_ERROR
  ---------
  Co-authored-by: Yoshi Ri <yoshiyoshidetteiu@gmail.com>
* chore(compare_map_segmentation): add node tests (`#8907 <https://github.com/autowarefoundation/autoware.universe/issues/8907>`_)
  * chore(compare_map_segmentation): add test for voxel_based_compare_map_filter
  * feat: add test for other compare map filter
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_compare_map_segmentation): typo bug fix (`#8939 <https://github.com/autowarefoundation/autoware.universe/issues/8939>`_)
  fix(compare_map_filter): typo bug fix
* fix(autoware_compare_map_segmentation): fix unusedFunction (`#8725 <https://github.com/autowarefoundation/autoware.universe/issues/8725>`_)
  fix:unusedFunction
* fix(compare_map_segmentation): use squared distance to compare threshold (`#8744 <https://github.com/autowarefoundation/autoware.universe/issues/8744>`_)
  fix: use square distance to compare threshold
* fix(autoware_compare_map_segmentation): fix unusedFunction (`#8565 <https://github.com/autowarefoundation/autoware.universe/issues/8565>`_)
  fix:unusedFunction
* fix(autoware_compare_map_segmentation): fix cppcheck warnings of functionStatic (`#8263 <https://github.com/autowarefoundation/autoware.universe/issues/8263>`_)
  * fix: deal with functionStatic warnings
  * fix: deal with functionStatic warnings
  * fix: remove unnecessary const
  * fix: build error
  ---------
* fix(autoware_compare_map_segmentation): fix uninitMemberVar (`#8338 <https://github.com/autowarefoundation/autoware.universe/issues/8338>`_)
  fix:uninitMemberVar
* fix(autoware_compare_map_segmentation): fix passedByValue (`#8233 <https://github.com/autowarefoundation/autoware.universe/issues/8233>`_)
  fix:passedByValue
* fix(autoware_compare_map_segmentation): fix redundantInitialization warning (`#8226 <https://github.com/autowarefoundation/autoware.universe/issues/8226>`_)
* revert: revert "refactor(autoware_map_msgs): modify pcd metadata msg (`#7852 <https://github.com/autowarefoundation/autoware.universe/issues/7852>`_)" (`#8180 <https://github.com/autowarefoundation/autoware.universe/issues/8180>`_)
* refactor(autoware_map_msgs): modify pcd metadata msg (`#7852 <https://github.com/autowarefoundation/autoware.universe/issues/7852>`_)
* refactor(compare_map_segmentation): add package name prefix of autoware\_ (`#8005 <https://github.com/autowarefoundation/autoware.universe/issues/8005>`_)
  * refactor(compare_map_segmentation): add package name prefix of autoware\_
  * docs: update Readme
  ---------
* Contributors: Esteve Fernandez, Ryohsuke Mitsudome, Ryuta Kambe, Yamato Ando, Yoshi Ri, Yukinari Hisaki, Yutaka Kondo, badai nguyen, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
