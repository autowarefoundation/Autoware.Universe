^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_costmap_generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* feat(costmap_generator, scenario_selector): improve freespace planning stability (`#9579 <https://github.com/autowarefoundation/autoware.universe/issues/9579>`_)
  * discretize updating grid center position by size of grid resolution
  * modify logic for switching to lane driving in scenario selector
  * fix spelling
  ---------
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* refactor: correct spelling (`#9528 <https://github.com/autowarefoundation/autoware.universe/issues/9528>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_costmap_generator): fix clang-diagnostic-unused-private-field (`#9395 <https://github.com/autowarefoundation/autoware.universe/issues/9395>`_)
  fix: clang-diagnostic-unused-private-field
* fix(costmap_generator): use vehicle frame for lidar height thresholds (`#9311 <https://github.com/autowarefoundation/autoware.universe/issues/9311>`_)
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
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Maxime CLEMENT, Ryohsuke Mitsudome, Yutaka Kondo, kobayu858, mkquda

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(costmap_generator): use vehicle frame for lidar height thresholds (`#9311 <https://github.com/autowarefoundation/autoware.universe/issues/9311>`_)
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
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Maxime CLEMENT, Yutaka Kondo, mkquda

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(costmap_generator): fix include for grid_map_utils (`#9179 <https://github.com/autowarefoundation/autoware.universe/issues/9179>`_)
* perf(costmap_generator): manual blurring and fill polygons without OpenCV (`#9160 <https://github.com/autowarefoundation/autoware.universe/issues/9160>`_)
* feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): add processing time pub. (`#9065 <https://github.com/autowarefoundation/autoware.universe/issues/9065>`_)
  * feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): Add: processing_time_pub
  * fix: pre-commit
  * feat(costmap_generator): fix: No output when not Active.
  * fix: clang-format
  * Re: fix: clang-format
  ---------
* perf(costmap_generator): prevent long transform lookup and add timekeeper (`#8886 <https://github.com/autowarefoundation/autoware.universe/issues/8886>`_)
* feat(costmap_generator): integrate generate_parameter_library (`#8827 <https://github.com/autowarefoundation/autoware.universe/issues/8827>`_)
  * add parameter description
  * use parameter listener
  * append global identifier
  * suppress deprecated error
  * fix parameter type
  ---------
* fix(other_planning_packages): align the parameters with launcher (`#8793 <https://github.com/autowarefoundation/autoware.universe/issues/8793>`_)
  * parameters in planning/others aligned
  * update json
  ---------
* fix(autoware_costmap_generator): fix unusedFunction (`#8641 <https://github.com/autowarefoundation/autoware.universe/issues/8641>`_)
  fix:unusedFunction
* perf(costmap_generator, scenario_selector): faster getLinkedParkingLot (`#7930 <https://github.com/autowarefoundation/autoware.universe/issues/7930>`_)
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(costmap_generator)!: add autoware prefix (`#7329 <https://github.com/autowarefoundation/autoware.universe/issues/7329>`_)
  refactor(costmap_generator): add autoware prefix
* Contributors: Kazunori-Nakajima, Kosuke Takeuchi, Maxime CLEMENT, Mitsuhiro Sakamoto, Yutaka Kondo, Zhe Shen, kobayu858

0.26.0 (2024-04-03)
-------------------
