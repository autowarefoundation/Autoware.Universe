^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_path_smoother
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(osqp_interface): added autoware prefix to osqp_interface (`#8958 <https://github.com/autowarefoundation/autoware.universe/issues/8958>`_)
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(motion_planning): align the parameters with launcher (`#8792 <https://github.com/autowarefoundation/autoware.universe/issues/8792>`_)
  parameters in motion_planning aligned
* fix(autoware_path_smoother): fix passedByValue (`#8203 <https://github.com/autowarefoundation/autoware.universe/issues/8203>`_)
  fix:passedByValue
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(path_smoother): rename to include/autoware/{package_name} (`#7527 <https://github.com/autowarefoundation/autoware.universe/issues/7527>`_)
  * feat(path_smoother): rename to include/autoware/{package_name}
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
* refactor(path_smoother)!: prefix package and namespace with autoware (`#7381 <https://github.com/autowarefoundation/autoware.universe/issues/7381>`_)
  * git mv
  * fix
  * fix launch
  * rever a part of prefix
  * fix test
  * fix
  * fix static_centerline_optimizer
  * fix
  ---------
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Takayuki Murooka, Yutaka Kondo, Zhe Shen, Zulfaqar Azmi, kobayu858

0.26.0 (2024-04-03)
-------------------
