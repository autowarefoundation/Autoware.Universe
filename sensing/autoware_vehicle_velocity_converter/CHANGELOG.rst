^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package vehicle_velocity_converter
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
* Merge branch 'main' into release-0.40.0
* Revert "chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)"
  This reverts commit c9f0f2688c57b0f657f5c1f28f036a970682e7f5.
* chore(package.xml): bump version to 0.39.0 (`#9587 <https://github.com/autowarefoundation/autoware.universe/issues/9587>`_)
  * chore(package.xml): bump version to 0.39.0
  * fix: fix ticket links in CHANGELOG.rst
  * fix: remove unnecessary diff
  ---------
  Co-authored-by: Yutaka Kondo <yutaka.kondo@youtalk.jp>
* refactor(vehicle_velocity_converter)!: prefix package and namespace with autoware (`#8967 <https://github.com/autowarefoundation/autoware.universe/issues/8967>`_)
  * add autoware prefix
  * fix conflict
  ---------
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* Contributors: Fumiya Watanabe, Masaki Baba, Ryohsuke Mitsudome

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
* refactor(vehicle_velocity_converter): apply static analysis (`#7975 <https://github.com/autowarefoundation/autoware.universe/issues/7975>`_)
  refactor based on linter
* feat(vehicle_velocity_converter): componentize VehicleVelocityConverter (`#7116 <https://github.com/autowarefoundation/autoware.universe/issues/7116>`_)
  * remove unusing main func file
  * mod to componentize and use glog
  * change log output from screen to both
  ---------
  Co-authored-by: Yamato Ando <yamato.ando@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for sensing modules (`#7247 <https://github.com/autowarefoundation/autoware.universe/issues/7247>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Masaki Baba, Ryohsuke Mitsudome, Yutaka Kondo

0.26.0 (2024-04-03)
-------------------
* refactor(sensing-vehicle-velocity-converter): rework parameters (`#5609 <https://github.com/autowarefoundation/autoware.universe/issues/5609>`_)
  * sensing-vehicle-velocity-converter-module
  * sensing-vehicle-velocity-converter
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat(vehicle_velocity_converter): add `speed_scale_factor` (`#2641 <https://github.com/autowarefoundation/autoware.universe/issues/2641>`_)
  * feat(vehicle_velocity_converter): add speed_scale_factor
  * add parameter description in readme
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(vehicle_velocity_converter): simpify parameter inputs (`#1727 <https://github.com/autowarefoundation/autoware.universe/issues/1727>`_)
  * fix(vehicle_velocity_converter): simpify parameter inputs
  * fix readme and default value
  * fix default value
* feat(distortion_corrector): use gyroscope for correcting LiDAR distortion (`#1120 <https://github.com/autowarefoundation/autoware.universe/issues/1120>`_)
  * first commit
  * ci(pre-commit): autofix
  * check if angular_velocity_queue\_ is empty or not
  * move vehicle velocity converter to sensing
  * ci(pre-commit): autofix
  * fix
  * ci(pre-commit): autofix
  * reflected reviews
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Vincent Richard, karishma1911, kminoda
