^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_control_validator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(control_validator): add hold and lpf (`#9120 <https://github.com/autowarefoundation/autoware.universe/issues/9120>`_)
* feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): add processing time pub. (`#9065 <https://github.com/autowarefoundation/autoware.universe/issues/9065>`_)
  * feat(costmap_generator, control_validator, scenario_selector, surround_obstacle_checker, vehicle_cmd_gate): Add: processing_time_pub
  * fix: pre-commit
  * feat(costmap_generator): fix: No output when not Active.
  * fix: clang-format
  * Re: fix: clang-format
  ---------
* fix(control): align the parameters with launcher (`#8789 <https://github.com/autowarefoundation/autoware.universe/issues/8789>`_)
  align the control parameters
* feat(autoware_control_validator): refactoring & testing (`#8096 <https://github.com/autowarefoundation/autoware.universe/issues/8096>`_)
  * refactoring
  * updating...
  * update
  * fix
  * fix
  * Update CMakeLists.txt
  * use yaml to load vehicle info
  ---------
* fix(control_validator): fix param names and doc (`#8104 <https://github.com/autowarefoundation/autoware.universe/issues/8104>`_)
  * fix
* feat(control_validator)!: add velocity check (`#7806 <https://github.com/autowarefoundation/autoware.universe/issues/7806>`_)
  * add velocity check
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control)!: refactor directory structures of the control checkers (`#7524 <https://github.com/autowarefoundation/autoware.universe/issues/7524>`_)
  * aeb
  * control_validator
  * lane_departure_checker
  * shift_decider
  * fix
  ---------
* feat(autoware_control_validator): add polling subcribers (`#7426 <https://github.com/autowarefoundation/autoware.universe/issues/7426>`_)
  * add polling subs
  * delete extra line
  ---------
* fix(autoware_control_validator): fix vehicle info utils (`#7417 <https://github.com/autowarefoundation/autoware.universe/issues/7417>`_)
* refactor(control_validator)!: prefix package and namespace with autoware (`#7304 <https://github.com/autowarefoundation/autoware.universe/issues/7304>`_)
  * rename folders
  * rename add prefix
  * change param path
  * fix pluggin problem
  * fix extra prefixes
  * change back launchers
  * add namespace to address conflict
  * delete stubborn file
  ---------
* Contributors: Fumiya Watanabe, Kazunori-Nakajima, Kosuke Takeuchi, Takayuki Murooka, Yuki TAKAGI, Yukinari Hisaki, Yutaka Kondo, Zhe Shen, danielsanchezaran

0.26.0 (2024-04-03)
-------------------
