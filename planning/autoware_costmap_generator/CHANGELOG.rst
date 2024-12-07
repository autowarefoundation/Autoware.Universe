^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_costmap_generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
