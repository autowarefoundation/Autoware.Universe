^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package mrm_handler
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* feat(mrm_handler): mrm handler publish emergecy holding (`#9285 <https://github.com/autowarefoundation/autoware.universe/issues/9285>`_)
  * feat: add publisher for emrgency holding
  * modify: fix msg element
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, TetsuKawa, Yutaka Kondo

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
* fix(mrm_handler, emergency_handler): remove unnecessary depend (`#8099 <https://github.com/autowarefoundation/autoware.universe/issues/8099>`_)
  * fix(mrm_handler): remove unnecessary depends
  * fix(emergency_handler): remove unnecessary depends
  ---------
* feat(mrm_handler): input gear command (`#8080 <https://github.com/autowarefoundation/autoware.universe/issues/8080>`_)
  * feat(mrm_handler): input gear command
  * style(pre-commit): autofix
  * fix minor
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(mrm_handler): add check for autonomous mode and do some refactoring (`#8067 <https://github.com/autowarefoundation/autoware.universe/issues/8067>`_)
  * add check for autonomous mode and do some refactoring
  * add comments
  * fix comment
  ---------
* feat(mrm_handler): operate mrm only when autonomous operation mode (`#7784 <https://github.com/autowarefoundation/autoware.universe/issues/7784>`_)
  * feat: add isOperationModeAutonomous() function to MRM handler core
  The code changes add a new function `isOperationModeAutonomous()` to the MRM handler core. This function is used to check if the operation mode is set to autonomous.
  ---------
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* fix(mrm_handler): fix multiCondition warning (`#7543 <https://github.com/autowarefoundation/autoware.universe/issues/7543>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(mrm_handler): fix stop judgement (`#7362 <https://github.com/autowarefoundation/autoware.universe/issues/7362>`_)
  fix stop judgement
  Co-authored-by: Autumn60 <akiro.harada@tier4.jp>
* feat(emergency_handler, mrm_handler): change to read topic by polling (`#7297 <https://github.com/autowarefoundation/autoware.universe/issues/7297>`_)
  * replace Subscription to InterProcessPollingSubscriber
  * sort depend packages list in package.xml
  * fix end of file
  * clang format
  * chore: fix comments
  * replace Subscription to InterProcessPollingSubscriber (mrm_handler)
  ---------
  Co-authored-by: Autumn60 <akiro.harada@tier4.jp>
* refactor(mrm_handler): use switch for state machine (`#7277 <https://github.com/autowarefoundation/autoware.universe/issues/7277>`_)
  * refactor nested if elses
  * delete other commits
  * return for consistency
  ---------
* fix(emergency_handler,mrm_handler): check for ego speed when determining the gear command (`#7264 <https://github.com/autowarefoundation/autoware.universe/issues/7264>`_)
  * check for ego speed when determining the gear command
  * add gear history
  * update msg types
  ---------
  Co-authored-by: veqcc <ryuta.kambe@tier4.jp>
* feat!: replace autoware_auto_msgs with autoware_msgs for system modules (`#7249 <https://github.com/autowarefoundation/autoware.universe/issues/7249>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* feat: componentize-mrm-handler (`#7018 <https://github.com/autowarefoundation/autoware.universe/issues/7018>`_)
* Contributors: Autumn60, Kosuke Takeuchi, Kyoichi Sugahara, Ryohsuke Mitsudome, Ryuta Kambe, Takayuki Murooka, TetsuKawa, Yutaka Kondo, danielsanchezaran

0.26.0 (2024-04-03)
-------------------
* fix(mrm_handler): fix bug in operation mode availability timeout (`#6513 <https://github.com/autowarefoundation/autoware.universe/issues/6513>`_)
  * fix operation mode availability timeout
* feat: add timeouts of request services (`#6532 <https://github.com/autowarefoundation/autoware.universe/issues/6532>`_)
  * feat: add timeouts of request services
  * style(pre-commit): autofix
  * feat: replace define with enum
  * style(pre-commit): autofix
  * modify: renam a function
  * modify: rename a function
  * modify: fix functions name at the caller side
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Ryuta Kambe <veqcc.c@gmail.com>
* refactor(mrm_handler): delete control_cmd publish function (`#6514 <https://github.com/autowarefoundation/autoware.universe/issues/6514>`_)
  * refactor(mrm_handler): delete control_cmd publish function
* feat(mrm_handler, emergency_handler): remove takeover (`#6522 <https://github.com/autowarefoundation/autoware.universe/issues/6522>`_)
  update(mrm_handler, emergency_handler): remove takeover
* feat(mrm_handler): add mrm_handler (`#6400 <https://github.com/autowarefoundation/autoware.universe/issues/6400>`_)
  * feat: add mrm_handler
  * style(pre-commit): autofix
  * modify: update README
  * feat: refactor isArrivedAtGoal()
  * modify: fix error massages gramatically.
  * feat: update the person in charge of the unimplemented parts
  * modify: fix typo in schema.json
  * modify: fix copyright
  * modify: fix mistakes in README
  * modify: correct a type mistake in README
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Makoto Kurihara <mkuri8m@gmail.com>
* Contributors: Ryuta Kambe, TetsuKawa
