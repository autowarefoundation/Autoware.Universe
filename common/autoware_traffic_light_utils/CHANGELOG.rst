^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_traffic_light_utils
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - common (`#9564 <https://github.com/autowarefoundation/autoware.universe/issues/9564>`_)
* fix: fix package names in changelog files (`#9500 <https://github.com/autowarefoundation/autoware.universe/issues/9500>`_)
* refactor(traffic_light_utils): prefix package and namespace with autoware (`#9251 <https://github.com/autowarefoundation/autoware.universe/issues/9251>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome

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
* fix(traffic_light_utils): fix unusedFunction (`#8605 <https://github.com/autowarefoundation/autoware.universe/issues/8605>`_)
  * fix:unusedFunction
  * fix:unusedFunction
  ---------
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* feat!: replace autoware_auto_msgs with autoware_msgs for common modules (`#7239 <https://github.com/autowarefoundation/autoware.universe/issues/7239>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Ryohsuke Mitsudome, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* chore(perception modules): remove maintainer... (`#6499 <https://github.com/autowarefoundation/autoware.universe/issues/6499>`_)
  * change maintainer
  * add uetake san as maintainer
  ---------
* refactor(tier4_perception_msgs): rename traffic_signal to traffic_light (`#6375 <https://github.com/autowarefoundation/autoware.universe/issues/6375>`_)
  * rename traffic_signal to traffic_light
  * style(pre-commit): autofix
  * fix(crosswalk_traffic_light_estimator): remove unused include, readme
  * rename traffic_signal_array to traffic_light_array
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(avoidance, lane_change): modules handle unknown traffic signal in the same way as red signal (`#6013 <https://github.com/autowarefoundation/autoware.universe/issues/6013>`_)
  * fix(avoidance, lane_change): modules handle unknown traffic signal in the same way as red signal
  * feat(traffic_light_utils): add util functions
  * refactor(bpp): use traffic light utils
  * refactor(bvp): use traffic light utils
  ---------
* feat(crosswalk_traffic_light): add detector and classifier for pedestrian traffic light  (`#5871 <https://github.com/autowarefoundation/autoware.universe/issues/5871>`_)
  * add: crosswalk traffic light recognition
  * fix: set conf=0 when occluded
  * fix: clean code
  * fix: refactor
  * fix: occlusion predictor
  * fix: output not detected signals as unknown
  * Revert "fix: output not detected signals as unknown"
  This reverts commit 7a166596e760d7eb037570e28106dcd105860567.
  * Revert "fix: occlusion predictor"
  This reverts commit 47d8cdd7fee8b4432f7a440f87bc35b50a8bc897.
  * fix: occlusion predictor
  * fix: clean debug code
  * style(pre-commit): autofix
  * fix: launch file
  * fix: set max angle range for different type
  * fix: precommit
  * fix: cancel the judge of flashing for estimated crosswalk traffic light
  * delete: not necessary judgement on label
  * Update perception/traffic_light_classifier/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/include/crosswalk_traffic_light_estimator/node.hpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * style(pre-commit): autofix
  * fix: topic names and message attribute name
  * style(pre-commit): autofix
  * fix: model names
  * style(pre-commit): autofix
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/crosswalk_traffic_light_estimator/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_occlusion_predictor/src/nodelet.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * fix: argument position
  * fix: set classifier type in launch file
  * fix: function and parameter name
  * fix: func name
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * Update perception/traffic_light_map_based_detector/src/node.cpp
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
  * style(pre-commit): autofix
  * fix: move max angle range to config
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * Update launch/tier4_perception_launch/launch/perception.launch.xml
  * fix: model name
  * fix: conflict
  * fix: precommit
  * fix: CI test
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yusuke Muramatsu <yukke42@users.noreply.github.com>
* test(traffic_light_utils): add test_traffic_light_utils (`#4643 <https://github.com/autowarefoundation/autoware.universe/issues/4643>`_)
  * test(traffic_light_utils): add test_traffic_light_utils
  * style(pre-commit): autofix
  * fix(traffic_light_utils): fix magic number
  * style(pre-commit): autofix
  * fix(traffic_light_utils): fix namespace cpplint
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* docs: add readme to perception related utils (`#4265 <https://github.com/autowarefoundation/autoware.universe/issues/4265>`_)
* chore: separate traffic_light_utils from perception_utils (`#4207 <https://github.com/autowarefoundation/autoware.universe/issues/4207>`_)
  * separate traffic_light_utils from perception_utils
  * style(pre-commit): autofix
  * fix namespace bug
  * remove unnecessary dependency
  * rename rest of perception_utils to object_recognition_utils
  * fix bug
  * rename for added radar_object_clustering
  * delete redundant namespace
  * Update common/perception_utils/include/perception_utils/prime_synchronizer.hpp
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  * Correct the failure in the previous merge.
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
* Contributors: Kyoichi Sugahara, Satoshi OTA, Shunsuke Miura, Tao Zhong, beginningfan
