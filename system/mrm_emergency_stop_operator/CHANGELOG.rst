^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package mrm_emergency_stop_operator
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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
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
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for system modules (`#7249 <https://github.com/autowarefoundation/autoware.universe/issues/7249>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat(mrm_emergency_stop_operator): add support for real time param reconfigure for mrm_emergency_stop (`#6994 <https://github.com/autowarefoundation/autoware.universe/issues/6994>`_)
  add support for real time param reconfigure for mrm_emergency_stop
* Contributors: Kosuke Takeuchi, Ryohsuke Mitsudome, Takayuki Murooka, Yutaka Kondo, danielsanchezaran

0.26.0 (2024-04-03)
-------------------
* chore(mrm_emergency_stop_operator): add a maintainer for mrm operator… (`#3489 <https://github.com/autowarefoundation/autoware.universe/issues/3489>`_)
  chore(mrm_emergency_stop_operator): add a maintainer for mrm operator packages
* docs(mrm_emergency_stop_operator): fix file name (`#4226 <https://github.com/autowarefoundation/autoware.universe/issues/4226>`_)
* style: fix typos (`#3617 <https://github.com/autowarefoundation/autoware.universe/issues/3617>`_)
  * style: fix typos in documents
  * style: fix typos in package.xml
  * style: fix typos in launch files
  * style: fix typos in comments
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(mrm_emergency_stop_operator): fix parameter loading in mrm operators (`#2378 <https://github.com/autowarefoundation/autoware.universe/issues/2378>`_)
  * fix(mrm_emergency_stop_operator): fix parameter loading in mrm operators
  * ci(pre-commit): autofix
  * fix(mrm_emergency_stop_operator): remove os import
  * fix(mrm_emergency_stop_operator): remove unused packages
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(emergency_handler): add a selector for multiple MRM behaviors (`#2070 <https://github.com/autowarefoundation/autoware.universe/issues/2070>`_)
  * feat(emergency_handler): add mrm command and status publishers
  * feat(autoware_ad_api_msgs): define mrm operation srv and mrm status msg
  * feat(emergency_handler): add mrm clients and subscribers
  * feat(mrm_comfortable_stop_operator): ready ros2 node template
  * feat(mrm_comfortable_stop_operator): implemented
  * feat(mrm_comfortable_stop_operator): implement as component
  * chore(mrm_comfortable_stop_operator): add a launch script
  * refactor(mrm_comfortable_stop_operator): remove a xml launch file
  * feat(autoware_ad_api_msgs): change mrm status msg
  * feat(emergency_handler): add mrm operator and mrm behavior updater
  * feat(emergency_handler): add mrm behavior state machine
  * feat(emergency_handler): remap io names
  * fix(emergency_handler): fix request generation
  * fix(emergency_handler): add multi thread execution for service
  * feat(vehicle_cmd_gate): add mrm operation service and status publisher
  * refactor(mrm_comfortable_stop_operator): use MRMBehaviorStatus struct
  * fix(mrm_comfortable_stop_operator): add time stamp for status
  * feat(vehicle_cmd_gate): change system emergency state by mrm operation
  * chore(autoware_ad_api_msgs): remove rti_operating state from mrm status
  * feat(mrm_sudden_stop_operator): add mrm_sudden_stop_operator
  * refactor(autoware_ad_api_msgs): rename from mrm status to mrm state
  * fix(mrm_comfortable_stop_operator): set qos for velocity limit publisher
  * feat(emergency_handler): add mrm state publisher
  * feat(vehicle_cmd_gate): add subscription for mrm_state
  * fix(mrm_sudden_stop_operator): fix control command topic name
  * feat(vehicle_cmd_gate): pub emergency control_cmd according to mrm state
  * feat(emergency_handler): remove emergency control_cmd publisher
  * chore(tier4_control_launch): remap mrm state topic
  * feat(tier4_system_launch): launch mrm operators
  * fix(emergency_handler): fix autoware_ad_api_msgs to autoware_adapi_v1_msgs
  * fix(vehicle_cmd_gate): remove subscribers for emergency_state and mrm operation
  * fix(vehicle_cmd_gate): fix system emergency condition
  * fix(emergency_handler): add stamp for mrm_state
  * fix(mrm_emergency_stop_operator): rename sudden stop to emergency stop
  * fix(vehicle_cmd_gate): remove emergency stop status publisher
  * fix(emergency_handler): replace emergency state to mrm state
  * feat(mrm_emergency_stop_operator): add is_available logic
  * feat(emergency_handler): add use_comfortable_stop param
  * refactor(emergency_handler): rename getCurrentMRMBehavior
  * feat(emergency_handler): add mrm available status for ready conditions
  * feat(emergency_handler): add readme
  * fix(mrm_comfortable_stop_operator): fix update rate
  * refactor(emergency_handler): move MRMBehaviorStatus msg to tier4_system_msgs
  * feat(emergency_handler): describe new io for emergency_handler
  * fix(emergency_handler): remove extra settings
  * fix(mrm_emergency_stop_operator): fix is_available condition
  * fix(mrm_emergency_stop_operator): fix typo
  * ci(pre-commit): autofix
  * fix(mrm_emergency_stop_operator): remove extra descriptions on config files
  * fix(mrm_comfortable_stop_operator): fix typo
  * chore(mrm_comfortable_stop_operator): change words
  * chore(mrm_comfortable_stop_operator): change maintainer infomation
  * fix(emergency_handler): fix acronyms case
  * chore(emergency_handler): add a maintainer
  * fix(emergency_handler): fix to match msg changes
  * fix(vehicle_cmd_gate): remove an extra include
  * ci(pre-commit): autofix
  * fix(emergency_handler): fix topic name spaces
  * fix(emergency_handler): fix acronyms case
  * chore(tier4_system_launch): add a mrm comfortable stop parameter
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Kenji Miyake, Makoto Kurihara, Takamasa Horibe, Vincent Richard
