^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_map_based_prediction
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(autoware_map_based_prediction): msg namespace (`#9553 <https://github.com/autowarefoundation/autoware.universe/issues/9553>`_)
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
* refactor(map_based_prediction): move member functions to utils (`#9225 <https://github.com/autowarefoundation/autoware.universe/issues/9225>`_)
* refactor(map_based_prediction): divide objectsCallback (`#9219 <https://github.com/autowarefoundation/autoware.universe/issues/9219>`_)
* refactor(autoware_map_based_prediction): split pedestrian and bicycle predictor (`#9201 <https://github.com/autowarefoundation/autoware.universe/issues/9201>`_)
  * refactor: grouping functions
  * refactor: grouping parameters
  * refactor: rename member road_users_history to road_users_history\_
  * refactor: separate util functions
  * refactor: Add predictor_vru.cpp and utils.cpp to map_based_prediction_node
  * refactor: Add explicit template instantiation for removeOldObjectsHistory function
  * refactor: Add tf2_geometry_msgs to data_structure
  * refactor: Remove unused variables and functions in map_based_prediction_node.cpp
  * Update perception/autoware_map_based_prediction/include/map_based_prediction/predictor_vru.hpp
  * Apply suggestions from code review
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Amadeusz Szymko, Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Mamoru Sobue, Ryohsuke Mitsudome, Taekjin LEE, Yutaka Kondo

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
* refactor(map_based_prediction): move member functions to utils (`#9225 <https://github.com/autowarefoundation/autoware.universe/issues/9225>`_)
* refactor(map_based_prediction): divide objectsCallback (`#9219 <https://github.com/autowarefoundation/autoware.universe/issues/9219>`_)
* refactor(autoware_map_based_prediction): split pedestrian and bicycle predictor (`#9201 <https://github.com/autowarefoundation/autoware.universe/issues/9201>`_)
  * refactor: grouping functions
  * refactor: grouping parameters
  * refactor: rename member road_users_history to road_users_history\_
  * refactor: separate util functions
  * refactor: Add predictor_vru.cpp and utils.cpp to map_based_prediction_node
  * refactor: Add explicit template instantiation for removeOldObjectsHistory function
  * refactor: Add tf2_geometry_msgs to data_structure
  * refactor: Remove unused variables and functions in map_based_prediction_node.cpp
  * Update perception/autoware_map_based_prediction/include/map_based_prediction/predictor_vru.hpp
  * Apply suggestions from code review
  * style(pre-commit): autofix
  ---------
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* Contributors: Esteve Fernandez, Mamoru Sobue, Taekjin LEE, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(autoware_map_based_prediction): refactoring lanelet path prediction and pose path conversion (`#9104 <https://github.com/autowarefoundation/autoware.universe/issues/9104>`_)
  * refactor: update predictObjectManeuver function parameters
  * refactor: update hash function for LaneletPath in map_based_prediction_node.hpp
  * refactor: path list rename
  * refactor: take the path conversion out of the lanelet prediction
  * refactor: lanelet possible paths
  * refactor: separate converter of lanelet path to pose path
  * refactor: block each path lanelet process
  * refactor: fix time keeper
  * Update perception/autoware_map_based_prediction/src/map_based_prediction_node.cpp
  ---------
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
* chore(autoware_map_based_prediction): add maintainers to package.xml (`#9125 <https://github.com/autowarefoundation/autoware.universe/issues/9125>`_)
  chore: add maintainers to package.xml
  The package.xml file was updated to include additional maintainers' email addresses.
