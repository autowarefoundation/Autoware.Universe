^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_planning_evaluator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_planning_evaluator): devops node dojo (`#8746 <https://github.com/autowarefoundation/autoware.universe/issues/8746>`_)
* feat(motion_velocity_planner,planning_evaluator): add  stop, slow_down diags (`#8503 <https://github.com/autowarefoundation/autoware.universe/issues/8503>`_)
  * tmp save.
  * publish diagnostics.
  * move clearDiagnostics func to head
  * change to snake_names.
  * remove a change of launch.xml
  * pre-commit run -a
  * publish diagnostics on node side.
  * move empty checking out of 'get_diagnostics'.
  * remove get_diagnostics; change reason str.
  * remove unused condition.
  * Update planning/motion_velocity_planner/autoware_motion_velocity_planner_node/src/planner_manager.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update planning/motion_velocity_planner/autoware_motion_velocity_planner_node/src/planner_manager.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
* fix(autoware_planning_evaluator): fix unreadVariable (`#8352 <https://github.com/autowarefoundation/autoware.universe/issues/8352>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* feat(evalautor): rename evaluator diag topics (`#8152 <https://github.com/autowarefoundation/autoware.universe/issues/8152>`_)
  * feat(evalautor): rename evaluator diag topics
  * perception
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
* feat(cruise_planner,planning_evaluator): add cruise and slow down diags (`#7960 <https://github.com/autowarefoundation/autoware.universe/issues/7960>`_)
  * add cruise and slow down diags to cruise planner
  * add cruise types
  * adjust planning eval
  ---------
* feat(planning_evaluator,control_evaluator, evaluator utils): add diagnostics subscriber to planning eval (`#7849 <https://github.com/autowarefoundation/autoware.universe/issues/7849>`_)
  * add utils and diagnostics subscription to planning_evaluator
  * add diagnostics eval
  * fix input diag in launch
  ---------
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(planning_evaluator): add planning evaluator polling sub (`#7827 <https://github.com/autowarefoundation/autoware.universe/issues/7827>`_)
  * WIP add polling subs
  * WIP
  * update functions
  * remove semicolon
  * use last data for modified goal
  ---------
* feat(planning_evaluator): add lanelet info to the planning evaluator (`#7781 <https://github.com/autowarefoundation/autoware.universe/issues/7781>`_)
  add lanelet info to the planning evaluator
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(planning_evaluator): rename to include/autoware/{package_name} (`#7518 <https://github.com/autowarefoundation/autoware.universe/issues/7518>`_)
  * fix
  * fix
  ---------
* Contributors: Kosuke Takeuchi, Takayuki Murooka, Tiankui Xian, Yukinari Hisaki, Yutaka Kondo, danielsanchezaran, kobayu858, odra

0.26.0 (2024-04-03)
-------------------
