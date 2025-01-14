^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_smart_mpc_trajectory_follower
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_smart_mpc_trajectory_follower): fix clang-diagnostic-ignored-optimization-argument (`#9437 <https://github.com/autowarefoundation/autoware.universe/issues/9437>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(autoware_smart_mpc_trajectory_follower): fix unusedFunction (`#8553 <https://github.com/autowarefoundation/autoware.universe/issues/8553>`_)
  fix:unusedFunction
* fix(autoware_smart_mpc_trajectory_follower): fix uninitMemberVar (`#8346 <https://github.com/autowarefoundation/autoware.universe/issues/8346>`_)
  fix: uninitMemberVar
  Co-authored-by: kobayu858 <129580202+kobayu858@users.noreply.github.com>
* refactor(smart_mpc_trajectory_folower): modify pacakge structure and install without setup.py (`#8268 <https://github.com/autowarefoundation/autoware.universe/issues/8268>`_)
  * refactor(smart_mpc_trajectory_follower): modify pacakge structure and install without setup.py
  * import proxima calc
  * use ament_get_python_install_dir
  * add <depend>python3-torch</depend>
  * remove pip from readme
  * remove str conversion from open
  * sort in cmake
  ---------
* feat(smart_mpc_trajectory_follower): enhance performance with LSTM and compensation, add compensator outside of MPC (`#7696 <https://github.com/autowarefoundation/autoware.universe/issues/7696>`_)
  * update smart mpc package
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/python_simulator/data_collection_utils.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/python_simulator/data_collection_utils.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/scripts/proxima_calc.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/scripts/drive_iLQR.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/python_simulator/pure_pursuit_gain_updater.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/scripts/proxima_calc.cpp
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/python_simulator/pure_pursuit_gain_updater.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * style(pre-commit): autofix
  * modified commentout
  * fixed duplicate conditions in run_auto_test.py
  * doc: add description of kernel density estimation
  * rename some parameters and remove unnecessary parameters
  * style(pre-commit): autofix
  * Fixed links in README.md
  * add a sample of trained models
  * style(pre-commit): autofix
  * doc: remove japanese comment
  * update README.md
  * add whitespace
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/python_simulator/pure_pursuit_gain_updater.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * Update control/autoware_smart_mpc_trajectory_follower/autoware_smart_mpc_trajectory_follower/scripts/drive_mppi.py
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  * style(pre-commit): autofix
  * Some files were refactored
  * const on member functions that do not change member variables
  * bug fixed
  ---------
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: asei-proxima <asei.inoue@proxima-ai-tech.com>
* fix(smart_mpc_trajectory_folower): fix running by adding control_state and changing msg/package_name (`#7666 <https://github.com/autowarefoundation/autoware.universe/issues/7666>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* ci(pre-commit): autoupdate (`#7499 <https://github.com/autowarefoundation/autoware.universe/issues/7499>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* chore(smart_mpc_trajectory_follower): add prefix autoware\_ to smart_mpc_trajectory_follower (`#7367 <https://github.com/autowarefoundation/autoware.universe/issues/7367>`_)
  * add prefix
  * fix pre-commit
  ---------
* Contributors: Go Sakayori, Hayate TOBA, Kosuke Takeuchi, Takayuki Murooka, Yutaka Kondo, awf-autoware-bot[bot], kobayu858, masayukiaino

0.26.0 (2024-04-03)
-------------------
