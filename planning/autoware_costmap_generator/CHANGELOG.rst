^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_costmap_generator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
