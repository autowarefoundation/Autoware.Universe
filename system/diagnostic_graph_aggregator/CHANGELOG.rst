^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package diagnostic_graph_aggregator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - system (`#9573 <https://github.com/autowarefoundation/autoware.universe/issues/9573>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(diagnostic_graph_aggregator): implement diagnostic graph dump functionality (`#9261 <https://github.com/autowarefoundation/autoware.universe/issues/9261>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(diagnostic_graph_aggregator): implement diagnostic graph dump functionality (`#9261 <https://github.com/autowarefoundation/autoware.universe/issues/9261>`_)
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
* fix(diagnostic_graph_aggregator): fix unusedFunction (`#8580 <https://github.com/autowarefoundation/autoware.universe/issues/8580>`_)
  fix: unusedFunction
  Co-authored-by: kobayu858 <129580202+kobayu858@users.noreply.github.com>
* fix(diagnostic_graph_aggregator): fix noConstructor (`#8508 <https://github.com/autowarefoundation/autoware.universe/issues/8508>`_)
  fix:noConstructor
* fix(diagnostic_graph_aggregator): fix cppcheck warning of functionStatic (`#8266 <https://github.com/autowarefoundation/autoware.universe/issues/8266>`_)
  * fix: deal with functionStatic warning
  * suppress warning by comment
  ---------
* fix(diagnostic_graph_aggregator): fix uninitMemberVar (`#8313 <https://github.com/autowarefoundation/autoware.universe/issues/8313>`_)
  * fix:funinitMemberVar
  * fix:funinitMemberVar
  * fix:uninitMemberVar
  * fix:clang format
  ---------
* fix(diagnostic_graph_aggregator): fix functionConst (`#8279 <https://github.com/autowarefoundation/autoware.universe/issues/8279>`_)
  * fix:functionConst
  * fix:functionConst
  * fix:clang format
  ---------
* fix(diagnostic_graph_aggregator): fix constParameterReference (`#8054 <https://github.com/autowarefoundation/autoware.universe/issues/8054>`_)
  fix:constParameterReference
* fix(diagnostic_graph_aggregator): fix constVariableReference (`#8062 <https://github.com/autowarefoundation/autoware.universe/issues/8062>`_)
  * fix:constVariableReference
  * fix:constVariableReference
  * fix:constVariableReference
  * fix:constVariableReference
  ---------
* fix(diagnostic_graph_aggregator): fix shadowFunction (`#7838 <https://github.com/autowarefoundation/autoware.universe/issues/7838>`_)
  * fix(diagnostic_graph_aggregator): fix shadowFunction
  * feat: modify variable name
  ---------
  Co-authored-by: Takagi, Isamu <isamu.takagi@tier4.jp>
* fix(diagnostic_graph_aggregator): fix uselessOverride warning (`#7768 <https://github.com/autowarefoundation/autoware.universe/issues/7768>`_)
  * fix(diagnostic_graph_aggregator): fix uselessOverride warning
  * restore and suppress inline
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(diagnostic_graph_aggregator): fix shadowArgument warning in create_unit_config (`#7664 <https://github.com/autowarefoundation/autoware.universe/issues/7664>`_)
* feat(diagnostic_graph_aggregator): componentize node (`#7025 <https://github.com/autowarefoundation/autoware.universe/issues/7025>`_)
* fix(diagnostic_graph_aggregator): fix a bug where unit links were incorrectly updated (`#6932 <https://github.com/autowarefoundation/autoware.universe/issues/6932>`_)
  fix(diagnostic_graph_aggregator): fix unit link filter
* feat: remake diagnostic graph packages (`#6715 <https://github.com/autowarefoundation/autoware.universe/issues/6715>`_)
* Contributors: Hayate TOBA, Koichi98, Ryuta Kambe, Takagi, Isamu, Yutaka Kondo, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
* feat(diagnostic_graph_aggregator): update tools (`#6614 <https://github.com/autowarefoundation/autoware.universe/issues/6614>`_)
* docs(diagnostic_graph_aggregator): update documents (`#6613 <https://github.com/autowarefoundation/autoware.universe/issues/6613>`_)
* feat(diagnostic_graph_aggregator): add dump tool (`#6427 <https://github.com/autowarefoundation/autoware.universe/issues/6427>`_)
* feat(diagnostic_graph_aggregator): change default publish rate (`#5872 <https://github.com/autowarefoundation/autoware.universe/issues/5872>`_)
* feat(diagnostic_graph_aggregator): rename system_diagnostic_graph package (`#5827 <https://github.com/autowarefoundation/autoware.universe/issues/5827>`_)
* Contributors: Takagi, Isamu
