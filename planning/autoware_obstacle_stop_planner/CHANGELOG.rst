^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_obstacle_stop_planner
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* fix(obstacle_stop_planner): remove stop reason (`#9465 <https://github.com/autowarefoundation/autoware.universe/issues/9465>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_obstacle_stop_planner): fix cppcheck warnings (`#9388 <https://github.com/autowarefoundation/autoware.universe/issues/9388>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Yutaka Kondo

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
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* chore(planning): consistent parameters with autoware_launch (`#8915 <https://github.com/autowarefoundation/autoware.universe/issues/8915>`_)
  * chore(planning): consistent parameters with autoware_launch
  * update
  * fix json schema
  ---------
* fix(other_planning_packages): align the parameters with launcher (`#8793 <https://github.com/autowarefoundation/autoware.universe/issues/8793>`_)
  * parameters in planning/others aligned
  * update json
  ---------
* fix(autoware_obstacle_stop_planner): register obstacle stop planner node with autoware scoping (`#8512 <https://github.com/autowarefoundation/autoware.universe/issues/8512>`_)
  Register node plugin with autoware scoping
* fix(autoware_obstacle_stop_planner): fix unusedFunction (`#8643 <https://github.com/autowarefoundation/autoware.universe/issues/8643>`_)
  fix:unusedFunction
* refactor(autoware_obstacle_stop_planner): rework parameters (`#7795 <https://github.com/autowarefoundation/autoware.universe/issues/7795>`_)
* fix(autoware_obstacle_stop_planner): fix cppcheck warning of functionStatic (`#8264 <https://github.com/autowarefoundation/autoware.universe/issues/8264>`_)
  * fix: deal with functionStatic warnings
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_obstacle_stop_planner): fix functionConst (`#8282 <https://github.com/autowarefoundation/autoware.universe/issues/8282>`_)
  fix:functionConst
* fix(autoware_obstacle_stop_planner): fix passedByValue (`#8189 <https://github.com/autowarefoundation/autoware.universe/issues/8189>`_)
  fix:passedByValue
* fix(autoware_obstacle_stop_planner): fix funcArgNamesDifferent (`#8018 <https://github.com/autowarefoundation/autoware.universe/issues/8018>`_)
  * fix:funcArgNamesDifferent
  * fix:funcArgNamesDifferent
  ---------
* refactor(autoware_obstacle_stop_planner): prefix package and namespace with autoware (`#7565 <https://github.com/autowarefoundation/autoware.universe/issues/7565>`_)
  * refactor(autoware_obstacle_stop_planner): prefix package and namespace with autoware
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Mukunda Bharatheesha, Takayuki Murooka, Yutaka Kondo, Yuxin Wang, Zhe Shen, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
