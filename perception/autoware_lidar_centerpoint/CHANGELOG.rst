^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_lidar_centerpoint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* fix(lidar_centerpoint): non-maximum suppression target decision logic (`#9595 <https://github.com/autowarefoundation/autoware.universe/issues/9595>`_)
  * refactor(lidar_centerpoint): optimize non-maximum suppression search distance calculation
  * feat(lidar_centerpoint): do not suppress if one side of the object is pedestrian
  * style(pre-commit): autofix
  * refactor(lidar_centerpoint): remove unused variables
  * refactor: remove unused variables
  fix: implement non-maximum suppression logic to the transfusion
  refactor: remove unused parameter iou_nms_target_class_names
  Revert "fix: implement non-maximum suppression logic to the transfusion"
  This reverts commit b8017fc366ec7d67234445ef5869f8beca9b6f45.
  fix: revert transfusion modification
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* fix: fix ticket links in CHANGELOG.rst (`#9588 <https://github.com/autowarefoundation/autoware.universe/issues/9588>`_)
* fix(cpplint): include what you use - perception (`#9569 <https://github.com/autowarefoundation/autoware.universe/issues/9569>`_)
* fix(autoware_lidar_centerpoint): fix clang-diagnostic-delete-abstract-non-virtual-dtor (`#9515 <https://github.com/autowarefoundation/autoware.universe/issues/9515>`_)
* feat(autoware_lidar_centerpoint): added a check to notify if we are dropping pillars (`#9488 <https://github.com/autowarefoundation/autoware.universe/issues/9488>`_)
  * feat: added a check to notify if we are dropping pillars
  * chore: updated text
  * chore: throttled the message
  ---------
* fix(autoware_lidar_centerpoint): fix clang-diagnostic-unused-private-field (`#9471 <https://github.com/autowarefoundation/autoware.universe/issues/9471>`_)
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kenzo Lobos Tsunekawa, M. Fatih Cırıt, Ryohsuke Mitsudome, Taekjin LEE, Yutaka Kondo, kobayu858

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
* refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace (`#9099 <https://github.com/autowarefoundation/autoware.universe/issues/9099>`_)
  * refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace
  * refactor(tensorrt_common): directory structure
  * style(pre-commit): autofix
  * fix(tensorrt_common): correct package name for logging
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* fix(autoware_lidar_centerpoint): fix twist covariance orientation (`#8996 <https://github.com/autowarefoundation/autoware.universe/issues/8996>`_)
  * fix(autoware_lidar_centerpoint): fix covariance converter considering the twist covariance matrix is based on the object coordinate
  fix style
  * fix: update test of box3DToDetectedObject function
  ---------
* fix(autoware_lidar_centerpoint): convert object's velocity to follow its definition (`#8980 <https://github.com/autowarefoundation/autoware.universe/issues/8980>`_)
  * fix: convert object's velocity to follow its definition in box3DToDetectedObject function
  * Update perception/autoware_lidar_centerpoint/lib/ros_utils.cpp
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* feat(autoware_lidar_centerpoint): shuffled points before feeding them to the model (`#8814 <https://github.com/autowarefoundation/autoware.universe/issues/8814>`_)
  * feat: shuffling points before feeding them into the model to achieve uniform sampling into the voxels
  * Update perception/autoware_lidar_centerpoint/src/node.cpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Update perception/autoware_lidar_centerpoint/src/node.cpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Update perception/autoware_lidar_centerpoint/lib/centerpoint_trt.cpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * Update perception/autoware_lidar_centerpoint/include/autoware/lidar_centerpoint/centerpoint_config.hpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* refactor(autoware_lidar_centerpoint): use std::size_t instead of size_t (`#8820 <https://github.com/autowarefoundation/autoware.universe/issues/8820>`_)
  * refactor(autoware_lidar_centerpoint): use std::size_t instead of size_t
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(autoware_lidar_centerpoint): add centerpoint sigma parameter (`#8731 <https://github.com/autowarefoundation/autoware.universe/issues/8731>`_)
  add centerpoint sigma parameter
* fix(autoware_lidar_centerpoint): fix unusedFunction (`#8572 <https://github.com/autowarefoundation/autoware.universe/issues/8572>`_)
  fix:unusedFunction
* fix(autoware_lidar_centerpoint): place device vector in CUDA device system (`#8272 <https://github.com/autowarefoundation/autoware.universe/issues/8272>`_)
* docs(centerpoint): add description for ml package params (`#8187 <https://github.com/autowarefoundation/autoware.universe/issues/8187>`_)
* chore(autoware_lidar_centerpoint): updated tests (`#8158 <https://github.com/autowarefoundation/autoware.universe/issues/8158>`_)
  chore: updated centerpoin tests. they are currently commented out but they were not compiling (forgot to update them when I added the new cloud capacity parameter)
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
* Contributors: Amadeusz Szymko, Esteve Fernandez, Kenzo Lobos Tsunekawa, Masato Saeki, Taekjin LEE, Yoshi Ri, Yutaka Kondo, kminoda, kobayu858

0.26.0 (2024-04-03)
-------------------
