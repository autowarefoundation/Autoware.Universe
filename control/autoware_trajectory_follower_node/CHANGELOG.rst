^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_trajectory_follower_node
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - control (`#9565 <https://github.com/autowarefoundation/autoware.universe/issues/9565>`_)
* refactor: correct spelling (`#9528 <https://github.com/autowarefoundation/autoware.universe/issues/9528>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(mpc_lateral_controller): suppress rclcpp_warning/error (`#9382 <https://github.com/autowarefoundation/autoware.universe/issues/9382>`_)
  * feat(mpc_lateral_controller): suppress rclcpp_warning/error
  * fix
  * fix test
  ---------
* fix(autoware_trajectory_follower_node): fix clang-diagnostic-format-security (`#9378 <https://github.com/autowarefoundation/autoware.universe/issues/9378>`_)
* refactor(fake_test_node): prefix package and namespace with autoware (`#9249 <https://github.com/autowarefoundation/autoware.universe/issues/9249>`_)
* feat(trajectory_follower): publsih control horzion (`#8977 <https://github.com/autowarefoundation/autoware.universe/issues/8977>`_)
  * feat(trajectory_follower): publsih control horzion
  * fix typo
  * rename functions and minor refactor
  * add option to enable horizon pub
  * add tests for horizon
  * update docs
  * rename to ~/debug/control_cmd_horizon
  ---------
* fix(control): missing dependency in control components (`#9073 <https://github.com/autowarefoundation/autoware.universe/issues/9073>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kosuke Takeuchi, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, ぐるぐる

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(control): missing dependency in control components (`#9073 <https://github.com/autowarefoundation/autoware.universe/issues/9073>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo, ぐるぐる

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(control): align the parameters with launcher (`#8789 <https://github.com/autowarefoundation/autoware.universe/issues/8789>`_)
  align the control parameters
* feat(autoware_mpc_lateral_controller): add predicted trajectory acconts for input delay (`#8436 <https://github.com/autowarefoundation/autoware.universe/issues/8436>`_)
  * feat: enable delayed initial state for predicted trajectory
  * feat: enable debug publishing of predicted and resampled reference trajectories
  ---------
* feat(pid_longitudinal_controller)!: add acceleration feedback block (`#8325 <https://github.com/autowarefoundation/autoware.universe/issues/8325>`_)
* refactor(control/pid_longitudinal_controller): rework parameters (`#6707 <https://github.com/autowarefoundation/autoware.universe/issues/6707>`_)
  * reset and re-apply refactoring
  * style(pre-commit): autofix
  * .
  * .
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(pid_longitudinal_controller): re-organize diff limit structure and fix state change condition (`#7718 <https://github.com/autowarefoundation/autoware.universe/issues/7718>`_)
  change diff limit structure
  change stopped condition
  define a new param
* fix(controller): revival of dry steering (`#7903 <https://github.com/autowarefoundation/autoware.universe/issues/7903>`_)
  * Revert "fix(autoware_mpc_lateral_controller): delete the zero speed constraint (`#7673 <https://github.com/autowarefoundation/autoware.universe/issues/7673>`_)"
  This reverts commit 69258bd92cb8a0ff8320df9b2302db72975e027f.
  * dry steering
  * add comments
  * add minor fix and modify unit test for dry steering
  ---------
* ci: disable failing tests undetected due to broken regex filter (`#7731 <https://github.com/autowarefoundation/autoware.universe/issues/7731>`_)
* fix(autoware_pid_longitudinal_controller, autoware_trajectory_follower_node): unite diagnostic_updater\_ in PID and MPC. (`#7674 <https://github.com/autowarefoundation/autoware.universe/issues/7674>`_)
  * diag_updater\_ added in PID
  * correct the pointer form
  * pre-commit
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(mpc_lateral_controller): signal a MRM when MPC fails. (`#7016 <https://github.com/autowarefoundation/autoware.universe/issues/7016>`_)
  * mpc fail checker diagnostic added
  * fix some scope issues
  * member attribute added.
  * shared pointer added.
  * member attribute (diag_updater\_) added
  * dependency added.
  * implementation of the MpcLateralController corrected!
  * typo in comment corrected!
  * member method argument corrected
  * delete unnecessary reference mark
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * rebase
  * correct the include
  * pre-commit
  ---------
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control)!: refactor directory structures of the trajectory followers (`#7521 <https://github.com/autowarefoundation/autoware.universe/issues/7521>`_)
  * control_traj
  * add follower_node
  * fix
  ---------
* refactor(pure_pursuit): prefix package and namespace with autoware\_ (`#7301 <https://github.com/autowarefoundation/autoware.universe/issues/7301>`_)
  * RT1-6683 add autoware prefix to package and namepace
  * fix precommit
  ---------
* refactor(trajectory_follower_node): trajectory follower node add autoware prefix (`#7344 <https://github.com/autowarefoundation/autoware.universe/issues/7344>`_)
  * rename trajectory follower node package
  * update dependencies, launch files, and README files
  * fix formats
  * remove autoware\_ prefix from launch arg option
  ---------
* Contributors: Kosuke Takeuchi, Kyoichi Sugahara, M. Fatih Cırıt, Takayuki Murooka, Yuki TAKAGI, Yutaka Kondo, Zhe Shen, Zulfaqar Azmi, mkquda, oguzkaganozt

0.26.0 (2024-04-03)
-------------------
