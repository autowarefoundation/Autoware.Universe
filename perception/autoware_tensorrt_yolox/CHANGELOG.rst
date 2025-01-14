^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_tensorrt_yolox
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_tensorrt_yolox): fix clang-diagnostic-inconsistent-missing-override (`#9512 <https://github.com/autowarefoundation/autoware.universe/issues/9512>`_)
  fix: clang-diagnostic-inconsistent-missing-override
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(cuda_utils): prefix package and namespace with autoware (`#9171 <https://github.com/autowarefoundation/autoware.universe/issues/9171>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Yutaka Kondo, kobayu858

0.39.0 (2024-11-25)
-------------------
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* refactor(cuda_utils): prefix package and namespace with autoware (`#9171 <https://github.com/autowarefoundation/autoware.universe/issues/9171>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

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
* refactor(object_recognition_utils): add autoware prefix to object_recognition_utils (`#8946 <https://github.com/autowarefoundation/autoware.universe/issues/8946>`_)
* feat(autoware_tensorrt_yolox): add GPU - CUDA device option (`#8245 <https://github.com/autowarefoundation/autoware.universe/issues/8245>`_)
  * init CUDA device option
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore(autoware_tensorrt_yolox): add Kotaro Uetake as maintainer (`#8595 <https://github.com/autowarefoundation/autoware.universe/issues/8595>`_)
  chore: add Kotaro Uetake as maintainer
* fix: cpp17 namespaces (`#8526 <https://github.com/autowarefoundation/autoware.universe/issues/8526>`_)
  Use traditional-style nameplace nesting for nvcc
  Co-authored-by: Yuri Guimaraes <yuri.kgpps@gmail.com>
* fix(docs): fix docs for tensorrt yolox (`#8304 <https://github.com/autowarefoundation/autoware.universe/issues/8304>`_)
  fix docs for tensorrt yolox
* refactor(tensorrt_yolox): move utils into perception_utils (`#8435 <https://github.com/autowarefoundation/autoware.universe/issues/8435>`_)
  * chore(tensorrt_yolo): refactor utils
  * style(pre-commit): autofix
  * fix: tensorrt_yolox
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_tensorrt_yolox): fix variableScope (`#8430 <https://github.com/autowarefoundation/autoware.universe/issues/8430>`_)
  fix: variableScope
  Co-authored-by: kobayu858 <129580202+kobayu858@users.noreply.github.com>
* fix(tensorrt_yolox): add run length encoding for sematic segmentation mask (`#7905 <https://github.com/autowarefoundation/autoware.universe/issues/7905>`_)
  * fix: add rle compress
  * fix: rle compress
  * fix: move rle into utils
  * chore: pre-commit
  * Update perception/autoware_tensorrt_yolox/src/utils.cpp
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix: remove unused variable
  * Update perception/autoware_tensorrt_yolox/src/utils.cpp
  Co-authored-by: Manato Hirabayashi <3022416+manato@users.noreply.github.com>
  * style(pre-commit): autofix
  * feat: add unit test for utils
  * style(pre-commit): autofix
  * fix: unit test
  * chore: change to explicit index
  * style(pre-commit): autofix
  * fix: cuda cmake
  * fix: separate unit test into different PR
  ---------
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  Co-authored-by: Manato Hirabayashi <3022416+manato@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(autoware_tensorrt_yolox): fix unreadVariable (`#8356 <https://github.com/autowarefoundation/autoware.universe/issues/8356>`_)
  * fix:unreadVariable
  * fix:unreadVariable
  ---------
* refactor: image transport decompressor/autoware prefix (`#8197 <https://github.com/autowarefoundation/autoware.universe/issues/8197>`_)
  * refactor: add `autoware` namespace prefix to image_transport_decompressor
  * refactor(image_transport_decompressor): add `autoware` prefix to the package code
  * refactor: update package name in CODEOWNER
  * fix: merge main into the branch
  * refactor: update packages which depend on image_transport_decompressor
  * refactor(image_transport_decompressor): update README
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* refactor(tensorrt_yolox)!: fix namespace and directory structure (`#7992 <https://github.com/autowarefoundation/autoware.universe/issues/7992>`_)
  * refactor: add autoware namespace prefix to `tensorrt_yolox`
  * refactor: apply `autoware` namespace to tensorrt_yolox
  * chore: update CODEOWNERS
  * fix: resolve `yolox_tiny` to work
  ---------
* Contributors: Abraham Monrroy Cano, Amadeusz Szymko, Esteve Fernandez, Ismet Atabay, Kotaro Uetake, Manato Hirabayashi, Nagi70, Yutaka Kondo, Yuxuan Liu, badai nguyen, kobayu858

0.26.0 (2024-04-03)
-------------------
