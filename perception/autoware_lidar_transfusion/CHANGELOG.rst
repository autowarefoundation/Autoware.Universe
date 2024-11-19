^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_lidar_transfusion
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
* refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace (`#9099 <https://github.com/autowarefoundation/autoware.universe/issues/9099>`_)
  * refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace
  * refactor(tensorrt_common): directory structure
  * style(pre-commit): autofix
  * fix(tensorrt_common): correct package name for logging
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* fix(autoware_lidar_transfusion): set tensor names by matching with predefined values. (`#9057 <https://github.com/autowarefoundation/autoware.universe/issues/9057>`_)
  * set tensor order using api
  * style(pre-commit): autofix
  * fix tensor order
  * style(pre-commit): autofix
  * style fix
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_lidar _ransfusion): fix 3d bounding box orientation (`#9052 <https://github.com/autowarefoundation/autoware.universe/issues/9052>`_)
  * fix bbox orientation
  * revert newline changes
  * change kernel
  ---------
  Co-authored-by: Amadeusz Szymko <amadeusz.szymko.2@tier4.jp>
* feat(autoware_lidar_transfusion): shuffled points before feeding them to the model (`#8815 <https://github.com/autowarefoundation/autoware.universe/issues/8815>`_)
  feat: shuffling points before feeding them into the model to achieve random sampling into the voxels
  Co-authored-by: Amadeusz Szymko <amadeusz.szymko.2@tier4.jp>
* refactor(autoware_lidar_transfusion): split config (`#8205 <https://github.com/autowarefoundation/autoware.universe/issues/8205>`_)
  * refactor(autoware_lidar_transfusion): split config
  * style(pre-commit): autofix
  * chore(autoware_lidar_transfusion): bypass schema CI workflow
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* fix(autoware_lidar_transfusion): place device vector in CUDA device system (`#8273 <https://github.com/autowarefoundation/autoware.universe/issues/8273>`_)
* fix(lidar_transfusion): commented tests were out of date (`#8116 <https://github.com/autowarefoundation/autoware.universe/issues/8116>`_)
  chore: commented tests were out of date
* refactor(lidar_transfusion)!: fix namespace and directory structure (`#8022 <https://github.com/autowarefoundation/autoware.universe/issues/8022>`_)
  * add prefix
  * add prefix in code owner
  * style(pre-commit): autofix
  * fix launcher
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Amadeusz Szymko <amadeusz.szymko.2@tier4.jp>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Kenzo Lobos Tsunekawa, Masato Saeki, Samrat Thapa, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
