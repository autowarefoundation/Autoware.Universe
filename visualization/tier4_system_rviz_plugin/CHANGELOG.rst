^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package tier4_system_rviz_plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore: move rviz plugins from common to visualization/ folder (`#9417 <https://github.com/autowarefoundation/autoware.universe/issues/9417>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe

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
* feat(system_error_monitor): remove system error monitor (`#8929 <https://github.com/autowarefoundation/autoware.universe/issues/8929>`_)
  * feat: delete-system-error-monitor-from-autoware
  * feat: remove unnecessary params
  ---------
* fix(tier4_system_rviz_plugin): fix cppcheck warning of virtualCallInConstructor (`#8378 <https://github.com/autowarefoundation/autoware.universe/issues/8378>`_)
  fix: deal with virtualCallInConstructor warning
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Ryohsuke Mitsudome, Takayuki Murooka, TetsuKawa, Yutaka Kondo, taisa1

0.26.0 (2024-04-03)
-------------------
* feat(tier4_system_rviz_plugin): improve visualization results and logics (`#5222 <https://github.com/autowarefoundation/autoware.universe/issues/5222>`_)
  * Add Init Localization and Init Planning Check; Add error list check
  * style(pre-commit): autofix
  * int casting problem updated
  * style(pre-commit): autofix
  * improve if statement writing styles
  * style(pre-commit): autofix
  * FIX ci
  * subscribe to ADAPI for more stable status checking
  * style(pre-commit): autofix
  * try using level attribute only
  * fix hpp so that it just look like the original hpp
  * style(pre-commit): autofix
  * set default value to NO_FAULT
  ---------
  Co-authored-by: Owen-Liuyuxuan <uken.ryu@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(common): extern template for motion_utils / remove tier4_autoware_utils.hpp / remove motion_utis.hpp (`#5027 <https://github.com/autowarefoundation/autoware.universe/issues/5027>`_)
* feat(tier4_system_rviz_plugin): add package (`#4945 <https://github.com/autowarefoundation/autoware.universe/issues/4945>`_)
  * feat(tier4_system_rviz_plugin): add package
  * update maintainer
  * style(pre-commit): autofix
  * minor update
  * style(pre-commit): autofix
  * update some arguments
  * style(pre-commit): autofix
  * fix pre-commit
  * style(pre-commit): autofix
  * update maintainer
  * fix test
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Mamoru Sobue, Yuxuan Liu, kminoda
