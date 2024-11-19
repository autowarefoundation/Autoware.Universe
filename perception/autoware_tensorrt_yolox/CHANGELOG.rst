^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_tensorrt_yolox
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
