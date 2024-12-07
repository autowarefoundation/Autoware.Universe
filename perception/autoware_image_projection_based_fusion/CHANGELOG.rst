^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_image_projection_based_fusion
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_image_projection_based_fusion): make optional to consider lens distortion in the point projection (`#9233 <https://github.com/autowarefoundation/autoware.universe/issues/9233>`_)
  chore: add point_project_to_unrectified_image parameter to fusion_common.param.yaml
* fix(autoware_image_projection_based_fusion): fix bugprone-misplaced-widening-cast (`#9226 <https://github.com/autowarefoundation/autoware.universe/issues/9226>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: clang-format
  ---------
* fix(autoware_image_projection_based_fusion): fix bugprone-misplaced-widening-cast (`#9229 <https://github.com/autowarefoundation/autoware.universe/issues/9229>`_)
  * fix: bugprone-misplaced-widening-cast
  * fix: clang-format
  ---------
* Contributors: Esteve Fernandez, Taekjin LEE, Yutaka Kondo, kobayu858

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
* fix(autoware_image_projection_based_fusion): pointpainting bug fix for point projection (`#9150 <https://github.com/autowarefoundation/autoware.universe/issues/9150>`_)
  fix: projected 2d point has 1.0 of depth
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* fix(autoware_image_projection_based_fusion): roi cluster fusion has no existence probability update (`#8864 <https://github.com/autowarefoundation/autoware.universe/issues/8864>`_)
  fix: add existence probability update, refactoring
* fix(autoware_image_projection_based_fusion): resolve issue with segmentation pointcloud fusion node failing with multiple mask inputs (`#8769 <https://github.com/autowarefoundation/autoware.universe/issues/8769>`_)
* fix(image_projection_based_fusion): remove unused variable (`#8634 <https://github.com/autowarefoundation/autoware.universe/issues/8634>`_)
  fix: remove unused variable
* fix(autoware_image_projection_based_fusion): fix unusedFunction (`#8567 <https://github.com/autowarefoundation/autoware.universe/issues/8567>`_)
  fix:unusedFunction
* fix(image_projection_based_fusion): add run length decoding for segmentation_pointcloud_fusion (`#7909 <https://github.com/autowarefoundation/autoware.universe/issues/7909>`_)
  * fix: add rle decompress
  * style(pre-commit): autofix
  * fix: use rld in tensorrt utils
  * fix: rebase error
  * fix: dependency
  * fix: skip publish debug mask
  * Update perception/autoware_image_projection_based_fusion/src/segmentation_pointcloud_fusion/node.cpp
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  * style(pre-commit): autofix
  * Revert "fix: skip publish debug mask"
  This reverts commit 30fa9aed866a019705abde71e8f5c3f98960c19e.
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* fix(image_projection_based_fusion): handle projection errors in image fusion nodes (`#7747 <https://github.com/autowarefoundation/autoware.universe/issues/7747>`_)
  * fix: add check for camera distortion model
  * feat(utils): add const qualifier to local variables in checkCameraInfo function
  * style(pre-commit): autofix
  * chore(utils): update checkCameraInfo function to use RCLCPP_ERROR_STREAM for unsupported distortion model and coefficients size
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_image_projection_based_fusion): fix passedByValue (`#8234 <https://github.com/autowarefoundation/autoware.universe/issues/8234>`_)
  fix:passedByValue
* refactor(image_projection_based_fusion)!: add package name prefix of autoware\_ (`#8162 <https://github.com/autowarefoundation/autoware.universe/issues/8162>`_)
  refactor: rename image_projection_based_fusion to autoware_image_projection_based_fusion
* Contributors: Esteve Fernandez, Taekjin LEE, Yi-Hsiang Fang (Vivid), Yoshi Ri, Yutaka Kondo, badai nguyen, kobayu858

0.26.0 (2024-04-03)
-------------------
