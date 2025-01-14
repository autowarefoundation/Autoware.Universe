^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_shape_estimation
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
* fix(cpplint): include what you use - perception (`#9569 <https://github.com/autowarefoundation/autoware.universe/issues/9569>`_)
* refactor: correct spelling (`#9528 <https://github.com/autowarefoundation/autoware.universe/issues/9528>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(cuda_utils): prefix package and namespace with autoware (`#9171 <https://github.com/autowarefoundation/autoware.universe/issues/9171>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

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
* refactor(cuda_utils): prefix package and namespace with autoware (`#9171 <https://github.com/autowarefoundation/autoware.universe/issues/9171>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* feat(autoware_shape_estimation): add reference object based corrector (`#9148 <https://github.com/autowarefoundation/autoware.universe/issues/9148>`_)
  * add object based corrector
  * apply cppcheck suggestion
  * fix typo
  ---------
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace (`#9099 <https://github.com/autowarefoundation/autoware.universe/issues/9099>`_)
  * refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace
  * refactor(tensorrt_common): directory structure
  * style(pre-commit): autofix
  * fix(tensorrt_common): correct package name for logging
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* test(autoware_shape_estimation): add unit testing for `ShapeEstimationNode` (`#8740 <https://github.com/autowarefoundation/autoware.universe/issues/8740>`_)
  test: add unit testing for `ShapeEstimationNode`
* fix(autoware_shape_estimation): fix unusedFunction (`#8575 <https://github.com/autowarefoundation/autoware.universe/issues/8575>`_)
  * fix:unusedFunction
  * fix:unusedFunction
  * fix:end of file issues
  * fix:copyright
  ---------
* fix(autoware_shape_estimation): resolve undefined reference to `~TrtShapeEstimator()` (`#8738 <https://github.com/autowarefoundation/autoware.universe/issues/8738>`_)
  fix: resolve undefined reference to `~TrtShapeEstimator()`
* feat(shape_estimation): add ml shape estimation (`#7860 <https://github.com/autowarefoundation/autoware.universe/issues/7860>`_)
  * feat(shape_estimation): add ml shape estimation
  * style(pre-commit): autofix
  * feat(shape_estimation): fix exceed objects
  * style(pre-commit): autofix
  * feat(shape_estimation): fix indent
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(shape_estimation): add package name prefix of autoware\_ (`#7999 <https://github.com/autowarefoundation/autoware.universe/issues/7999>`_)
  * refactor(shape_estimation): add package name prefix of autoware\_
  * style(pre-commit): autofix
  * fix: mising prefix
  * fix: cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Amadeusz Szymko, Kaan Çolak, Kotaro Uetake, Masaki Baba, Yutaka Kondo, badai nguyen, kobayu858

0.26.0 (2024-04-03)
-------------------
