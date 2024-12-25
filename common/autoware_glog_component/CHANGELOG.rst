^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package glog_component
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* refactor(glog_component): prefix package and namespace with autoware (`#9302 <https://github.com/autowarefoundation/autoware.universe/issues/9302>`_)
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome

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
* chore(glog): add initialization check (`#6792 <https://github.com/autowarefoundation/autoware.universe/issues/6792>`_)
* Contributors: Takamasa Horibe, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat(glog): add glog in planning and control modules (`#4714 <https://github.com/autowarefoundation/autoware.universe/issues/4714>`_)
  * feat(glog): add glog component
  * formatting
  * remove namespace
  * remove license
  * Update launch/tier4_planning_launch/launch/scenario_planning/lane_driving/motion_planning/motion_planning.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update launch/tier4_planning_launch/launch/scenario_planning/lane_driving/behavior_planning/behavior_planning.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update common/glog_component/CMakeLists.txt
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Update launch/tier4_control_launch/launch/control.launch.py
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * add copyright
  ---------
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* Contributors: Takamasa Horibe
