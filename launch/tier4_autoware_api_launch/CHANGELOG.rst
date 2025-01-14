^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_autoware_api_launch
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

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
* chore(autoware_default_adapi)!: prefix autoware to package name (`#8533 <https://github.com/autowarefoundation/autoware.universe/issues/8533>`_)
* refactor(autoware_path_distance_calculator): prefix package and namespace with autoware (`#8085 <https://github.com/autowarefoundation/autoware.universe/issues/8085>`_)
* Contributors: Esteve Fernandez, Takagi, Isamu, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* chore: update api package maintainers (`#6086 <https://github.com/autowarefoundation/autoware.universe/issues/6086>`_)
  * update api maintainers
  * fix
  ---------
* chore: update maintainer (`#4140 <https://github.com/autowarefoundation/autoware.universe/issues/4140>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
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
* feat(tier4_autoware_api_launch): suppress rtc_controller's info (`#3020 <https://github.com/autowarefoundation/autoware.universe/issues/3020>`_)
  * feat(tier4_autoware_api_launch): suppress rtc_controller's info
  * Update launch/tier4_autoware_api_launch/launch/autoware_api.launch.xml
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  ---------
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
* feat: replace tier4_ad_api_adaptor with default_ad_api (`#2438 <https://github.com/autowarefoundation/autoware.universe/issues/2438>`_)
  * feat: add options
  * fix: rosbridge max message size
  * feat: remove old apis
  * feat: remove api adaptors except rtc controller
  * feat: remove external engage
  * feat: add deprecated api option
  * fix: path
* fix(tier4_autoware_api_launch): add rosbridge_server dependency (`#2470 <https://github.com/autowarefoundation/autoware.universe/issues/2470>`_)
* chore: add api maintainers (`#2361 <https://github.com/autowarefoundation/autoware.universe/issues/2361>`_)
* feat: remove launch for old routing api (`#2136 <https://github.com/autowarefoundation/autoware.universe/issues/2136>`_)
* feat(autoware_api_launch): delete image base64 converter launch (`#2131 <https://github.com/autowarefoundation/autoware.universe/issues/2131>`_)
  delete image base64 converter launch
* feat(tier4_autoware_api_launch): add rtc_controller to api launch (`#1919 <https://github.com/autowarefoundation/autoware.universe/issues/1919>`_)
* fix: add adapi dependency (`#1892 <https://github.com/autowarefoundation/autoware.universe/issues/1892>`_)
* feat: move adapi rviz adaptor  (`#1900 <https://github.com/autowarefoundation/autoware.universe/issues/1900>`_)
  feat: move adapi rviz adaptor
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
* chore(planning/control packages): organized authors and maintainers (`#1610 <https://github.com/autowarefoundation/autoware.universe/issues/1610>`_)
  * organized planning authors and maintainers
  * organized control authors and maintainers
  * fix typo
  * fix colcon test
  * fix
  Update control/external_cmd_selector/package.xml
  Update control/vehicle_cmd_gate/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/motion_velocity_smoother/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/freespace_planner/package.xml
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix
  * fix
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
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
* feat(tier4_autoware_api_launch): add some arguments (`#1324 <https://github.com/autowarefoundation/autoware.universe/issues/1324>`_)
  * feat(tier4_autoware_api_launch): add some arguments
  * fix launch py
  * deal with review
  * deal with review
  * deal with review
* fix(tier4_autoware_api_launch): add group tag (`#1235 <https://github.com/autowarefoundation/autoware.universe/issues/1235>`_)
* fix(tier4_autoware_api_launch): typo (`#1047 <https://github.com/autowarefoundation/autoware.universe/issues/1047>`_)
  I will write release note, thank you.
* feat(tier4_autoware_api_launch): add rosbridge (`#779 <https://github.com/autowarefoundation/autoware.universe/issues/779>`_)
  * fix(image_projection_based_fusion): modify build error in rolling (`#775 <https://github.com/autowarefoundation/autoware.universe/issues/775>`_)
  * feat(tier4_autoware_api_launch): add rosbridge
  docs(web_controller): rosbridge is automatically launched in tier4_autoware_api_launch
  * docs(web_controller): rosbridge is automatically launched in tier4_autoware_api_launch
  * Update launch/tier4_autoware_api_launch/launch/autoware_api.launch.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* chore: replace topic tools (`#986 <https://github.com/autowarefoundation/autoware.universe/issues/986>`_)
  * chore: replace topic tools
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* feat(tier4_autoware_api_launch): add tier4_autoware_api_launch package (`#658 <https://github.com/autowarefoundation/autoware.universe/issues/658>`_)
  * feat(tier4_autoware_api_launch): add tier4_autoware_api_launch package
  * ci(pre-commit): autofix
  * fix(tier4_autoware_api_launch): fix the command in the README instructions
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Daisuke Nishimatsu, Kah Hooi Tan, Kenji Miyake, Ryohsuke Mitsudome, Shumpei Wakabayashi, Takagi, Isamu, Takayuki Murooka, Vincent Richard, Xinyu Wang, yabuta
