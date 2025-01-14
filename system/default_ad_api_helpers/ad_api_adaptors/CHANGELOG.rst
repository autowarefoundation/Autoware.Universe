^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package ad_api_adaptors
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
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
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(autoware_ad_api_specs): prefix package and namespace with autoware (`#9250 <https://github.com/autowarefoundation/autoware.universe/issues/9250>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api_specs to adapi_specs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* refactor(autoware_ad_api_specs): prefix package and namespace with autoware (`#9250 <https://github.com/autowarefoundation/autoware.universe/issues/9250>`_)
  * refactor(autoware_ad_api_specs): prefix package and namespace with autoware
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * style(pre-commit): autofix
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api to adapi
  * chore(autoware_adapi_specs): rename ad_api_specs to adapi_specs
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(ad_api_adaptors): rework parameter (`#8796 <https://github.com/autowarefoundation/autoware.universe/issues/8796>`_)
  * refactor(ad_api_adaptors): rework parameter
  * <refactor(ad_api_adaptors): rework parameter>
  * ad_api_adaptors.schema.json
  * style(pre-commit): autofix
  * refactor(ad_api_adaptors): rework parameter
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(map_height_fitter)!: prefix package and namespace with autoware  (`#8421 <https://github.com/autowarefoundation/autoware.universe/issues/8421>`_)
  * add autoware\_ prefix
  * style(pre-commit): autofix
  * remove duplicated dependency
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: SakodaShintaro <shintaro.sakoda@tier4.jp>
* feat(ad_api_adaptors): componentize nodes (`#7022 <https://github.com/autowarefoundation/autoware.universe/issues/7022>`_)
* Contributors: Masaki Baba, Prakash Kannaiah, Takagi, Isamu, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat(map_height_fitter): fitting by vector_map (`#6340 <https://github.com/autowarefoundation/autoware.universe/issues/6340>`_)
  * Added fit_target
  * Fixed group
  * Fixed to run
  * style(pre-commit): autofix
  * Fixed to work by pointcloud_map
  * Fixed comments
  * Added a comment
  * Fixed a comment
  * Fixed to use arg
  * Added info log
  * FIxed default value
  * FIxed default values
  * Updated schema.json
  * Fixed description of fit_target
  * Fixed arg name
  * Restore const
  * Fixed map_height_fitter.param.yaml
  * Fixed map_height_fitter.schema.json
  * style(pre-commit): autofix
  * Removed an unused variable
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* chore(default_ad_api_helpers): update readme topic (`#5258 <https://github.com/autowarefoundation/autoware.universe/issues/5258>`_)
  * chore(default_ad_api_helpers): update readme topic
  * style(pre-commit): autofix
  * update readme
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(map_height_fitter): add service node (`#4128 <https://github.com/autowarefoundation/autoware.universe/issues/4128>`_)
  * add map height fitter node
  * fix response success
  ---------
* docs(ad_api_adaptors): fix readme to remove unused service (`#4117 <https://github.com/autowarefoundation/autoware.universe/issues/4117>`_)
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(default_ad_api): add route change api (`#3197 <https://github.com/autowarefoundation/autoware.universe/issues/3197>`_)
  * feat: add route change api
  * fix: reroute
  ---------
* chore(default_ad_api): add yukkysaito and mitsudome-r to maintainer (`#3440 <https://github.com/autowarefoundation/autoware.universe/issues/3440>`_)
  * chore(default_ad_api): add yukkysaito to maintainer
  * add mitsudome-r instead of kenji-miyake
  ---------
* feat(default_ad_api_helpers): support goal modification for rviz (`#3370 <https://github.com/autowarefoundation/autoware.universe/issues/3370>`_)
* feat(map_height_fitter): change map height fitter to library (`#2724 <https://github.com/autowarefoundation/autoware.universe/issues/2724>`_)
  * feat: move map height fitter
  * feat: remove map height fitter dependency
  * apply to initial pose adaptor
  * feat: get param from map loader
  * feat: modify pose initializer
  * feat: parameterize map loader name
  * docs: update readme
  * feat: add debug code
  * Revert "feat: add debug code"
  This reverts commit 71250342305aad6ac3710625ab2ea1dfd3eca11a.
  * feat: add map fit log
  ---------
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* fix(ad_api_adaptors): fix to merge waypoint (`#2215 <https://github.com/autowarefoundation/autoware.universe/issues/2215>`_)
  * fix(ad_api_adaptors): fix to merge waypoint
  * fix(ad_api_adaptors): update comments and variable name
* feat(autoware_ad_api_msgs): replace adapi message (`#1897 <https://github.com/autowarefoundation/autoware.universe/issues/1897>`_)
* feat(default_ad_api): add localization api  (`#1431 <https://github.com/autowarefoundation/autoware.universe/issues/1431>`_)
  * feat(default_ad_api): add localization api
  * docs: add readme
  * feat: add auto initial pose
  * feat(autoware_ad_api_msgs): define localization interface
  * fix(default_ad_api): fix interface definition
  * feat(default_ad_api): modify interface version api to use spec package
  * feat(default_ad_api): modify interface version api to use spec package
  * fix: pre-commit
  * fix: pre-commit
  * fix: pre-commit
  * fix: copyright
  * feat: split helper package
  * fix: change topic name to local
  * fix: style
  * fix: style
  * fix: style
  * fix: remove needless keyword
  * feat: change api helper node namespace
  * fix: fix launch file path
* feat(default_ad_api): add routing api (`#1494 <https://github.com/autowarefoundation/autoware.universe/issues/1494>`_)
  * feat(default_ad_api): add routing api
  * fix: build error
  * docs: add readme
  * feat: change topic namespace
  * fix: function name
  * fix: remove debug code
  * fix: copyright
  * fix: adaptor name
  * fix: remove macro
  * feat: add launch option for default ad api
  * fix: component interface namespace
  * fix: build error
  * feat: remove start pose
  * feat(autoware_ad_api_msgs): define routing interface
  * feat: rename route body message
  * feat: remove create node macro
  * feat: adaptor package
  * fix: helper node
  * fix: error handling
* Contributors: Kosuke Takeuchi, SakodaShintaro, Takagi, Isamu, Vincent Richard, kminoda
