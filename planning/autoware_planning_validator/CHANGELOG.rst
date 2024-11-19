^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_planning_validator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(autoware_planning_validator): fix unusedFunction (`#8646 <https://github.com/autowarefoundation/autoware.universe/issues/8646>`_)
  fix:unusedFunction
* fix(autoware_planning_validator): fix knownConditionTrueFalse (`#7817 <https://github.com/autowarefoundation/autoware.universe/issues/7817>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(planning_validator): rename to include/autoware/{package_name} (`#7514 <https://github.com/autowarefoundation/autoware.universe/issues/7514>`_)
  * feat(planning_validator): rename to include/autoware/{package_name}
  * fix
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
* refactor(planning_validator)!: rename directory name  (`#7411 <https://github.com/autowarefoundation/autoware.universe/issues/7411>`_)
  change directory name
* Contributors: Kosuke Takeuchi, Kyoichi Sugahara, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, Zulfaqar Azmi, kobayu858

0.26.0 (2024-04-03)
-------------------
