^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_test_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* ci(pre-commit): update cpplint to 2.0.0 (`#9557 <https://github.com/autowarefoundation/autoware.universe/issues/9557>`_)
* fix(cpplint): include what you use - common (`#9564 <https://github.com/autowarefoundation/autoware.universe/issues/9564>`_)
* refactor(test_utils): return parser as optional (`#9391 <https://github.com/autowarefoundation/autoware.universe/issues/9391>`_)
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(autoware_test_utils): add parser for LaneletRoute, TrackedObject (`#9317 <https://github.com/autowarefoundation/autoware.universe/issues/9317>`_)
* test(autoware_test_utils): add TrafficLight to map, fix launcher (`#9318 <https://github.com/autowarefoundation/autoware.universe/issues/9318>`_)
* test(bpp_common): add tests for the static drivable area (`#9324 <https://github.com/autowarefoundation/autoware.universe/issues/9324>`_)
* test(costmap_generator): unit test implementation for costmap generator (`#9149 <https://github.com/autowarefoundation/autoware.universe/issues/9149>`_)
  * modify costmap generator directory structure
  * rename class CostmapGenerator to CostmapGeneratorNode
  * unit test for object_map_utils
  * catch error from lookupTransform
  * use polling subscriber in costmap generator node
  * add test for costmap generator node
  * add test for isActive()
  * revert unnecessary changes
  * remove commented out line
  * minor fix
  * Update planning/autoware_costmap_generator/src/costmap_generator.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(autoware_test_utils): use sample_vehicle/sample_sensor_kit (`#9290 <https://github.com/autowarefoundation/autoware.universe/issues/9290>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(autoware_test_utils): add general topic dumper (`#9207 <https://github.com/autowarefoundation/autoware.universe/issues/9207>`_)
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* feat(autoware_test_utils): add traffic light msgs parser (`#9177 <https://github.com/autowarefoundation/autoware.universe/issues/9177>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Mamoru Sobue, Maxime CLEMENT, Ryohsuke Mitsudome, Yutaka Kondo, Zulfaqar Azmi, awf-autoware-bot[bot], mkquda

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* test(costmap_generator): unit test implementation for costmap generator (`#9149 <https://github.com/autowarefoundation/autoware.universe/issues/9149>`_)
  * modify costmap generator directory structure
  * rename class CostmapGenerator to CostmapGeneratorNode
  * unit test for object_map_utils
  * catch error from lookupTransform
  * use polling subscriber in costmap generator node
  * add test for costmap generator node
  * add test for isActive()
  * revert unnecessary changes
  * remove commented out line
  * minor fix
  * Update planning/autoware_costmap_generator/src/costmap_generator.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(autoware_test_utils): use sample_vehicle/sample_sensor_kit (`#9290 <https://github.com/autowarefoundation/autoware.universe/issues/9290>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(autoware_test_utils): add general topic dumper (`#9207 <https://github.com/autowarefoundation/autoware.universe/issues/9207>`_)
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* feat(autoware_test_utils): add traffic light msgs parser (`#9177 <https://github.com/autowarefoundation/autoware.universe/issues/9177>`_)
* Contributors: Esteve Fernandez, Mamoru Sobue, Yutaka Kondo, mkquda

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(autoware_test_utils): add parser for PredictedObjects (`#9176 <https://github.com/autowarefoundation/autoware.universe/issues/9176>`_)
  feat(autoware_test_utils): add parser for predicted objects
* refactor(autoware_test_utils): sanitizer header (`#9174 <https://github.com/autowarefoundation/autoware.universe/issues/9174>`_)
* refactor(component_interface_specs): prefix package and namespace with autoware (`#9094 <https://github.com/autowarefoundation/autoware.universe/issues/9094>`_)
* fix(autoware_test_utils): remove unnecessary cppcheck suppression (`#9165 <https://github.com/autowarefoundation/autoware.universe/issues/9165>`_)
* feat(autoware_test_utils): add parser for geometry_msgs and others (`#9167 <https://github.com/autowarefoundation/autoware.universe/issues/9167>`_)
* feat(autoware_test_utils): add path with lane id parser (`#9098 <https://github.com/autowarefoundation/autoware.universe/issues/9098>`_)
  * add path with lane id parser
  * refactor parse to use template
  ---------
* feat(autoware_test_utils): move test_map, add launcher for test_map (`#9045 <https://github.com/autowarefoundation/autoware.universe/issues/9045>`_)
* feat(test_utils): add simple path with lane id generator (`#9113 <https://github.com/autowarefoundation/autoware.universe/issues/9113>`_)
  * add simple path with lane id generator
  * chnage to explicit template
  * fix
  * add static cast
  * remove header file
  ---------
* fix(autoware_test_utils): missing ament_index_cpp dependency (`#8618 <https://github.com/autowarefoundation/autoware.universe/issues/8618>`_)
* fix(autoware_test_utils): fix unusedFunction (`#8857 <https://github.com/autowarefoundation/autoware.universe/issues/8857>`_)
  fix:unusedFunction
* fix(autoware_test_utils): fix unusedFunction (`#8816 <https://github.com/autowarefoundation/autoware.universe/issues/8816>`_)
  fix: unusedFunction
* test(autoware_route_handler): add unit test for autoware route handler function (`#8271 <https://github.com/autowarefoundation/autoware.universe/issues/8271>`_)
  * remove unused functions in route handler
  * add docstring for function getShoulderLaneletsAtPose
  * update test map to include shoulder lanelet
  * add unit test for function getShoulderLaneletsAtPose
  * add test case for getCenterLinePath to improve branch coverage
  ---------
* feat(autoware_test_utils): add qos handler in pub/sub (`#7856 <https://github.com/autowarefoundation/autoware.universe/issues/7856>`_)
  * feat: add qos handler in pub/sub
  * style(pre-commit): autofix
  * feat: update test_pub_msg function to not use setpublisher function
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat(auoware_test_utils): add jump_clock interface (`#7638 <https://github.com/autowarefoundation/autoware.universe/issues/7638>`_)
  * feat(auoware_test_utils): add jump_clock interface
  * add comment
  ---------
* feat(route_handler): add unit test for lane change related functions (`#7504 <https://github.com/autowarefoundation/autoware.universe/issues/7504>`_)
  * RT1-6230 feat(route_handler): add unit test for lane change related functions
  * fix spell check
  * fix spellcheck
  ---------
* feat(autoware_test_utils): add autoware test manager (`#7597 <https://github.com/autowarefoundation/autoware.universe/issues/7597>`_)
  * feat(detected_object_validation): add test
  * move to autoware_test_utils
  * remove perception
  * update cmake
  * style(pre-commit): autofix
  * remove perception change
  * add include
  * refactored
  * avoid using void and static_pointer_cast
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(autoware_test_utils): function to load paths from folder (`#7474 <https://github.com/autowarefoundation/autoware.universe/issues/7474>`_)
* fix(route_handler): route handler overlap removal is too conservative (`#7156 <https://github.com/autowarefoundation/autoware.universe/issues/7156>`_)
  * add flag to enable/disable loop check in getLaneletSequence functions
  * implement function to get closest route lanelet based on previous closest lanelet
  * refactor DefaultPlanner::plan function
  * modify loop check logic in getLaneletSequenceUpTo function
  * improve logic in isEgoOutOfRoute function
  * fix format
  * check if prev lanelet is a goal lanelet in getLaneletSequenceUpTo function
  * separate function to update current route lanelet in planner manager
  * rename function and add docstring
  * modify functions extendNextLane and extendPrevLane to account for overlap
  * refactor function getClosestRouteLaneletFromLanelet
  * add route handler unit tests for overlapping route case
  * fix function getClosestRouteLaneletFromLanelet
  * format fix
  * move test map to autoware_test_utils
  ---------
* refactor(test_utils): move to common folder (`#7158 <https://github.com/autowarefoundation/autoware.universe/issues/7158>`_)
  * Move autoware planning test manager to autoware namespace
  * fix package share directory for behavior path planner
  * renaming files and directory
  * rename variables that has planning_test_utils in its name.
  * use autoware namespace for test utils
  * move folder to common
  * update .pages file
  * fix test error
  * removed obstacle velocity limiter test artifact
  * remove namespace from planning validator, it has using keyword
  ---------
* Contributors: Esteve Fernandez, Go Sakayori, Kosuke Takeuchi, Mamoru Sobue, Nagi70, Ryuta Kambe, Takayuki Murooka, Tim Clephas, Yoshi Ri, Yutaka Kondo, Zulfaqar Azmi, kminoda, kobayu858, mkquda

0.26.0 (2024-04-03)
-------------------
