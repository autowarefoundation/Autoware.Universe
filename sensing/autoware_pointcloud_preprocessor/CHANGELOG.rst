^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_pointcloud_preprocessor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - sensing (`#9571 <https://github.com/autowarefoundation/autoware.universe/issues/9571>`_)
* fix(autoware_pointcloud_preprocessor): remove unused arg and unavailable param file. (`#9525 <https://github.com/autowarefoundation/autoware.universe/issues/9525>`_)
  Remove unused arg and unavailable param file.
* fix(autoware_pointcloud_preprocessor): fix clang-diagnostic-inconsistent-missing-override (`#9445 <https://github.com/autowarefoundation/autoware.universe/issues/9445>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore: update license of pointcloud preprocessor (`#9397 <https://github.com/autowarefoundation/autoware.universe/issues/9397>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_pointcloud_preprocessor): clang-tidy error in distortion corrector (`#9412 <https://github.com/autowarefoundation/autoware.universe/issues/9412>`_)
  fix: clang-tidy
* fix(autoware_pointcloud_preprocessor): clang-tidy for overrides (`#9414 <https://github.com/autowarefoundation/autoware.universe/issues/9414>`_)
  fix: clang-tidy for overrides
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(autoware_pointcloud_preprocessor): fix the wrong naming of crop box parameter file  (`#9258 <https://github.com/autowarefoundation/autoware.universe/issues/9258>`_)
  fix: fix the wrong file name
* fix(autoware_pointcloud_preprocessor): launch file load parameter from yaml (`#8129 <https://github.com/autowarefoundation/autoware.universe/issues/8129>`_)
  * feat: fix launch file
  * chore: fix spell error
  * chore: fix parameters file name
  * chore: remove filter base
  ---------
* Contributors: Daisuke Nishimatsu, Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Mukunda Bharatheesha, Ryohsuke Mitsudome, Ryuta Kambe, Yi-Hsiang Fang (Vivid), Yutaka Kondo

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
* fix(autoware_pointcloud_preprocessor): fix the wrong naming of crop box parameter file  (`#9258 <https://github.com/autowarefoundation/autoware.universe/issues/9258>`_)
  fix: fix the wrong file name
* fix(autoware_pointcloud_preprocessor): launch file load parameter from yaml (`#8129 <https://github.com/autowarefoundation/autoware.universe/issues/8129>`_)
  * feat: fix launch file
  * chore: fix spell error
  * chore: fix parameters file name
  * chore: remove filter base
  ---------
* Contributors: Esteve Fernandez, Yi-Hsiang Fang (Vivid), Yutaka Kondo

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
* feat(autoware_pointcloud_preprocessor): distortion corrector node update azimuth and distance (`#8380 <https://github.com/autowarefoundation/autoware.universe/issues/8380>`_)
  * feat: add option for updating distance and azimuth value
  * chore: clean code
  * chore: remove space
  * chore: add documentation
  * chore: fix docs
  * feat: conversion formula implementation for degree, still need to change to rad
  * chore: fix tests for AzimuthConversionExists function
  * feat: add fastatan to utils
  * feat: remove seperate sin, cos and use sin_and_cos function
  * chore: fix readme
  * chore: fix some grammar errors
  * chore: fix spell error
  * chore: set debug mode to false
  * chore: set update_azimuth_and_distance default value to false
  * chore: update readme
  * chore: remove cout
  * chore: add opencv license
  * chore: fix grammar error
  * style(pre-commit): autofix
  * chore: add runtime error when azimuth conversion failed
  * chore: change default pointcloud
  * chore: change function name
  * chore: move variables to structure
  * chore: add random seed
  * chore: rewrite get conversion function
  * chore: fix opencv fast atan2 function
  * chore: fix schema description
  * Update sensing/autoware_pointcloud_preprocessor/test/test_distortion_corrector_node.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * Update sensing/autoware_pointcloud_preprocessor/test/test_distortion_corrector_node.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * chore: move code to function for readability
  * chore: simplify code
  * chore: fix sentence, angle conversion
  * chore: add more invalid condition
  * chore: fix the string name to enum
  * chore: remove runtime error
  * chore: use optional for AngleConversion structure
  * chore: fix bug and clean code
  * chore: refactor the logic of calculating conversion
  * chore: refactor function in unit test
  * chore: RCLCPP_WARN_STREAM logging when failed to get angle conversion
  * chore: improve normalize angle algorithm
  * chore: improve multiple_of_90_degrees logic
  * chore: add opencv license
  * style(pre-commit): autofix
  * chore: clean code
  * chore: fix sentence
  * style(pre-commit): autofix
  * chore: add 0 0 0 points in test case
  * chore: fix spell error
  * Update common/autoware_universe_utils/NOTICE
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * Update sensing/autoware_pointcloud_preprocessor/src/distortion_corrector/distortion_corrector_node.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * Update sensing/autoware_pointcloud_preprocessor/src/distortion_corrector/distortion_corrector.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * chore: use constexpr for threshold
  * chore: fix the path of license
  * chore: explanation for failures
  * chore: use throttle
  * chore: fix empty pointcloud function
  * refactor: change camel to snake case
  * Update sensing/autoware_pointcloud_preprocessor/include/autoware/pointcloud_preprocessor/distortion_corrector/distortion_corrector_node.hpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * Update sensing/autoware_pointcloud_preprocessor/include/autoware/pointcloud_preprocessor/distortion_corrector/distortion_corrector_node.hpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * style(pre-commit): autofix
  * Update sensing/autoware_pointcloud_preprocessor/test/test_distortion_corrector_node.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * refactor: refactor virtual function in base class
  * chore: fix test naming error
  * chore: fix clang error
  * chore: fix error
  * chore: fix clangd
  * chore: add runtime error if the setting is wrong
  * chore: clean code
  * Update sensing/autoware_pointcloud_preprocessor/src/distortion_corrector/distortion_corrector.cpp
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  * style(pre-commit): autofix
  * chore: fix unit test for runtime error
  * Update sensing/autoware_pointcloud_preprocessor/docs/distortion-corrector.md
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  * chore: fix offset_rad_threshold
  * chore: change pointer to reference
  * chore: snake_case for unit test
  * chore: fix refactor process twist and imu
  * chore: fix abs and return type of matrix to tf2
  * chore: fix grammar error
  * chore: fix readme description
  * chore: remove runtime error
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Max Schmeller <6088931+mojomex@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(autoware_pointcloud_preprocessor): rework crop box parameters (`#8466 <https://github.com/autowarefoundation/autoware.universe/issues/8466>`_)
  * feat: add parameter schema for crop box
  * chore: fix readme
  * chore: remove filter.param.yaml file
  * chore: add negative parameter for voxel grid based euclidean cluster
  * chore: fix schema description
  * chore: fix description of negative param
  ---------
* refactor(autoware_pointcloud_preprocessor): rework approximate downsample filter parameters (`#8480 <https://github.com/autowarefoundation/autoware.universe/issues/8480>`_)
  * feat: rework approximate downsample parameters
  * chore: add boundary
  * chore: change double to float
  * feat: rework approximate downsample parameters
  * chore: add boundary
  * chore: change double to float
  * chore: fix grammatical error
  * chore: fix variables from double to float in header
  * chore: change minimum to float
  * chore: fix CMakeLists
  ---------
* refactor(autoware_pointcloud_preprocessor): rework dual return outlier filter parameters (`#8475 <https://github.com/autowarefoundation/autoware.universe/issues/8475>`_)
  * feat: rework dual return outlier filter parameters
  * chore: fix readme
  * chore: change launch file name
  * chore: fix type
  * chore: add boundary
  * chore: change boundary
  * chore: fix boundary
  * chore: fix json schema
  * Update sensing/autoware_pointcloud_preprocessor/schema/dual_return_outlier_filter_node.schema.json
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  * chore: fix grammar error
  * chore: fix description for weak_first_local_noise_threshold
  * chore: change minimum and maximum to float
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(autoware_pointcloud_preprocessor): rework ring outlier filter parameters (`#8468 <https://github.com/autowarefoundation/autoware.universe/issues/8468>`_)
  * feat: rework ring outlier parameters
  * chore: add explicit cast
  * chore: add boundary
  * chore: remove filter.param
  * chore: set default frame
  * chore: add maximum boundary
  * chore: boundary to float type
  ---------
* refactor(autoware_pointcloud_preprocessor): rework pickup based voxel grid downsample filter parameters (`#8481 <https://github.com/autowarefoundation/autoware.universe/issues/8481>`_)
  * feat: rework pickup based voxel grid downsample filter parameter
  * chore: update date
  * chore: fix spell error
  * chore: add boundary
  * chore: fix grammatical error
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* ci(pre-commit): autoupdate (`#7630 <https://github.com/autowarefoundation/autoware.universe/issues/7630>`_)
  * ci(pre-commit): autoupdate
  * style(pre-commit): autofix
  * fix: remove the outer call to dict()
  ---------
  Co-authored-by: github-actions <github-actions@github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
* refactor(autoware_pointcloud_preprocessor): rework random downsample filter parameters (`#8485 <https://github.com/autowarefoundation/autoware.universe/issues/8485>`_)
  * feat: rework random downsample filter parameter
  * chore: change name
  * chore: add explicit cast
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(autoware_pointcloud_preprocessor): rework pointcloud accumulator parameters  (`#8487 <https://github.com/autowarefoundation/autoware.universe/issues/8487>`_)
  * feat: rework pointcloud accumulator parameters
  * chore: add explicit cast
  * chore: add boundary
  ---------
* refactor(autoware_pointcloud_preprocessor): rework radius search 2d outlier filter parameters (`#8474 <https://github.com/autowarefoundation/autoware.universe/issues/8474>`_)
  * feat: rework radius search 2d outlier filter parameters
  * chore: fix schema
  * chore: explicit cast
  * chore: add boundary in schema
  ---------
* refactor(autoware_pointcloud_preprocessor): rework ring passthrough filter parameters (`#8472 <https://github.com/autowarefoundation/autoware.universe/issues/8472>`_)
  * feat: rework ring passthrough parameters
  * chore: fix cmake
  * feat: add schema
  * chore: fix readme
  * chore: fix parameter file name
  * chore: add boundary
  * chore: fix default parameter
  * chore: fix default parameter in schema
  ---------
* fix(autoware_pointcloud_preprocessor): static TF listener as Filter option (`#8678 <https://github.com/autowarefoundation/autoware.universe/issues/8678>`_)
* fix(pointcloud_preprocessor): fix typo (`#8762 <https://github.com/autowarefoundation/autoware.universe/issues/8762>`_)
* fix(autoware_pointcloud_preprocessor): instantiate templates so that the symbols exist when linking (`#8743 <https://github.com/autowarefoundation/autoware.universe/issues/8743>`_)
* fix(autoware_pointcloud_preprocessor): fix unusedFunction (`#8673 <https://github.com/autowarefoundation/autoware.universe/issues/8673>`_)
  fix:unusedFunction
* fix(autoware_pointcloud_preprocessor): resolve issue with FLT_MAX not declared on Jazzy (`#8586 <https://github.com/autowarefoundation/autoware.universe/issues/8586>`_)
  fix(pointcloud-preprocessor): FLT_MAX not declared
  Fixes compilation error on Jazzy:
  error: ‘FLT_MAX’ was not declared in this scope
* fix(autoware_pointcloud_preprocessor): blockage diag node add runtime error when the parameter is wrong (`#8564 <https://github.com/autowarefoundation/autoware.universe/issues/8564>`_)
  * fix: add runtime error
  * Update blockage_diag_node.cpp
  Co-authored-by: badai nguyen  <94814556+badai-nguyen@users.noreply.github.com>
  * fix: add RCLCPP error logging
  * chore: remove unused variable
  ---------
  Co-authored-by: badai nguyen <94814556+badai-nguyen@users.noreply.github.com>
* chore(autoware_pointcloud_preprocessor): change unnecessary warning message to debug (`#8525 <https://github.com/autowarefoundation/autoware.universe/issues/8525>`_)
* refactor(autoware_pointcloud_preprocessor): rework voxel grid outlier filter  parameters (`#8476 <https://github.com/autowarefoundation/autoware.universe/issues/8476>`_)
  * feat: rework voxel grid outlier filter parameters
  * chore: add boundary
  ---------
* refactor(autoware_pointcloud_preprocessor): rework lanelet2 map filter parameters (`#8491 <https://github.com/autowarefoundation/autoware.universe/issues/8491>`_)
  * feat: rework lanelet2 map filter parameters
  * chore: remove unrelated files
  * fix: fix node name in launch
  * chore: fix launcher
  * chore: fix spell error
  * chore: add boundary
  ---------
* refactor(autoware_pointcloud_preprocessor): rework vector map inside area filter parameters  (`#8493 <https://github.com/autowarefoundation/autoware.universe/issues/8493>`_)
  * feat: rework vector map inside area filter parameter
  * chore: fix launcher
  * chore: fix launcher input and output
  ---------
* refactor(autoware_pointcloud_preprocessor): rework concatenate_pointcloud and time_synchronizer_node parameters (`#8509 <https://github.com/autowarefoundation/autoware.universe/issues/8509>`_)
  * feat: rewort concatenate pointclouds and time synchronizer parameter
  * chore: fix launch files
  * chore: fix schema
  * chore: fix schema
  * chore: fix integer and number default value in schema
  * chore: add boundary
  ---------
* refactor(autoware_pointcloud_preprocessor): rework voxel grid downsample filter parameters (`#8486 <https://github.com/autowarefoundation/autoware.universe/issues/8486>`_)
  * feat:rework voxel grid downsample parameters
  * chore: add boundary
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(autoware_pointcloud_preprocessor): rework blockage diag parameters  (`#8488 <https://github.com/autowarefoundation/autoware.universe/issues/8488>`_)
  * feat: rework blockage diag parameters
  * chore: fix readme
  * chore: fix schema description
  * chore: add boundary for schema
  ---------
* chore(autoware_pcl_extensions): refactored the pcl_extensions (`#8220 <https://github.com/autowarefoundation/autoware.universe/issues/8220>`_)
  chore: refactored the pcl_extensions according to the new rules
* feat(pointcloud_preprocessor)!: revert "fix: added temporary retrocompatibility to old perception data (`#7929 <https://github.com/autowarefoundation/autoware.universe/issues/7929>`_)" (`#8397 <https://github.com/autowarefoundation/autoware.universe/issues/8397>`_)
  * feat!(pointcloud_preprocessor): Revert "fix: added temporary retrocompatibility to old perception data (`#7929 <https://github.com/autowarefoundation/autoware.universe/issues/7929>`_)"
  This reverts commit 6b9f164b123e2f6a6fedf7330e507d4b68e45a09.
  * feat(pointcloud_preprocessor): minor grammar fix
  Co-authored-by: David Wong <33114676+drwnz@users.noreply.github.com>
  ---------
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
  Co-authored-by: David Wong <33114676+drwnz@users.noreply.github.com>
* fix(autoware_pointcloud_preprocessor): fix variableScope (`#8447 <https://github.com/autowarefoundation/autoware.universe/issues/8447>`_)
  * fix:variableScope
  * refactor:use const
  ---------
* fix(autoware_pointcloud_preprocessor): fix unreadVariable (`#8370 <https://github.com/autowarefoundation/autoware.universe/issues/8370>`_)
  fix:unreadVariable
* fix(ring_outlier_filter): remove unnecessary resize to prevent zero points (`#8402 <https://github.com/autowarefoundation/autoware.universe/issues/8402>`_)
  fix: remove unnecessary resize
* fix(autoware_pointcloud_preprocessor): fix cppcheck warnings of functionStatic (`#8163 <https://github.com/autowarefoundation/autoware.universe/issues/8163>`_)
  fix: deal with functionStatic warnings
  Co-authored-by: Yi-Hsiang Fang (Vivid) <146902905+vividf@users.noreply.github.com>
* perf(autoware_pointcloud_preprocessor): lazy & managed TF listeners (`#8174 <https://github.com/autowarefoundation/autoware.universe/issues/8174>`_)
  * perf(autoware_pointcloud_preprocessor): lazy & managed TF listeners
  * fix(autoware_pointcloud_preprocessor): param names & reverse frames transform logic
  * fix(autoware_ground_segmentation): add missing TF listener
  * feat(autoware_ground_segmentation): change to static TF buffer
  * refactor(autoware_pointcloud_preprocessor): move StaticTransformListener to universe utils
  * perf(autoware_universe_utils): skip redundant transform
  * fix(autoware_universe_utils): change checks order
  * doc(autoware_universe_utils): add docstring
  ---------
* fix(autoware_pointcloud_preprocessor): fix functionConst (`#8280 <https://github.com/autowarefoundation/autoware.universe/issues/8280>`_)
  fix:functionConst
* fix(autoware_pointcloud_preprocessor): fix passedByValue (`#8242 <https://github.com/autowarefoundation/autoware.universe/issues/8242>`_)
  fix:passedByValue
* fix(autoware_pointcloud_preprocessor): fix redundantInitialization (`#8229 <https://github.com/autowarefoundation/autoware.universe/issues/8229>`_)
* fix(autoware_pointcloud_preprocessor): revert increase_size() in robin_hood (`#8151 <https://github.com/autowarefoundation/autoware.universe/issues/8151>`_)
* fix(autoware_pointcloud_preprocessor): fix knownConditionTrueFalse warning (`#8139 <https://github.com/autowarefoundation/autoware.universe/issues/8139>`_)
* refactor(pointcloud_preprocessor): prefix package and namespace with autoware (`#7983 <https://github.com/autowarefoundation/autoware.universe/issues/7983>`_)
  * refactor(pointcloud_preprocessor)!: prefix package and namespace with autoware
  * style(pre-commit): autofix
  * style(pointcloud_preprocessor): suppress line length check for macros
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * refactor(pointcloud_preprocessor): directory structure (soft)
  * refactor(pointcloud_preprocessor): directory structure (hard)
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Fumiya Watanabe, Kenzo Lobos Tsunekawa, Rein Appeldoorn, Ryuta Kambe, Shintaro Tomie, Yi-Hsiang Fang (Vivid), Yoshi Ri, Yukinari Hisaki, Yutaka Kondo, awf-autoware-bot[bot], kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
