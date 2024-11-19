^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_traffic_light_classifier
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(traffic_light_classifier): fix traffic light monitor warning (`#8412 <https://github.com/autowarefoundation/autoware.universe/issues/8412>`_)
  fix traffic light monitor warning
* fix(autoware_traffic_light_classifier): fix passedByValue (`#8392 <https://github.com/autowarefoundation/autoware.universe/issues/8392>`_)
  fix:passedByValue
* fix(traffic_light_classifier): fix zero size roi bug (`#7608 <https://github.com/autowarefoundation/autoware.universe/issues/7608>`_)
  * fix: continue to process when input roi size is zero
  * fix: consider when roi size is zero, rois is empty
  fix
  * fix: use emplace_back instead of push_back for adding images and backlight indices
  The code changes in `traffic_light_classifier_node.cpp` modify the way images and backlight indices are added to the respective vectors. Instead of using `push_back`, the code now uses `emplace_back`. This change improves performance and ensures proper object construction.
  * refactor: bring back for loop skim and output_msg filling
  * chore: refactor code to handle empty input ROIs in traffic_light_classifier_node.cpp
  * refactor: using index instead of vector length
  ---------
* fix(traffic_light_classifier): fix funcArgNamesDifferent (`#8153 <https://github.com/autowarefoundation/autoware.universe/issues/8153>`_)
  * fix:funcArgNamesDifferent
  * fix:clang format
  ---------
* refactor(traffic_light\_*)!: add package name prefix of autoware\_ (`#8159 <https://github.com/autowarefoundation/autoware.universe/issues/8159>`_)
  * chore: rename traffic_light_fine_detector to autoware_traffic_light_fine_detector
  * chore: rename traffic_light_multi_camera_fusion to autoware_traffic_light_multi_camera_fusion
  * chore: rename traffic_light_occlusion_predictor to autoware_traffic_light_occlusion_predictor
  * chore: rename traffic_light_classifier to autoware_traffic_light_classifier
  * chore: rename traffic_light_map_based_detector to autoware_traffic_light_map_based_detector
  * chore: rename traffic_light_visualization to autoware_traffic_light_visualization
  ---------
* Contributors: Amadeusz Szymko, Sho Iwasawa, Taekjin LEE, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
