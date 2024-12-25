^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_qp_interface
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
* fix: fix package names in changelog files (`#9500 <https://github.com/autowarefoundation/autoware.universe/issues/9500>`_)
* fix(autoware_osqp_interface): fix clang-tidy errors (`#9440 <https://github.com/autowarefoundation/autoware.universe/issues/9440>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(qp_interface): prefix package and namespace with autoware (`#9236 <https://github.com/autowarefoundation/autoware.universe/issues/9236>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(qp_interface): prefix package and namespace with autoware (`#9236 <https://github.com/autowarefoundation/autoware.universe/issues/9236>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

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
