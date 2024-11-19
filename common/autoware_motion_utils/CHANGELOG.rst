^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(autoware_motion_utils): add spherical linear interpolator (`#9175 <https://github.com/autowarefoundation/autoware.universe/issues/9175>`_)
* test(motion_utils): add test for path shift (`#9083 <https://github.com/autowarefoundation/autoware.universe/issues/9083>`_)
  * remove unused function
  * mover path shifter utils function to autoware motion utils
  * minor change in license header
  * fix warning message
  * remove header file
  * add test file
  * add unit test to all function
  * fix spelling
  ---------
* refactor(bpp_common, motion_utils): move path shifter util functions to autoware::motion_utils (`#9081 <https://github.com/autowarefoundation/autoware.universe/issues/9081>`_)
  * remove unused function
  * mover path shifter utils function to autoware motion utils
  * minor change in license header
  * fix warning message
  * remove header file
  ---------
* refactor(autoware_motion_utils): refactor interpolator (`#8931 <https://github.com/autowarefoundation/autoware.universe/issues/8931>`_)
  * refactor interpolator
  * update cmake
  * update
  * rename
  * Update CMakeLists.txt
  ---------
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(autoware_motion_utils): set zero velocity after stop in resample trajectory (`#8768 <https://github.com/autowarefoundation/autoware.universe/issues/8768>`_)
  * feat(autoware_motion_utils): set zero velocity after stop in resample trajectory
  * fix unit test
  * simplify implementation
  * update comment and add test
  ---------
* fix(autoware_motion_utils): fix unusedFunction (`#8733 <https://github.com/autowarefoundation/autoware.universe/issues/8733>`_)
  refactor:remove Path/Trajectory length calculation between designated points
* feat(autoware_motion_utils): add clone function and make the constructor public (`#8688 <https://github.com/autowarefoundation/autoware.universe/issues/8688>`_)
  * feat(autoware_motion_utils): add interpolator
  * use int32_t instead of int
  * use int32_t instead of int
  * use int32_t instead of int
  * add const as much as possible and use `at()` in `vector`
  * fix directory name
  * refactor code and add example
  * update
  * remove unused include
  * refactor code
  * add clone function
  * fix stairstep
  * make constructor to public
  ---------
* feat(out_of_lane): redesign to improve accuracy and performance (`#8453 <https://github.com/autowarefoundation/autoware.universe/issues/8453>`_)
* feat(autoware_motion_utils): add interpolator (`#8517 <https://github.com/autowarefoundation/autoware.universe/issues/8517>`_)
  * feat(autoware_motion_utils): add interpolator
  * use int32_t instead of int
  * use int32_t instead of int
  * use int32_t instead of int
  * add const as much as possible and use `at()` in `vector`
  * fix directory name
  * refactor code and add example
  * update
  * remove unused include
  * refactor code
  ---------
* fix(autoware_motion_utils): fix unusedFunction (`#8519 <https://github.com/autowarefoundation/autoware.universe/issues/8519>`_)
  fix: unusedFunction
* fix(start/goal_planner): fix addition of duplicate segments in calcBeforeShiftedArcLength (`#7902 <https://github.com/autowarefoundation/autoware.universe/issues/7902>`_)
  * fix(start/goal_planner): fix addition of duplicate segments in calcBeforeShiftedArcLength
  * Update trajectory.hpp
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
  * Update trajectory.hpp
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
  ---------
  Co-authored-by: Kyoichi Sugahara <kyoichi.sugahara@tier4.jp>
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* Contributors: Esteve Fernandez, Go Sakayori, Kosuke Takeuchi, Maxime CLEMENT, Nagi70, Yukinari Hisaki, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
