^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_universe_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - common (`#9564 <https://github.com/autowarefoundation/autoware.universe/issues/9564>`_)
* feat(universe_utils): add extra info to time keeper warning (`#9484 <https://github.com/autowarefoundation/autoware.universe/issues/9484>`_)
  add extra info to time keeper warning
* refactor(evaluators, autoware_universe_utils): rename Stat class to Accumulator and move it to autoware_universe_utils (`#9459 <https://github.com/autowarefoundation/autoware.universe/issues/9459>`_)
  * add Accumulator class to autoware_universe_utils
  * use Accumulator on all evaluators.
  * pre-commit
  * found and fixed a bug. add more tests.
  * pre-commit
  * Update common/autoware_universe_utils/include/autoware/universe_utils/math/accumulator.hpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_utils): address self-intersecting polygons in random_concave_generator and handle empty inners() during triangulation (`#8995 <https://github.com/autowarefoundation/autoware.universe/issues/8995>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Giovanni Muhammad Raditya, Kem (TiankuiXian), M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, danielsanchezaran

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_utils): address self-intersecting polygons in random_concave_generator and handle empty inners() during triangulation (`#8995 <https://github.com/autowarefoundation/autoware.universe/issues/8995>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Giovanni Muhammad Raditya, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
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
* fix(universe_utils): avoid test timeout (`#8993 <https://github.com/autowarefoundation/autoware.universe/issues/8993>`_)
  reduce number of polygons to be generated
* fix(autoware_universe_utils): fix unmatchedSuppression (`#8986 <https://github.com/autowarefoundation/autoware.universe/issues/8986>`_)
  fix:unmatchedSuppression
* refactor(universe_utils): eliminate dependence on Boost.Geometry (`#8965 <https://github.com/autowarefoundation/autoware.universe/issues/8965>`_)
  * add alt::Polygon2d -> Polygon2d conversion function
  * migrate to alt geometry
  * invert orientation of linked list
  * suppress cppcheck unusedFunction error
  * fix parameter to avoid confusion
  ---------
* feat(autoware_universe_utils): reduce dependence on Boost.Geometry (`#8592 <https://github.com/autowarefoundation/autoware.universe/issues/8592>`_)
  * add find_farthest()
  * add simplify()
  * add envelope()
  * (WIP) add buffer()
  * add Polygon2d class
  * change input type of envelope()
  * disable convexity check until correct() supports non-convex polygons
  * add is_clockwise()
  * make correct() support non-convex polygons
  * fix test case
  * Revert "(WIP) add buffer()"
  This reverts commit 123b0ba85ede5e558431a4336038c14023d1bef1.
  ---------
* refactor(universe_utils): remove raw pointers from the triangulation function (`#8893 <https://github.com/autowarefoundation/autoware.universe/issues/8893>`_)
* fix(autoware_pointcloud_preprocessor): static TF listener as Filter option (`#8678 <https://github.com/autowarefoundation/autoware.universe/issues/8678>`_)
* feat(universe_utils): add Triangulation (ear clipping) implementation for 2D concave polygon with/without holes (`#8609 <https://github.com/autowarefoundation/autoware.universe/issues/8609>`_)
  * added random_concave_polygon and triangulation
  * disable some test with GJK
  * pre-commit fix
  * fully fixed convexHull issue and  styling fix
  * fix conflict
  * cleaning up the code
  * cleanup the code
  * cleanup the code
  * fix spelling
  * last cleanup
  * more spellcheck fix
  * more spellcheck fixes
  ---------
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
* refactor(autoware_universe_utils): refactor Boost.Geometry alternatives (`#8594 <https://github.com/autowarefoundation/autoware.universe/issues/8594>`_)
  * move alternatives to separate files
  * style(pre-commit): autofix
  * include missing headers
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_universe_utils): fix unusedFunction (`#8723 <https://github.com/autowarefoundation/autoware.universe/issues/8723>`_)
  fix:unusedFunction
* feat(universe_utils): add SAT implementation for 2D convex polygon collision check (`#8239 <https://github.com/autowarefoundation/autoware.universe/issues/8239>`_)
* feat(autoware_universe_utils): add thread_id check to time_keeper (`#8628 <https://github.com/autowarefoundation/autoware.universe/issues/8628>`_)
  add thread_id check
* fix(autoware_universe_utils): fix unusedFunction (`#8521 <https://github.com/autowarefoundation/autoware.universe/issues/8521>`_)
  fix: unusedFunction
