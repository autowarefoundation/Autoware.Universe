^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package localization_evaluator
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* refactor(evaluator/localization_evaluator): rework parameters  (`#6744 <https://github.com/autowarefoundation/autoware.universe/issues/6744>`_)
  * add param file and schema
  * style(pre-commit): autofix
  * .
  * .
  * .
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* ci(pre-commit): autoupdate (`#7499 <https://github.com/autowarefoundation/autoware.universe/issues/7499>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* feat!: replace autoware_auto_msgs with autoware_msgs for evaluator modules (`#7241 <https://github.com/autowarefoundation/autoware.universe/issues/7241>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Ryohsuke Mitsudome, Takayuki Murooka, Yutaka Kondo, awf-autoware-bot[bot], oguzkaganozt

0.26.0 (2024-04-03)
-------------------
* chore(build): remove tier4_autoware_utils.hpp evaluator/ simulator/ (`#4839 <https://github.com/autowarefoundation/autoware.universe/issues/4839>`_)
* chore: add maintainer (`#4234 <https://github.com/autowarefoundation/autoware.universe/issues/4234>`_)
  * chore: add maintainer
  * Update evaluator/localization_evaluator/package.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
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
* feat(metrics_calculation): add kinematic and localization evaluators with metrics (`#928 <https://github.com/autowarefoundation/autoware.universe/issues/928>`_)
  * initial skeleton of localization evaluator
  * Add simple localization evaliuation framework
  * Clean and add tests
  * ci(pre-commit): autofix
  * Clean localization evaluator
  * Update kinematic evaluator
  * ci(pre-commit): autofix
  * dependency fix
  * Fix localization evaluator tests
  * ci(pre-commit): autofix
  * Add missing includes and remove unnecessary quotes
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Wojciech Jaworski <wojciech.jaworski@robotec.ai>
* Contributors: Kenji Miyake, Mamoru Sobue, Satoshi OTA, Vincent Richard, djargot
