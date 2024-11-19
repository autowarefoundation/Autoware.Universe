^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package qp_interface
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(qp_interface): fix unreadVariable (`#8349 <https://github.com/autowarefoundation/autoware.universe/issues/8349>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* perf(velocity_smoother): use ProxQP for faster optimization (`#8028 <https://github.com/autowarefoundation/autoware.universe/issues/8028>`_)
  * perf(velocity_smoother): use ProxQP for faster optimization
  * consider max_iteration
  * disable warm start
  * fix test
  ---------
* refactor(qp_interface): clean up the code (`#8029 <https://github.com/autowarefoundation/autoware.universe/issues/8029>`_)
  * refactor qp_interface
  * variable names: m_XXX -> XXX\_
  * update
  * update
  ---------
* fix(fake_test_node, osqp_interface, qp_interface): remove unnecessary cppcheck inline suppressions (`#7855 <https://github.com/autowarefoundation/autoware.universe/issues/7855>`_)
  * fix(fake_test_node, osqp_interface, qp_interface): remove unnecessary cppcheck inline suppressions
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Ryuta Kambe, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* feat(qp_interface): support proxqp (`#3699 <https://github.com/autowarefoundation/autoware.universe/issues/3699>`_)
  * feat(qp_interface): add proxqp interface
  * update
  ---------
* feat(qp_interface): add new package which will contain various qp solvers (`#3697 <https://github.com/autowarefoundation/autoware.universe/issues/3697>`_)
* Contributors: Takayuki Murooka