* feat(autoware_universe_utils): add LRU Cache (`#8456 <https://github.com/autowarefoundation/autoware.universe/issues/8456>`_)
* fix(autoware_universe_utils): fix memory leak of time_keeper (`#8425 <https://github.com/autowarefoundation/autoware.universe/issues/8425>`_)
  fix bug of time_keeper
* feat(autoware_universe_utils): reduce dependence on Boost.Geometry (`#7778 <https://github.com/autowarefoundation/autoware.universe/issues/7778>`_)
  * add within function
  * return nullopt as is
  * add disjoint function
  * add polygon-and-polygon version of intersect function
  * use intersect for disjoint
  * add test case for disjoint
  * checking intersection of edges is unnecessary
  * return nullopt when no intersection point found
  * add distance function
  * add coveredBy function
  * add point-polygon variant of distance function
  * add isAbove function
  * add divideBySegment function
  * add convexHull function
  * add correct function
  * add area function
  * change point type to tf2::Vector3
  * simplify correct function
  * push geometry types to namespace
  * match the behavior of Boost.Geometry
  * add test cases for benchmarking
  * add headers for convex_hull()
  * remove polygon-polygon intersect & disjoint function
  * add intersects function
  * add touches function
  * add disjoint function
  * minor fix
  * change name Polygon to CvxPolygon
  * change name CvxPolygon to ConvexPolygon
  * rename intersect function and restore the original
  * change function names to snake_case
  * early return
  * change point type from tf2::Vector3 to custom struct
  * style(pre-commit): autofix
  * use alt::Vector2d to represent point
  * convert from boost before time measurement
  * add header for std::move
  * avoid using long
  * convert from boost before time measurement
  * add point-segment variant of touches function
  * improve performance of point-polygon touches()
  * improve performance of area()
  * add note for class naming
  * improve performance of covered_by()
  * simplify within()
  * improve performance of covered_by()
  * improve performance of within()
  * use operator[] instead of at()
  * print point when covered_by() test failed
  * avoid using hypot()
  * improve performace of convex_hull()
  * remove divide_by_segment() function
  * fix test cases
  * improve performance of touches()
  * add test case for touches()
  * improve performance of touches()
  * change type alias PointList to Points2d
  * add & fix vector size assertions
  * define epsilon respectively
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* fix(autoware_universe_utils): fix constParameterReference (`#8145 <https://github.com/autowarefoundation/autoware.universe/issues/8145>`_)
  * fix:constParameterReference
  * fix:clang format
  * fix:constParameterReference
  * fix:clang format
  ---------
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
* refactor(autoware_universe_utils): changed the API to be more intuitive and added documentation (`#7443 <https://github.com/autowarefoundation/autoware.universe/issues/7443>`_)
  * refactor(tier4_autoware_utils): Changed the API to be more intuitive and added documentation.
  * use raw shared ptr in PollingPolicy::NEWEST
  * update
  * fix
  * Update evaluator/autoware_control_evaluator/include/autoware/control_evaluator/control_evaluator_node.hpp
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
  ---------
  Co-authored-by: danielsanchezaran <daniel.sanchez@tier4.jp>
* feat(universe_utils): add GJK implementation for 2D convex polygon collision check (`#7853 <https://github.com/autowarefoundation/autoware.universe/issues/7853>`_)
* feat(autoware_universe_utils): add comment function to time_keeper (`#7991 <https://github.com/autowarefoundation/autoware.universe/issues/7991>`_)
  * update readme
  * refactoring
  * remove string reporter
  * fix readme.md
  * feat(autoware_universe_utils): add comment function to time_keeper
  * remove comment from scoped time track
  * modify readme
  ---------
* chore(autoware_universe_utils): update document (`#7907 <https://github.com/autowarefoundation/autoware.universe/issues/7907>`_)
  * update readme
  * refactoring
  * remove string reporter
  * fix readme.md
  * change node name of example
  * update readme
  ---------
* fix(autoware_universe_utils): fix constParameterReference (`#7882 <https://github.com/autowarefoundation/autoware.universe/issues/7882>`_)
  * fix: constParameterReference
  * fix: constParameterReference
  ---------
* feat(autoware_universe_utils): add TimeKeeper to track function's processing time (`#7754 <https://github.com/autowarefoundation/autoware.universe/issues/7754>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* Contributors: Amadeusz Szymko, Giovanni Muhammad Raditya, Kosuke Takeuchi, Maxime CLEMENT, Mitsuhiro Sakamoto, Nagi70, Takayuki Murooka, Yi-Hsiang Fang (Vivid), Yukinari Hisaki, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
