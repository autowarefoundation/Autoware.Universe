^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_ndt_scan_matcher
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - localization (`#9567 <https://github.com/autowarefoundation/autoware.universe/issues/9567>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_ndt_scan_matcher): remove unsed functions (`#9387 <https://github.com/autowarefoundation/autoware.universe/issues/9387>`_)
* perf(autoware_ndt_scan_matcher): remove evecs\_, evals\_ of Leaf for memory efficiency (`#9281 <https://github.com/autowarefoundation/autoware.universe/issues/9281>`_)
  * fix(lane_change): correct computation of maximum lane changing length threshold (`#9279 <https://github.com/autowarefoundation/autoware.universe/issues/9279>`_)
  fix computation of maximum lane changing length threshold
  * perf: remove evecs, evals from Leaf
  * perf: remove evecs, evals from Leaf
  ---------
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(autoware_ndt_scan_matcher): fix cppcheck unusedFunction (`#9275 <https://github.com/autowarefoundation/autoware.universe/issues/9275>`_)
* fix(autoware_ndt_scan_matcher): reduce initial_pose_estimation.particles_num from 200 to 100 on tests (`#9218 <https://github.com/autowarefoundation/autoware.universe/issues/9218>`_)
  Reduced initial_pose_estimation.particles_num from 200 to 100 on tests
* refactor(ndt_scan_matcher, ndt_omp): move ndt_omp into ndt_scan_matcher (`#8912 <https://github.com/autowarefoundation/autoware.universe/issues/8912>`_)
  * Moved ndt_omp into ndt_scan_matcher
  * Added Copyright
  * style(pre-commit): autofix
  * Fixed include
  * Fixed cast style
  * Fixed include
  * Fixed honorific title
  * Fixed honorific title
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed hierarchy
  * Fixed NVTP to NVTL
  * Added cspell:ignore
  * Fixed miss spell
  * style(pre-commit): autofix
  * Fixed include
  * Renamed applyFilter
  * Moved ***_impl.hpp from include/ to src/
  * style(pre-commit): autofix
  * Fixed variable scope
  * Fixed to pass by reference
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Fumiya Watanabe, Kento Osa, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, SakodaShintaro, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* perf(autoware_ndt_scan_matcher): remove evecs\_, evals\_ of Leaf for memory efficiency (`#9281 <https://github.com/autowarefoundation/autoware.universe/issues/9281>`_)
  * fix(lane_change): correct computation of maximum lane changing length threshold (`#9279 <https://github.com/autowarefoundation/autoware.universe/issues/9279>`_)
  fix computation of maximum lane changing length threshold
  * perf: remove evecs, evals from Leaf
  * perf: remove evecs, evals from Leaf
  ---------
  Co-authored-by: mkquda <168697710+mkquda@users.noreply.github.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(autoware_ndt_scan_matcher): fix cppcheck unusedFunction (`#9275 <https://github.com/autowarefoundation/autoware.universe/issues/9275>`_)
* fix(autoware_ndt_scan_matcher): reduce initial_pose_estimation.particles_num from 200 to 100 on tests (`#9218 <https://github.com/autowarefoundation/autoware.universe/issues/9218>`_)
  Reduced initial_pose_estimation.particles_num from 200 to 100 on tests
* refactor(ndt_scan_matcher, ndt_omp): move ndt_omp into ndt_scan_matcher (`#8912 <https://github.com/autowarefoundation/autoware.universe/issues/8912>`_)
  * Moved ndt_omp into ndt_scan_matcher
  * Added Copyright
  * style(pre-commit): autofix
  * Fixed include
  * Fixed cast style
  * Fixed include
  * Fixed honorific title
  * Fixed honorific title
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed include hierarchy
  * style(pre-commit): autofix
  * Fixed hierarchy
  * Fixed NVTP to NVTL
  * Added cspell:ignore
  * Fixed miss spell
  * style(pre-commit): autofix
  * Fixed include
  * Renamed applyFilter
  * Moved ***_impl.hpp from include/ to src/
  * style(pre-commit): autofix
  * Fixed variable scope
  * Fixed to pass by reference
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Kento Osa, Ryuta Kambe, SakodaShintaro, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(localization_util)!: prefix package and namespace with autoware (`#8922 <https://github.com/autowarefoundation/autoware.universe/issues/8922>`_)
  add autoware prefix to localization_util
* refactor(ndt_scan_matcher)!: prefix package and namespace with autoware (`#8904 <https://github.com/autowarefoundation/autoware.universe/issues/8904>`_)
  add autoware\_ prefix
* Contributors: Masaki Baba, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
