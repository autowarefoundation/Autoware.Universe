^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_adapi_specs
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix: fix package names in changelog files (`#9500 <https://github.com/autowarefoundation/autoware.universe/issues/9500>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
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
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
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
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix: adapi vehicle topic qos (`#7847 <https://github.com/autowarefoundation/autoware.universe/issues/7847>`_)
* feat(default_ad_api): add heratbeat api (`#6969 <https://github.com/autowarefoundation/autoware.universe/issues/6969>`_)
  * feat(default_ad_api): add heratbeat api
  * fix node dying
  ---------
* Contributors: Takagi, Isamu, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* feat(default_ad_api): add door api (`#5737 <https://github.com/autowarefoundation/autoware.universe/issues/5737>`_)
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* feat(default_ad_api): add object recognition api (`#2887 <https://github.com/autowarefoundation/autoware.universe/issues/2887>`_)
  * add object recognition api
  * add unorder map
  * pre-commit
  * add missing time_span
  * change naming
  * update message
  * change style
  * change topic naming
  ---------
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* docs: add readme for interface packages (`#4235 <https://github.com/autowarefoundation/autoware.universe/issues/4235>`_)
  add readme for interface packages
* feat: add vehicle status api (`#2930 <https://github.com/autowarefoundation/autoware.universe/issues/2930>`_)
  * add vehicle status api
  * update msgs
  * change naming
  * ada none
  * change naming
  * add publish geoposition
  * add door status
  * change variable naming
  * change variable naming
  * update license
  * fix gps convert
  * add support for UTM reverse
  * fix naming
  * update naming
  * fix naming
  * remote door status
  * clean up vehicle.hpp
  * fix missing declare
  * move convert to cpp
  * move convert to timer callback
  * set to nan when no projector info
  * added checking message valid
  * fix msgs
  ---------
* feat(default_ad_api): add vehicle info api (`#2984 <https://github.com/autowarefoundation/autoware.universe/issues/2984>`_)
  * feat(default_ad_api): add vehicle dimensions api
  * feat: add footprint
  * update api name
  ---------
* feat(autoware_api_specs): change topic depth of route state api and localization state api (`#3757 <https://github.com/autowarefoundation/autoware.universe/issues/3757>`_)
  fix depth
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* chore: update codeowners (`#3513 <https://github.com/autowarefoundation/autoware.universe/issues/3513>`_)
* feat(default_ad_api): add route change api (`#3197 <https://github.com/autowarefoundation/autoware.universe/issues/3197>`_)
  * feat: add route change api
  * fix: reroute
  ---------
* feat(default_ad_api): add planning api (`#2481 <https://github.com/autowarefoundation/autoware.universe/issues/2481>`_)
  * feat(default_ad_api): add planning api
  * feat: complement velocity factor
  * feat: add stop check
  * feat: make the same process into a function
  * feat: update visualizer
  * fix: remove flake8 test
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* feat(default_ad_api): add fail-safe api (`#2295 <https://github.com/autowarefoundation/autoware.universe/issues/2295>`_)
* feat(default_ad_api): add motion api  (`#1809 <https://github.com/autowarefoundation/autoware.universe/issues/1809>`_)
  * feat(autoware_ad_api_specs): define motion interface
  * feat(default_ad_api): add motion api
  * feat: modify motion api
  * feat: modify motion api
  * feat: add error code
  * feat: move adapi messages
  * feat: update message type
  * feat(component_interface_utils): apply message change
  * feat: apply status type change
  * feat: change message type
  * feat: change message name
  * fix: fix state
  * feat: add option
  * feat: modify state name
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat(autoware_ad_api_specs): define operation mode interface (`#1570 <https://github.com/autowarefoundation/autoware.universe/issues/1570>`_)
  * feat(autoware_ad_api_msgs): define operation mode interface
  * fix: add message
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat: move adapi message
  * feat: change message type
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* feat(autoware_ad_api_specs): define motion interface (`#1808 <https://github.com/autowarefoundation/autoware.universe/issues/1808>`_)
  * feat(autoware_ad_api_specs): define motion interface
  * feat: add error code
  * feat: move adapi messages
  * feat(component_interface_utils): apply message change
  * feat: change message type
* feat(autoware_ad_api_msgs): replace adapi message (`#1897 <https://github.com/autowarefoundation/autoware.universe/issues/1897>`_)
* feat(autoware_ad_api_specs): define localization interface (`#1560 <https://github.com/autowarefoundation/autoware.universe/issues/1560>`_)
  feat(autoware_ad_api_msgs): define localization interface
* feat(autoware_ad_api_specs): define routing interface (`#1559 <https://github.com/autowarefoundation/autoware.universe/issues/1559>`_)
  * feat(autoware_ad_api_msgs): define routing interface
  * feat: rename route body message
  * feat: rename route state
* feat(autoware_ad_api_specs): modify interface version api to use spec package  (`#1677 <https://github.com/autowarefoundation/autoware.universe/issues/1677>`_)
* Contributors: Kah Hooi Tan, Takagi, Isamu, Vincent Richard, yabuta
