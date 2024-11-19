^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_lidar_centerpoint
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