* fix(autoware_map_based_prediction): adjust lateral duration when object is behind reference path (`#8973 <https://github.com/autowarefoundation/autoware.universe/issues/8973>`_)
  fix: adjust lateral duration when object is behind reference path
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(autoware_map_based_prediction): improve frenet path generation (`#8811 <https://github.com/autowarefoundation/autoware.universe/issues/8811>`_)
  * feat: calculate terminal d position based on playable width in path_generator.cpp
  * feat: Add width parameter path generations
  refactor(path_generator): improve backlash width calculation
  refactor(path_generator): improve backlash width calculation
  * fix: set initial point of Frenet Path to Cartesian Path conversion
  refactor: limit the d value to the radius for curved reference paths
  refactor: limit d value to curve limit for curved reference paths
  refactor: extend base_path_s with extrapolated base_path_x, base_path_y, base_path_z if min_s is negative
  refactor: linear path when object is moving backward
  feat: Update getFrenetPoint function to include target_path parameter
  The getFrenetPoint function in path_generator.hpp and path_generator.cpp has been updated to include a new parameter called target_path. This parameter is used to trim the reference path based on the starting segment index, allowing for more accurate calculations.
  * feat: Add interpolationLerp function for linear interpolation
  * Update starting_segment_idx type in getFrenetPoint function
  refactor: Update starting_segment_idx type in getFrenetPoint function
  refactor: Update getFrenetPoint function to include target_path parameter
  refactor: exclude target path determination logic from getFrenetPoint
  refactor: Add interpolationLerp function for quaternion linear interpolation
  refactor: remove redundant yaw height update
  refactor: Update path_generator.cpp to include object height in predicted_pose
  fix: comment out optimum target searcher
  * feat: implement a new optimization of target ref path search
  refactor: Update path_generator.cpp to include object height in predicted_pose
  refactor: measure performance
  refactor: remove comment-outs, measure times
  style(pre-commit): autofix
  refactor: move starting point search function to getPredictedReferencePath
  refactor: target segment index search parameter adjust
  * fix: replace nearest search to custom one for efficiency
  feat: Update CLOSE_LANELET_THRESHOLD and CLOSE_PATH_THRESHOLD values
  * refactor: getFrenetPoint blocks
  * chore: add comments
  * feat: Trim reference paths if optimum position is not found
  style(pre-commit): autofix
  chore: remove comment
  * fix: shadowVariable of time keeper pointers
  * refactor: improve backlash width calculation, parameter adjustment
  * fix: cylinder type object do not have y dimension, use x dimension
  * chore: add comment to explain an internal parameter 'margin'
  * chore: add comment of backlash calculation shortcut
  * chore: Improve readability of backlash to target shift model
  * feat: set the return width by the path width
  * refactor: separate a logic to searchProperStartingRefPathIndex function
  * refactor: search starting ref path using optional for return type
  * fix: object orientation calculation is added to the predicted path generation
  * chore: fix spell-check
  ---------
* revert(autoware_map_based_prediction): revert improve frenet path gen (`#8808 <https://github.com/autowarefoundation/autoware.universe/issues/8808>`_)
  Revert "feat(autoware_map_based_prediction): improve frenet path generation (`#8602 <https://github.com/autowarefoundation/autoware.universe/issues/8602>`_)"
  This reverts commit 67265bbd60c85282c1c3cf65e603098e0c30c477.
* feat(autoware_map_based_prediction): improve frenet path generation (`#8602 <https://github.com/autowarefoundation/autoware.universe/issues/8602>`_)
  * feat: calculate terminal d position based on playable width in path_generator.cpp
  * feat: Add width parameter path generations
  refactor(path_generator): improve backlash width calculation
  refactor(path_generator): improve backlash width calculation
  * fix: set initial point of Frenet Path to Cartesian Path conversion
  refactor: limit the d value to the radius for curved reference paths
  refactor: limit d value to curve limit for curved reference paths
  refactor: extend base_path_s with extrapolated base_path_x, base_path_y, base_path_z if min_s is negative
  refactor: linear path when object is moving backward
  feat: Update getFrenetPoint function to include target_path parameter
  The getFrenetPoint function in path_generator.hpp and path_generator.cpp has been updated to include a new parameter called target_path. This parameter is used to trim the reference path based on the starting segment index, allowing for more accurate calculations.
  * feat: Add interpolationLerp function for linear interpolation
  * Update starting_segment_idx type in getFrenetPoint function
  refactor: Update starting_segment_idx type in getFrenetPoint function
  refactor: Update getFrenetPoint function to include target_path parameter
  refactor: exclude target path determination logic from getFrenetPoint
  refactor: Add interpolationLerp function for quaternion linear interpolation
  refactor: remove redundant yaw height update
  refactor: Update path_generator.cpp to include object height in predicted_pose
  fix: comment out optimum target searcher
  * feat: implement a new optimization of target ref path search
  refactor: Update path_generator.cpp to include object height in predicted_pose
  refactor: measure performance
  refactor: remove comment-outs, measure times
  style(pre-commit): autofix
  refactor: move starting point search function to getPredictedReferencePath
  refactor: target segment index search parameter adjust
  * fix: replace nearest search to custom one for efficiency
  feat: Update CLOSE_LANELET_THRESHOLD and CLOSE_PATH_THRESHOLD values
  * refactor: getFrenetPoint blocks
  * chore: add comments
  * feat: Trim reference paths if optimum position is not found
  style(pre-commit): autofix
  chore: remove comment
  * fix: shadowVariable of time keeper pointers
  * refactor: improve backlash width calculation, parameter adjustment
  * fix: cylinder type object do not have y dimension, use x dimension
  * chore: add comment to explain an internal parameter 'margin'
  * chore: add comment of backlash calculation shortcut
  * chore: Improve readability of backlash to target shift model
  * feat: set the return width by the path width
  * refactor: separate a logic to searchProperStartingRefPathIndex function
  * refactor: search starting ref path using optional for return type
  ---------
