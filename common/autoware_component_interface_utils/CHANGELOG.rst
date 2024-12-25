^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_component_interface_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

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
* refactor(component_interface_utils): prefix package and namespace with autoware (`#9092 <https://github.com/autowarefoundation/autoware.universe/issues/9092>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* ci(pre-commit): autoupdate (`#7499 <https://github.com/autowarefoundation/autoware.universe/issues/7499>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* Contributors: Yutaka Kondo, awf-autoware-bot[bot]

0.26.0 (2024-04-03)
-------------------
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* chore(component_interface_utils): set log level of debug printing to DEBUG (`#5995 <https://github.com/autowarefoundation/autoware.universe/issues/5995>`_)
* feat: add common interface utils test (`#5173 <https://github.com/autowarefoundation/autoware.universe/issues/5173>`_)
  * add interface utils test
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* feat(component_interface_tools): add service log checker  (`#2503 <https://github.com/autowarefoundation/autoware.universe/issues/2503>`_)
  * feat(component_interface_utils): add service log checker
  * feat(component_interface_tools): add service log checker
  * feat(component_interface_tools): add diagnostics
  * feat: update system error monitor config
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* fix(component_interface_utils): fix error level (`#2322 <https://github.com/autowarefoundation/autoware.universe/issues/2322>`_)
* feat(operation_mode_transition_manager): support ad api (`#1535 <https://github.com/autowarefoundation/autoware.universe/issues/1535>`_)
  * feat(operation_mode_transition_manager): support ad api
  * fix: merge operation mode state message
  * feat(autoware_ad_api_msgs): define operation mode interface
  * fix: add message
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/autoware_ad_api_msgs/operation_mode/msg/OperationModeState.msg
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * feat: apply field name change
  * feat: move adapi message
  * feat: change message type
  * fix: fix build error
  * fix: fix error message
  * WIP
  * feat: add compatibility
  * fix: fix operation mode change when disable autoware control
  * fix: fix operation mode change when autoware control is disabled
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
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
* feat(component_interface_utils): change service log output (`#2022 <https://github.com/autowarefoundation/autoware.universe/issues/2022>`_)
  * feat(component_interface_utils): change client log
  * feat(component_interface_utils): change server log
  * feat(component_interface_utils): add interface
  * feat: add console log
* feat(component_interface_utils): apply message change (`#1921 <https://github.com/autowarefoundation/autoware.universe/issues/1921>`_)
  * feat(component_interface_utils): apply message change
  * feat: add error category
* feat(autoware_ad_api_msgs): replace adapi message (`#1897 <https://github.com/autowarefoundation/autoware.universe/issues/1897>`_)
* feat(autoware_ad_api_specs): define routing interface (`#1559 <https://github.com/autowarefoundation/autoware.universe/issues/1559>`_)
  * feat(autoware_ad_api_msgs): define routing interface
  * feat: rename route body message
  * feat: rename route state
* feat(component_interface_utils): update service utility (`#1429 <https://github.com/autowarefoundation/autoware.universe/issues/1429>`_)
  * feat(component_interface_utils): add exceptions, relay, and synchronous service call
  * feat(autoware_ad_api_msgs): add status code
  * feat(component_interface_utils): fix for hubmle
  * feat(component_interface_utils): fix for galactic
  * docs(component_interface_utils): update readme
  * docs(component_interface_utils): update readme
  * docs(component_interface_utils): fix typo and spell check
  * feat(component_interface_utils): add transform error
  * feat(component_interface_utils): use optional for no timeout
  * docs(component_interface_utils): update readme
  * feat(component_interface_utils): add bind function
* feat(component_interface_utils): add interface classes  (`#899 <https://github.com/autowarefoundation/autoware.universe/issues/899>`_)
  * feat(component_interface_utils): add interface classes
  * feat(default_ad_api): apply the changes of interface utils
  * fix(component_interface_utils): remove old comment
  * fix(component_interface_utils): add client log
  * fix(component_interface_utils): remove unimplemented message
  * docs(component_interface_utils): add design policy
  * docs(component_interface_utils): add comment
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* fix: apply fixes for rolling (`#821 <https://github.com/autowarefoundation/autoware.universe/issues/821>`_)
  * fix(component_interface_utils): add USE_DEPRECATED_TO_YAML
  * fix(lidar_apollo_instance_segmentation): add USE_TF2_GEOMETRY_MSGS_DEPRECATED_HEADER
  * add rclcpp_components to package.xml
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(default_ad_api): add interface version (`#704 <https://github.com/autowarefoundation/autoware.universe/issues/704>`_)
  * feat(default_ad_api): add interface version
  * feat(default_ad_api): add http server
  * feat(default_ad_api): add message readme
  * feat(default_ad_api): modify message readme
  * feat(default_ad_api): fix message type
  * feat(default_ad_api): fix message type
  * feat(default_ad_api): remove unused message
  * feat(component_interface_utils): use full path
  * feat(component_interface_utils): rename package
  * feat(autoware_ad_api_msgs): remove unused message
  * feat(component_interface_utils): add readme and comments
  * feat(default_ad_api): fix api name
  * Update common/autoware_ad_api_msgs/README.md
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * add test
  * fix server name
  * Add comment
  * fix typo
  * rename version api
  * Update system/default_ad_api/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update common/component_interface_utils/include/component_interface_utils/rclcpp/create_interface.hpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Update system/default_ad_api/launch/default_ad_api.launch.py
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Fix for renaming web server script
  * Fix test script for readability
  * Fix test script for readability
  * Add comment
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* Contributors: Kenji Miyake, Takagi, Isamu, Takayuki Murooka, Vincent Richard, shulanbushangshu, yabuta
