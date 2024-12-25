^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_motion_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(factor): move steering factor interface to motion utils (`#9337 <https://github.com/autowarefoundation/autoware.universe/issues/9337>`_)
* feat(behavior_velocity_stop_line): replace autoware_motion_utils to autoware_trajectory (`#9299 <https://github.com/autowarefoundation/autoware.universe/issues/9299>`_)
  * feat(behavior_velocity_stop_line): replace autoware_motion_utils to autoware_trajectory
  * update
  ---------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(autoware_trajectory): move trajectory_container from autoware_motion_utils to a new package (`#9253 <https://github.com/autowarefoundation/autoware.universe/issues/9253>`_)
  * create trajectory container package
  * update
  * update
  * style(pre-commit): autofix
  * update codeowner
  * update
  * fix cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_motion_utils): add new trajectory class (`#8693 <https://github.com/autowarefoundation/autoware.universe/issues/8693>`_)
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
  * feat(autoware_motion_utils): add trajectory class
  * Update CMakeLists.txt
  * fix
  * fix package.xml
  * update crop
  * revert crtp change
  * update package.xml
  * updating...
  * update
  * solve build problem
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Satoshi OTA, Yukinari Hisaki, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* feat(autoware_trajectory): move trajectory_container from autoware_motion_utils to a new package (`#9253 <https://github.com/autowarefoundation/autoware.universe/issues/9253>`_)
  * create trajectory container package
  * update
  * update
  * style(pre-commit): autofix
  * update codeowner
  * update
  * fix cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_motion_utils): add new trajectory class (`#8693 <https://github.com/autowarefoundation/autoware.universe/issues/8693>`_)
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
  * feat(autoware_motion_utils): add trajectory class
  * Update CMakeLists.txt
  * fix
  * fix package.xml
  * update crop
  * revert crtp change
  * update package.xml
  * updating...
  * update
  * solve build problem
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Yukinari Hisaki, Yutaka Kondo

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
