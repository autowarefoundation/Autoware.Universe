^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package yabloc_monitor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* chore(localization, map): remove maintainer (`#7940 <https://github.com/autowarefoundation/autoware.universe/issues/7940>`_)
* feat(yabloc_monitor): componentize yabloc_monitor node (`#7509 <https://github.com/autowarefoundation/autoware.universe/issues/7509>`_)
  * change node to component
  * fix launch file & cmake
  ---------
* Contributors: Kento Yabuuchi, Yutaka Kondo, kminoda

0.26.0 (2024-04-03)
-------------------
* chore(yabloc): rework parameters (`#6170 <https://github.com/autowarefoundation/autoware.universe/issues/6170>`_)
  * introduce json schema for ground_server
  * introduce json schema for ll2_decomposer
  * style(pre-commit): autofix
  * fix json in yabloc_common
  * introduce json schema for graph_segment
  * introduce json schema for segment_filter
  * fix yabloc_common schema.json
  * introduce json schema for undistort
  * style(pre-commit): autofix
  * Revert "introduce json schema for ground_server"
  This reverts commit 33d3e609d4e01919d11a86d3c955f53e529ae121.
  * Revert "introduce json schema for graph_segment"
  This reverts commit 00ae417f030324f2dcc7dfb4b867a969ae31aea7.
  * style(pre-commit): autofix
  * introduce json schema for yabloc_monitor
  * introduce json schema for yabloc_particle_filter
  * introduce json schema for yabloc_pose_initializer
  * apply pre-commit
  * fix revert conflict
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add localization & mapping maintainers (`#6085 <https://github.com/autowarefoundation/autoware.universe/issues/6085>`_)
  * Added lm maintainers
  * Add more
  * Fixed maintainer
  ---------
* chore: add maintainer in localization and map packages (`#4501 <https://github.com/autowarefoundation/autoware.universe/issues/4501>`_)
* feat(yabloc_monitor): add yabloc_monitor (`#4395 <https://github.com/autowarefoundation/autoware.universe/issues/4395>`_)
  * feat(yabloc_monitor): add yabloc_monitor
  * style(pre-commit): autofix
  * add readme
  * style(pre-commit): autofix
  * update config
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * update
  * style(pre-commit): autofix
  * remove unnecessary part
  * remove todo
  * fix typo
  * remove unnecessary part
  * update readme
  * shorten function
  * reflect chatgpt
  * style(pre-commit): autofix
  * update
  * cland-tidy
  * style(pre-commit): autofix
  * update variable name
  * fix if name
  * use nullopt (and moved yabloc monitor namespace
  * fix readme
  * style(pre-commit): autofix
  * add dependency
  * style(pre-commit): autofix
  * reflect comment
  * update comment
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Kento Yabuuchi, SakodaShintaro, kminoda
