^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_traffic_light_fine_detector
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - perception (`#9569 <https://github.com/autowarefoundation/autoware.universe/issues/9569>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(autoware_traffic_light*): add maintainer (`#9280 <https://github.com/autowarefoundation/autoware.universe/issues/9280>`_)
  * add fundamental commit
  * add forgot package
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Masato Saeki, Ryohsuke Mitsudome, Yutaka Kondo

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(autoware_traffic_light*): add maintainer (`#9280 <https://github.com/autowarefoundation/autoware.universe/issues/9280>`_)
  * add fundamental commit
  * add forgot package
  ---------
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Masato Saeki, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace (`#9099 <https://github.com/autowarefoundation/autoware.universe/issues/9099>`_)
  * refactor(tensorrt_common)!: fix namespace, directory structure & move to perception namespace
  * refactor(tensorrt_common): directory structure
  * style(pre-commit): autofix
  * fix(tensorrt_common): correct package name for logging
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenzo Lobos Tsunekawa <kenzo.lobos@tier4.jp>
* feat(autoware_tensorrt_yolox): add GPU - CUDA device option (`#8245 <https://github.com/autowarefoundation/autoware.universe/issues/8245>`_)
  * init CUDA device option
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_traffic_light_fine_detector): fix unusedFunction (`#8583 <https://github.com/autowarefoundation/autoware.universe/issues/8583>`_)
  fix:unusedFunction
* fix(autoware_traffic_light_fine_detector): fix passedByValue (`#8237 <https://github.com/autowarefoundation/autoware.universe/issues/8237>`_)
  fix:passedByValue
* fix(traffic_light_fine_detector): fix funcArgNamesDifferent (`#8154 <https://github.com/autowarefoundation/autoware.universe/issues/8154>`_)
  fix:funcArgNamesDifferent
* fix(autoware_traffic_light_fine_detector): fix constParameterReference (`#8146 <https://github.com/autowarefoundation/autoware.universe/issues/8146>`_)
  * fix:constParameterReference
  * fix:constParameterReference
  * fix:constParameterReference
  ---------
* refactor(tensorrt_yolox)!: fix namespace and directory structure (`#7992 <https://github.com/autowarefoundation/autoware.universe/issues/7992>`_)
  * refactor: add autoware namespace prefix to `tensorrt_yolox`
  * refactor: apply `autoware` namespace to tensorrt_yolox
  * chore: update CODEOWNERS
  * fix: resolve `yolox_tiny` to work
  ---------
* refactor(traffic_light\_*)!: add package name prefix of autoware\_ (`#8159 <https://github.com/autowarefoundation/autoware.universe/issues/8159>`_)
  * chore: rename traffic_light_fine_detector to autoware_traffic_light_fine_detector
  * chore: rename traffic_light_multi_camera_fusion to autoware_traffic_light_multi_camera_fusion
  * chore: rename traffic_light_occlusion_predictor to autoware_traffic_light_occlusion_predictor
  * chore: rename traffic_light_classifier to autoware_traffic_light_classifier
  * chore: rename traffic_light_map_based_detector to autoware_traffic_light_map_based_detector
  * chore: rename traffic_light_visualization to autoware_traffic_light_visualization
  ---------
* Contributors: Amadeusz Szymko, Ismet Atabay, Kotaro Uetake, Taekjin LEE, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