* perf(autoware_map_based_prediction): replace pow (`#8751 <https://github.com/autowarefoundation/autoware.universe/issues/8751>`_)
* fix(autoware_map_based_prediction): output from screen to both (`#8408 <https://github.com/autowarefoundation/autoware.universe/issues/8408>`_)
* perf(autoware_map_based_prediction): removed duplicate findNearest calculations (`#8490 <https://github.com/autowarefoundation/autoware.universe/issues/8490>`_)
* perf(autoware_map_based_prediction): enhance speed by removing unnecessary calculation (`#8471 <https://github.com/autowarefoundation/autoware.universe/issues/8471>`_)
  * fix(autoware_map_based_prediction): use surrounding_crosswalks instead of external_surrounding_crosswalks
  * perf(autoware_map_based_prediction): enhance speed by removing unnecessary calculation
  ---------
* refactor(autoware_map_based_prediction): map based pred time keeper ptr (`#8462 <https://github.com/autowarefoundation/autoware.universe/issues/8462>`_)
  * refactor(map_based_prediction): implement time keeper by pointer
  * feat(map_based_prediction): set time keeper in path generator
  * feat: use scoped time track only when the timekeeper ptr is not null
  * refactor: define publish function to measure time
  * chore: add debug parameters for map-based prediction
  * chore: remove unnecessary ScopedTimeTrack instances
  * feat: replace member to pointer
  ---------
* fix(autoware_map_based_prediction): use surrounding_crosswalks instead of external_surrounding_crosswalks (`#8467 <https://github.com/autowarefoundation/autoware.universe/issues/8467>`_)
* perf(autoware_map_based_prediction): speed up map based prediction by using lru cache in convertPathType (`#8461 <https://github.com/autowarefoundation/autoware.universe/issues/8461>`_)
  feat(autoware_map_based_prediction): speed up map based prediction by using lru cache in convertPathType
* perf(map_based_prediction): improve world to map transform calculation (`#8413 <https://github.com/autowarefoundation/autoware.universe/issues/8413>`_)
  * perf(map_based_prediction): improve world to map transform calculation
  1. remove unused transforms
  2. make transform loading late as possible
  * perf(map_based_prediction): get transform only when it is necessary
  ---------
* perf(autoware_map_based_prediction): improve orientation calculation and resample converted path (`#8427 <https://github.com/autowarefoundation/autoware.universe/issues/8427>`_)
  * refactor: improve orientation calculation and resample converted path with linear interpolation
  Simplify the calculation of the orientation for each pose in the convertPathType function by directly calculating the sine and cosine of half the yaw angle. This improves efficiency and readability. Also, improve the resampling of the converted path by using linear interpolation for better performance.
  * Update perception/autoware_map_based_prediction/src/map_based_prediction_node.cpp
  Co-authored-by: Kotaro Uetake <60615504+ktro2828@users.noreply.github.com>
  * Update perception/autoware_map_based_prediction/src/map_based_prediction_node.cpp
  Co-authored-by: Kotaro Uetake <60615504+ktro2828@users.noreply.github.com>
  ---------
  Co-authored-by: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  Co-authored-by: Kotaro Uetake <60615504+ktro2828@users.noreply.github.com>
