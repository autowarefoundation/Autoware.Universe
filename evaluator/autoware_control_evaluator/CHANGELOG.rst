^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_control_evaluator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(control_evaluator): add goal accuracy longitudinal, lateral, yaw (`#9155 <https://github.com/autowarefoundation/autoware.universe/issues/9155>`_)
  * feat(control_evaluator): add goal accuracy longitudinal, lateral, yaw
  * style(pre-commit): autofix
  * fix: content of kosuke55-san comments
  * fix: variable name
  * fix: variable name
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* test(autoware_control_evaluator): add test for autoware_control_evaluator. (`#9114 <https://github.com/autowarefoundation/autoware.universe/issues/9114>`_)
  * init
  * tmp save.
  * save, there is a bug
  * update package.xml
  * coverage rate 64.5
  * remove comments.
  ---------
* docs(control_evaluator): update readme (`#8829 <https://github.com/autowarefoundation/autoware.universe/issues/8829>`_)
  * update readme
  * add maintainer
  * Update evaluator/autoware_control_evaluator/package.xml
  Add extra maintainer
  Co-authored-by: Tiankui Xian <1041084556@qq.com>
  ---------
  Co-authored-by: Tiankui Xian <1041084556@qq.com>
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
* feat(planning_evaluator,control_evaluator, evaluator utils): add diagnostics subscriber to planning eval (`#7849 <https://github.com/autowarefoundation/autoware.universe/issues/7849>`_)
  * add utils and diagnostics subscription to planning_evaluator
  * add diagnostics eval
  * fix input diag in launch
  ---------
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(control_evaluator): use class naming standard and use remapped param name (`#7782 <https://github.com/autowarefoundation/autoware.universe/issues/7782>`_)
  use class naming standard and use remapped param name
* feat(control_evaluator): add lanelet info to the metrics (`#7765 <https://github.com/autowarefoundation/autoware.universe/issues/7765>`_)
  * add route handler
  * add lanelet info to diagnostic
  * add const
  * add kinematic state info
  * clean
  * remove unusde subscriptions
  * clean
  * add shoulder lanelets
  * fix includes
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(control_evaluator): rename to include/autoware/{package_name} (`#7520 <https://github.com/autowarefoundation/autoware.universe/issues/7520>`_)
  * feat(control_evaluator): rename to include/autoware/{package_name}
  * fix
  ---------
* Contributors: Kazunori-Nakajima, Kosuke Takeuchi, Takayuki Murooka, Tiankui Xian, Yukinari Hisaki, Yutaka Kondo, danielsanchezaran

0.26.0 (2024-04-03)
-------------------
