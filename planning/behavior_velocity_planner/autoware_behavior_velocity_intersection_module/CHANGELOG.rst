^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_behavior_velocity_intersection_module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - planning (`#9570 <https://github.com/autowarefoundation/autoware.universe/issues/9570>`_)
* feat(behavior_velocity_planner)!: remove stop_reason (`#9452 <https://github.com/autowarefoundation/autoware.universe/issues/9452>`_)
* refactor: correct spelling (`#9528 <https://github.com/autowarefoundation/autoware.universe/issues/9528>`_)
* 0.39.0
* update changelog
* Merge commit '6a1ddbd08bd' into release-0.39.0
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix(autoware_behavior_velocity_intersection_module): fix clang-diagnostic-unused-parameter (`#9409 <https://github.com/autowarefoundation/autoware.universe/issues/9409>`_)
  fix: clang-diagnostic-unused-parameter
* fix(autoware_behavior_velocity_intersection_module): fix clang-diagnostic-unused-lambda-capture (`#9407 <https://github.com/autowarefoundation/autoware.universe/issues/9407>`_)
  fix: clang-diagnostic-unused-parameter
* chore(autoware_behavior_velocity_intersection_module): include opencv as system (`#9330 <https://github.com/autowarefoundation/autoware.universe/issues/9330>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* fix(bvp): remove expired module safely (`#9212 <https://github.com/autowarefoundation/autoware.universe/issues/9212>`_)
  * fix(bvp): remove expired module safely
  * fix: remove module id set
  * fix: use itr to erase expired module
  * fix: remove unused function
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Mamoru Sobue, Ryohsuke Mitsudome, Satoshi OTA, Yukinari Hisaki, Yutaka Kondo, kobayu858

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
* fix(bvp): remove expired module safely (`#9212 <https://github.com/autowarefoundation/autoware.universe/issues/9212>`_)
  * fix(bvp): remove expired module safely
  * fix: remove module id set
  * fix: use itr to erase expired module
  * fix: remove unused function
  ---------
* Contributors: Esteve Fernandez, Satoshi OTA, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* chore(intersection): print RTC status in diagnostic debug message (`#9007 <https://github.com/autowarefoundation/autoware.universe/issues/9007>`_)
  debug(intersection): print RTC status in diagnostic message
* fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID (`#9103 <https://github.com/autowarefoundation/autoware.universe/issues/9103>`_)
  * fix(behavior_path_planner, behavior_velocity_planner): fix to not read invalid ID
  * style(pre-commit): autofix
  * fix typo
  * fix(behavior_path_planner, behavior_velocity_planner): fix typo and indentation
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(intersection): handle pass judge after red/arrow-signal to ignore NPCs after the signal changed to green again (`#9119 <https://github.com/autowarefoundation/autoware.universe/issues/9119>`_)
* fix(intersection): set RTC enable (`#9040 <https://github.com/autowarefoundation/autoware.universe/issues/9040>`_)
  set rtc enable
* fix(interpolation): fix bug of interpolation (`#8969 <https://github.com/autowarefoundation/autoware.universe/issues/8969>`_)
  fix bug of interpolation
* refactor(autoware_interpolation): prefix package and namespace with autoware (`#8088 <https://github.com/autowarefoundation/autoware.universe/issues/8088>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* fix(intersection): fix typo (`#8911 <https://github.com/autowarefoundation/autoware.universe/issues/8911>`_)
  * fix(intersection): fix typo
  * fix(intersection): fix typo
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(bvp): fix rtc state update logic (`#8884 <https://github.com/autowarefoundation/autoware.universe/issues/8884>`_)
  * fix(bvp): fix rtc state update logic
  * fix(intersection): fix unexpected rtc state initialization
  ---------
* fix(autoware_behavior_velocity_intersection_module): fix unusedFunction (`#8666 <https://github.com/autowarefoundation/autoware.universe/issues/8666>`_)
  * fix:unusedFunction
  * fix:unusedFunction
  ---------
* fix(autoware_behavior_velocity_intersection_module): fix unreadVariable (`#8836 <https://github.com/autowarefoundation/autoware.universe/issues/8836>`_)
  fix:unreadVariable
* fix(autoware_behavior_velocity_intersection_module): fix virtualCallInConstructor (`#8835 <https://github.com/autowarefoundation/autoware.universe/issues/8835>`_)
  fix:virtualCallInConstructor
* fix(behavior_velocity_planner): align the parameters with launcher (`#8791 <https://github.com/autowarefoundation/autoware.universe/issues/8791>`_)
  parameters in behavior_velocity_planner aligned
* fix(intersection): additional fix for 8520 (`#8561 <https://github.com/autowarefoundation/autoware.universe/issues/8561>`_)
* feat(intersection): fix topological sort for complicated intersection (`#8520 <https://github.com/autowarefoundation/autoware.universe/issues/8520>`_)
  * for enclave occlusion detection lanelet
  * some refactorings and modify doxygen
  * fix ci
  ---------
  Co-authored-by: Y.Hisaki <yhisaki31@gmail.com>
* fix(behavior_velocity_planner): fix cppcheck warnings of virtualCallInConstructor (`#8376 <https://github.com/autowarefoundation/autoware.universe/issues/8376>`_)
  Co-authored-by: Ryuta Kambe <ryuta.kambe@tier4.jp>
* feat(intersection): add test map for intersection (`#8455 <https://github.com/autowarefoundation/autoware.universe/issues/8455>`_)
* fix(autoware_smart_mpc_trajectory_follower): fix unusedStructMember (`#8393 <https://github.com/autowarefoundation/autoware.universe/issues/8393>`_)
  * fix:unusedStructMember
  * fix:unusedStructMember
  * fix:clang format
  ---------
* fix(autoware_behavior_velocity_intersection_module): fix functionConst (`#8283 <https://github.com/autowarefoundation/autoware.universe/issues/8283>`_)
  fix:functionConst
* fix(autoware_behavior_velocity_intersection_module): fix funcArgNamesDifferent (`#8023 <https://github.com/autowarefoundation/autoware.universe/issues/8023>`_)
  * fix:funcArgNamesDifferent
  * fix:funcArgNamesDifferent
  * refactor:clang format
  * fix:funcArgNamesDifferent
  ---------
* refactor(probabilistic_occupancy_grid_map, occupancy_grid_map_outlier_filter): add autoware\_ prefix to package name (`#8183 <https://github.com/autowarefoundation/autoware.universe/issues/8183>`_)
  * chore: fix package name probabilistic occupancy grid map
  * fix: solve launch error
  * chore: update occupancy_grid_map_outlier_filter
  * style(pre-commit): autofix
  * refactor: update package name to autoware_probabilistic_occupancy_grid_map on a test
  * refactor: rename folder of occupancy_grid_map_outlier_filter
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Taekjin LEE <taekjin.lee@tier4.jp>
* fix(autoware_behavior_velocity_intersection_module): fix shadowVariable (`#7976 <https://github.com/autowarefoundation/autoware.universe/issues/7976>`_)
* fix(autoware_behavior_velocity_intersection_module): fix shadowFunction (`#7835 <https://github.com/autowarefoundation/autoware.universe/issues/7835>`_)
  * fix(autoware_behavior_velocity_intersection_module): fix shadowFunction
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: add `autoware\_` prefix to `lanelet2_extension` (`#7640 <https://github.com/autowarefoundation/autoware.universe/issues/7640>`_)
* refactor(behavior_velocity_intersection): apply clang-tidy check (`#7552 <https://github.com/autowarefoundation/autoware.universe/issues/7552>`_)
  intersection
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* chore(behavior_velocity_planner): move packages (`#7526 <https://github.com/autowarefoundation/autoware.universe/issues/7526>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Go Sakayori, Kosuke Takeuchi, Mamoru Sobue, Ryuta Kambe, Satoshi OTA, T-Kimura-MM, Takayuki Murooka, Yoshi Ri, Yukinari Hisaki, Yutaka Kondo, Zhe Shen, kobayu858, taisa1

0.26.0 (2024-04-03)
-------------------
