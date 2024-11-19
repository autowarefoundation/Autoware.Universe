^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_bytetrack
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace (`#9099 <https://github.com/autowarefoundation/autoware.universe/issues/9099>`_)
  * refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace
  * refactor(tensorrt_common): directory structure
  * style(pre-commit): autofix
  * fix(tensorrt_common): correct package name for logging
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(kalman_filter): prefix package and namespace with autoware (`#7787 <https://github.com/autowarefoundation/autoware.universe/issues/7787>`_)
  * refactor(kalman_filter): prefix package and namespace with autoware
  * move headers to include/autoware/
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(bytetrack):  fix namespace and directory structure (`#8125 <https://github.com/autowarefoundation/autoware.universe/issues/8125>`_)
  * chore: fix namespace
  * chore: change include name
  * refactor: rename perception/bytetrack to perception/autoware_bytetrack
  * refactor: fix node exe
  ---------
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Yoshi Ri, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
