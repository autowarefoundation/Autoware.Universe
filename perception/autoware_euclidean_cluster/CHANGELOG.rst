^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_euclidean_cluster
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_point_types): prefix namespace with autoware::point_types (`#9169 <https://github.com/autowarefoundation/autoware.universe/issues/9169>`_)
* refactor(autoware_pointcloud_preprocessor): rework crop box parameters (`#8466 <https://github.com/autowarefoundation/autoware.universe/issues/8466>`_)
  * feat: add parameter schema for crop box
  * chore: fix readme
  * chore: remove filter.param.yaml file
  * chore: add negative parameter for voxel grid based euclidean cluster
  * chore: fix schema description
  * chore: fix description of negative param
  ---------
* refactor(pointcloud_preprocessor): prefix package and namespace with autoware (`#7983 <https://github.com/autowarefoundation/autoware.universe/issues/7983>`_)
  * refactor(pointcloud_preprocessor)!: prefix package and namespace with autoware
  * style(pre-commit): autofix
  * style(pointcloud_preprocessor): suppress line length check for macros
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * fix(pointcloud_preprocessor): missing prefix
  * refactor(pointcloud_preprocessor): directory structure (soft)
  * refactor(pointcloud_preprocessor): directory structure (hard)
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* refactor(euclidean_cluster): add package name prefix of autoware\_ (`#8003 <https://github.com/autowarefoundation/autoware.universe/issues/8003>`_)
  * refactor(euclidean_cluster): add package name prefix of autoware\_
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Yi-Hsiang Fang (Vivid), Yutaka Kondo, badai nguyen

0.26.0 (2024-04-03)
-------------------