* perf(map_based_prediction): apply lerp instead of spline (`#8416 <https://github.com/autowarefoundation/autoware.universe/issues/8416>`_)
  perf: apply lerp interpolation instead of spline
* revert (map_based_prediction): use linear interpolation for path conversion (`#8400 <https://github.com/autowarefoundation/autoware.universe/issues/8400>`_)" (`#8417 <https://github.com/autowarefoundation/autoware.universe/issues/8417>`_)
  Revert "perf(map_based_prediction): use linear interpolation for path conversion (`#8400 <https://github.com/autowarefoundation/autoware.universe/issues/8400>`_)"
  This reverts commit 147403f1765346be9c5a3273552d86133298a899.
* perf(map_based_prediction): use linear interpolation for path conversion (`#8400 <https://github.com/autowarefoundation/autoware.universe/issues/8400>`_)
  * refactor: improve orientation calculation in MapBasedPredictionNode
  Simplify the calculation of the orientation for each pose in the convertPathType function. Instead of using the atan2 function, calculate the sine and cosine of half the yaw angle directly. This improves the efficiency and readability of the code.
  * refactor: resample converted path with linear interpolation
  Improve the resampling of the converted path in the convertPathType function. Using linear interpolation for performance improvement.
  the mark indicates true, but the function resamplePoseVector implementation is opposite.
  chore: write comment about use_akima_slpine_for_xy
  ---------
* perf(map_based_prediction): create a fence LineString layer and use rtree query (`#8406 <https://github.com/autowarefoundation/autoware.universe/issues/8406>`_)
  use fence layer
* perf(map_based_prediction): remove unncessary withinRoadLanelet() (`#8403 <https://github.com/autowarefoundation/autoware.universe/issues/8403>`_)
* feat(map_based_prediction): filter surrounding crosswalks for pedestrians beforehand (`#8388 <https://github.com/autowarefoundation/autoware.universe/issues/8388>`_)
  fix withinAnyCroswalk
* fix(autoware_map_based_prediction): fix argument order (`#8031 <https://github.com/autowarefoundation/autoware.universe/issues/8031>`_)
  fix(autoware_map_based_prediction): fix argument order in call `getFrenetPoint()`
  Co-authored-by: Shintaro Tomie <58775300+Shin-kyoto@users.noreply.github.com>
  Co-authored-by: Kotaro Uetake <60615504+ktro2828@users.noreply.github.com>
* feat(map_based_prediction): add time_keeper (`#8176 <https://github.com/autowarefoundation/autoware.universe/issues/8176>`_)
* fix(autoware_map_based_prediction): fix shadowVariable (`#7934 <https://github.com/autowarefoundation/autoware.universe/issues/7934>`_)
  fix:shadowVariable
* perf(map_based_prediction): remove query on all fences linestrings (`#7237 <https://github.com/autowarefoundation/autoware.universe/issues/7237>`_)
* fix(autoware_map_based_prediction): fix syntaxError (`#7813 <https://github.com/autowarefoundation/autoware.universe/issues/7813>`_)
  * fix(autoware_map_based_prediction): fix syntaxError
  * style(pre-commit): autofix
  * fix spellcheck
  * fix new cppcheck warnings
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat(map based prediction): use polling subscriber (`#7397 <https://github.com/autowarefoundation/autoware.universe/issues/7397>`_)
  feat(map_based_prediction): use polling subscriber
* refactor(map_based_prediction): prefix map based prediction (`#7391 <https://github.com/autowarefoundation/autoware.universe/issues/7391>`_)
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Kotaro Uetake, Mamoru Sobue, Maxime CLEMENT, Onur Can Yücedağ, Ryuta Kambe, Taekjin LEE, Takamasa Horibe, Takayuki Murooka, Yukinari Hisaki, Yutaka Kondo, kminoda, kobayu858

0.26.0 (2024-04-03)
-------------------
