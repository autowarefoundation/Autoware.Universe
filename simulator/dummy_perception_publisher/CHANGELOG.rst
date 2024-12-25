^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package dummy_perception_publisher
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

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
* fix(cpplint): include what you use - simulator (`#9572 <https://github.com/autowarefoundation/autoware.universe/issues/9572>`_)
* fix(dummy_perception_publisher): fix clang-diagnostic-unused-private-field (`#9447 <https://github.com/autowarefoundation/autoware.universe/issues/9447>`_)
* 0.39.0
* update changelog
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* fix: fix ticket links to point to https://github.com/autowarefoundation/autoware.universe (`#9304 <https://github.com/autowarefoundation/autoware.universe/issues/9304>`_)
* chore(package.xml): bump version to 0.38.0 (`#9266 <https://github.com/autowarefoundation/autoware.universe/issues/9266>`_) (`#9284 <https://github.com/autowarefoundation/autoware.universe/issues/9284>`_)
  * unify package.xml version to 0.37.0
  * remove system_monitor/CHANGELOG.rst
  * add changelog
  * 0.38.0
  ---------
* Contributors: Esteve Fernandez, Fumiya Watanabe, M. Fatih Cırıt, Ryohsuke Mitsudome, Ryuta Kambe, Yutaka Kondo

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
* fix(dummy_perception_publisher, tier4_dummy_object_rviz_plugin): separate dummy object msg (`#8828 <https://github.com/autowarefoundation/autoware.universe/issues/8828>`_)
  * fix: dummy object rviz plugin dependency
  * fix: remove message from dummy perception publisher
  * fix: node name
  ---------
* feat(dummy_perception_publisher): modify orientation availability of dummy objects  (`#8534 <https://github.com/autowarefoundation/autoware.universe/issues/8534>`_)
  feat: add orientation availability for tracked objects in perception publisher
* refactor(shape_estimation): add package name prefix of autoware\_ (`#7999 <https://github.com/autowarefoundation/autoware.universe/issues/7999>`_)
  * refactor(shape_estimation): add package name prefix of autoware\_
  * style(pre-commit): autofix
  * fix: mising prefix
  * fix: cmake
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* refactor(detected_object_feature_remover)!: add package name prefix of autoware\_ (`#8127 <https://github.com/autowarefoundation/autoware.universe/issues/8127>`_)
  refactor(detected_object_feature_remover): add package name prefix of autoware\_
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* feat!: replace autoware_auto_msgs with autoware_msgs for simulator modules (`#7248 <https://github.com/autowarefoundation/autoware.universe/issues/7248>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* Contributors: Kosuke Takeuchi, Ryohsuke Mitsudome, Taekjin LEE, Takayuki Murooka, Yutaka Kondo, badai nguyen

0.26.0 (2024-04-03)
-------------------
* fix(log-messages): reduce excessive log messages (`#5971 <https://github.com/autowarefoundation/autoware.universe/issues/5971>`_)
* chore(build): remove tier4_autoware_utils.hpp evaluator/ simulator/ (`#4839 <https://github.com/autowarefoundation/autoware.universe/issues/4839>`_)
* fix: take dummy objects' height into calculation when locating their Z position (`#4195 <https://github.com/autowarefoundation/autoware.universe/issues/4195>`_)
  * Update node.cpp
  fix: consider dummy objects' height when locating its Z position
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* fix(dummy_perception_publisher): fix runtime error (`#3869 <https://github.com/autowarefoundation/autoware.universe/issues/3869>`_)
  fix(dummy_perception_publisher): fix namespace
* feat(dummy_perception_publisher): publish ground truth object in option (`#3731 <https://github.com/autowarefoundation/autoware.universe/issues/3731>`_)
  * add ground truth publisher
  * refactor ground truth publisher
  * enable to fix random seed
  * update readme
  * change GT output name to debug info
  * debug info must be under the node ns
  ---------
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(dummy_perception_publisher): add parameter to configure z pose of dummy object (`#3457 <https://github.com/autowarefoundation/autoware.universe/issues/3457>`_)
* chore(typo): eliminate typos (`#2216 <https://github.com/autowarefoundation/autoware.universe/issues/2216>`_)
  * Replace 'asssert' with 'assert'
  * fix(typo): computationall => computational
  * fix(typo): collinearity => collinearity
  * fix(typo): hypothenus => hypotenuse
  * fix(typo): numbef => number
  * fix(typo): missmatched => mismatched
  * fix(typo): minimun => minimum
  * fix(typo): neighbore => neighbor
  * fix(typo): neighbour => neighbor
  * fix(typo): propery => properly
  * ci(pre-commit): autofix
  * fix(typo): reagion => region
  * fix(typo): shirinking => shrinking
  * fix(typo): turining => turning
  * fix(typo): lexas => lexus
  * fix(typo): fastetst => fastest
  * fix(typo): analyse => analyze
  * fix(typo): ordinaray => ordinary
  * fix(typo): existance => existence
  * fix(typo): insert missing space
  * fix(typo): modify url including typo in original url
  * fix(typo): precompined => precomputed
  * fix(typo): magitude => magnitude
  * fix(typo): exernal => external
  * fix(typo): undderlying => underlying
  * fix(typo): expicitly => explicitly
  * fix(typo): paremterized => parameterized
  * fix(typo): thier => their
  * fix(typo): simualtor => simulator
  * fix(typo): modifiy => modify
  * fix(typo): neccessary => necessary
  * fix(typo): travelled => traveled
  * fix(typo): heursitic => heuristic
  * fix(typo): chagne => change
  * fix(typo): waypints => waypoints
  * fix(typo): unknwon => unknown
  * fix(typo): true => true
  * fix(typo): approximiate => approximate
  * fix(typo): analitically => analytically
  * fix(typo): modify url including typo in original url
  * fix(typo): computationall => computational
  * fix(typo): hypothenus => hypotenuse
  * fix(typo): neighbour => neighbor
  * ci(pre-commit): autofix
  * fix(typo): modify url including typo in original url
  * fix(typo): kiro => kilo
  * fix(typo): flowchar => flowchart
  * fix(typo): projecton => projection
  * fix(cspell): divide variable name with space to fix cspell error
  * fix(typo): yawrate => yaw rate
  * fix(typo): timelag => time_lag
  * fix(cspell): divide variable name with space to fix cspell error
  * fix(typo): retrive => retrieve
  * fix(typo): posemsg => pose msg
  * fix(cspell): replace northup with east_north_up
  * ci(pre-commit): autofix
  * fix(cspell): ignore person names
  * fix(cspell): ignore cspell error due to the source from OpenCV
  * fix(cspell): ignore cspell error due to the source from OpenCV
  * ci(pre-commit): autofix
  * chore(spell-check): ignore minx, maxx, miny, maxy, minz, maxz from autoware parameter names
  * chore(spell-check): Ignore cspell errors caused by external factor(plotjuggler)
  * fix(typo): dereferencable => dereferenceable
  * fix(typo): maxs => maxes
  * fix(typo): interpolatable => interpolable (more common)
  * fix(typo): fillter => filter
  * fix(typo): retrurn => return
  * fix(typo): diagnotics => diagnostics
  * fix(typo): Frist => First
  * chore(cspell): ignore ptfilter (external reference code)
  * fix(typo): overwite => overwrite
  * fix(cspell): use semi-major instead of semimajor
  * fix(typo): transien => transient
  * chore(cspell): ignore multipolygon, multilinestring
  * fix(typo): symetric => symmetric
  * chore(cspell): ignore Gammell (person name)
  * chore(cspell): ignore Karaman (person name)
  * chore(cspell): ignore feps with adding explanation
  * chore(cspell): replace iradius with i_radius
  * chore(cspell): replace inorm with inv_norm
  * chore(cspell): replace idist with i_dist
  * chore(cspell): ignore lfit, LFIT
  * chore(cspell): ignore Bboxes
  * fix(typo): unsuppoerted => unsupported
  * chore(cspell): ignore person names
  * chore(cspell): replace eigvec with eig_vec
  * chore(cspell): replace eigv with eig_v
  * chore(cspell): ignore eigenbox
  * chore(cspell): replace fltmax with flt_max
  * chore(cspell): ignore asan
  * ci(pre-commit): autofix
  * chore(cspell): ignore rsspace with adding explanation
  * chore(cspell): replace bfqueue with bf_queue
  * chore(cspell): expanded abbreviations in variable names　in debug_plot.py
  * chore(cspell): ignore nparr with adding explanation
  * chore(cspell): replace vmodel with vehicle_model
  * chore(cspell): ignore fpalgos
  * ci(pre-commit): autofix
  * chore(cspell): replace inpro with inner_product
  * chore(cspell): replace iradius with i_radius
  * chore(cspell): replace sstm with ss
  * chore(cspell): ignore dend
  * chore(cspell): ignore ndim, ndata, linewidth
  * ci(pre-commit): autofix
  * chore(cspell): ignore errors from parameter name
  * fix(typo): socre => score
  * chore(cspell): newstamp => new_stamp
  * chore(cspell): fuseon => fuseOn
  * chore(cspell): stdpair => std_pair
  * chore(cspell): boxid => box_id
  * fix(typo): intensity => intensity
  * fix(typo): inorder to => in order to
  * chore(cspell): ignore divup
  * chore(cspell): faceobjects => face_objects
  * chore(cspell): ignore rsspace
  * chore(cspell): ignore errors from citation
  * chore(cspell): ignore moraisim
  * chore(cspell): ignore ADMM
  * chore(cspell): ignore pointinpoly from reference
  * fix(typo): replaned => replanned
  * fix(typo): interaface => interface
  * fix(typo): supress => suppress
  * ci(pre-commit): autofix
  * fix(typo): distane => distance
  * fix(typo): relevent => relevant
  * fix(typo): pedestrain => pedestrian
  * fix(typo): obejct => object
  * fix(typo): paramters => parameters
  * ci(pre-commit): autofix
  * chore(cspell): ignore asdasd
  * chore(cspell): unnormalized => un-normalized
  * chore(cspell): precompilation => pre-compilation
  * fix(typo): compensents => components
  * fix(typo): cummulative => cumulative
  * chore(cspell): ignore degrounded
  * chore(cspell): ignore person names
  * ci(pre-commit): autofix
  * chore(cspell): publically => publicly
  * chore(cspell): interpolable => interpolatable
  * chore(cspell): ignore longl
  * chore(cspell): pngs => png images
  * chore(cspell): concate => concat
  * chore(cspell): ignore cand
  * chore(cspell): image magick => imagemagick
  * fix(typo): faceo_ject=> face_object
  * chore(cspell): velocityinsertion => velocity insertion
  * fix(typo): euclidian => euclidean
  * chore(cspell): ignore steerings
  * chore(cspell): ignore OCCUPANCYGRID
  * fix(typo): occuring => occurring
  * fix(typo): refere => refer
  * chore(cspell): ignore fourcell
  * chore(cspell): eigvalue => eigenvalue
  * chore(cspell): ignore badpt
  * chore(cspell): ignore divb
  * ci(pre-commit): autofix
  * style(pre-commit): autofix
  * doc: add comment to describe LFIT
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
  * fix(typo): computationall => computational
  * fix(typo): hypothenus => hypotenuse
  * ci(pre-commit): autofix
  * fix(typo): computationall => computational
  * fix(typo): hypothenus => hypotenuse
  * ci(pre-commit): autofix
  * update
  * fix(typo): interpolatable => interpolable (more common)
  * Squashed commit of the following:
  commit c7d3b7d2132323af3437af01e9d774b13005bace
  Author: Hirokazu Ishida <38597814+HiroIshida@users.noreply.github.com>
  Date:   Fri Dec 16 13:51:35 2022 +0900
  test(freespace_planning_algorithms): done't dump rosbag by default (`#2504 <https://github.com/autowarefoundation/autoware.universe/issues/2504>`_)
  commit 6731e0ced39e3187c2afffe839eaa697a19e5e84
  Author: kminoda <44218668+kminoda@users.noreply.github.com>
  Date:   Fri Dec 16 09:29:35 2022 +0900
  feat(pose_initializer): partial map loading (`#2500 <https://github.com/autowarefoundation/autoware.universe/issues/2500>`_)
  * first commit
  * move function
  * now works
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * clarify how to enable partial mao loading interface
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * Update localization/pose_initializer/config/pose_initializer.param.yaml
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * fix pre-commit
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  commit efb4ff1cea6e07aa9e894a6042e8685e30b420ba
  Author: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Date:   Thu Dec 15 17:29:44 2022 +0900
  feat(trajectory_follower): extend mpc trajectory for terminal yaw (`#2447 <https://github.com/autowarefoundation/autoware.universe/issues/2447>`_)
  * feat(trajectory_follower): extend mpc trajectory for terminal yaw
  * make mpc min vel param
  * add mpc extended point after smoothing
  * Revert "make mpc min vel param"
  This reverts commit 02157b6ae0c2ff1564840f6d15e3c55025327baf.
  * add comment and hypot
  * remove min vel
  * add flag for extending traj
  * add extend param to default param
  * fix typo
  * fix from TakaHoribe review
  * fix typo
  * refactor
  commit ad2ae7827bdc3af7da8607fdd53ea74940426421
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Thu Dec 15 15:52:34 2022 +0900
  feat(component_interface_tools): add service log checker  (`#2503 <https://github.com/autowarefoundation/autoware.universe/issues/2503>`_)
  * feat(component_interface_utils): add service log checker
  * feat(component_interface_tools): add service log checker
  * feat(component_interface_tools): add diagnostics
  * feat: update system error monitor config
  commit 4a13cc5a32898f5b17791d9381744bf71ff8ed20
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Thu Dec 15 12:54:11 2022 +0900
  fix(behavior_path_planner): fix goal lanelet extension (`#2508 <https://github.com/autowarefoundation/autoware.universe/issues/2508>`_)
  commit 77b1c36b5ca89b25250dcbb117c9f03a9c36c1c4
  Author: Kyoichi Sugahara <81.s.kyo.19@gmail.com>
  Date:   Thu Dec 15 10:45:45 2022 +0900
  feat(behavior_path_planner): change side shift module logic (`#2195 <https://github.com/autowarefoundation/autoware.universe/issues/2195>`_)
  * change side shift module design
  * cherry picked side shift controller
  * add debug marker to side shift
  * fix pointer error due to direct assignment
  added make_shared
  * add flow chart
  * add status of AFTER_SHIFT
  * remove function for debug
  * ci(pre-commit): autofix
  * fix flow chart
  * ci(pre-commit): autofix
  Co-authored-by: tanaka3 <ttatcoder@outlook.jp>
  Co-authored-by: Muhammad Zulfaqar Azmi <zulfaqar.azmi@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit 9183c4f20eb4592ed0b48c2eac67add070711677
  Author: Takamasa Horibe <horibe.takamasa@gmail.com>
  Date:   Wed Dec 14 19:59:00 2022 +0900
  refactor(simple_planning_simulator): make function for duplicated code (`#2502 <https://github.com/autowarefoundation/autoware.universe/issues/2502>`_)
  commit ed992b10ed326f03354dce3b563b8622f9ae9a6c
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 14 17:48:24 2022 +0900
  fix(behavior_path_planner): fix planner data copy (`#2501 <https://github.com/autowarefoundation/autoware.universe/issues/2501>`_)
  commit 0c6c46b33b3c828cb95eaa31fcbf85655fc6a55f
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 14 14:42:16 2022 +0900
  fix(behavior_path_planner): fix find nearest function from lateral distance (`#2499 <https://github.com/autowarefoundation/autoware.universe/issues/2499>`_)
  * feat(behavior_path_planner): fix find nearest function from lateral distance
  * empty commit
  commit a26b69d1df55e9369ea3adcdd011ae2d7c86dfb7
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 14 11:28:07 2022 +0900
  feat(behavior_path_planner): fix overlap checker (`#2498 <https://github.com/autowarefoundation/autoware.universe/issues/2498>`_)
  * feat(behavior_path_planner): fix overlap checker
  * remove reserve
  commit 3a24859ca6851caaeb25fc4fac2334fcbdb887d1
  Author: Ismet Atabay <56237550+ismetatabay@users.noreply.github.com>
  Date:   Tue Dec 13 16:51:59 2022 +0300
  feat(mission_planner): check goal footprint (`#2088 <https://github.com/autowarefoundation/autoware.universe/issues/2088>`_)
  commit b6a18855431b5f3a67fcbf383fac8df2b45d462e
  Author: Takamasa Horibe <horibe.takamasa@gmail.com>
  Date:   Tue Dec 13 22:46:24 2022 +0900
  feat(trajectory_visualizer): update for steer limit, remove tf for pose source (`#2267 <https://github.com/autowarefoundation/autoware.universe/issues/2267>`_)
  commit f1a9a9608559a5b89f631df3dc2fadd037e36ab4
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Tue Dec 13 19:47:16 2022 +0900
  feat(behavior_path_planner): remove unnecessary code and clean turn signal decider (`#2494 <https://github.com/autowarefoundation/autoware.universe/issues/2494>`_)
  * feat(behavior_path_planner): clean drivable area code
  * make a function for turn signal decider
  commit fafe1d8235b99302bc9ba8f3770ae34878f1e7e7
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Tue Dec 13 18:19:41 2022 +0900
  feat(behavior_path_planner): change turn signal output timing (`#2493 <https://github.com/autowarefoundation/autoware.universe/issues/2493>`_)
  commit c48b9cfa7074ecd46d96f6dc43679e17bde3a63d
  Author: kminoda <44218668+kminoda@users.noreply.github.com>
  Date:   Tue Dec 13 09:16:14 2022 +0900
  feat(map_loader): add differential map loading interface (`#2417 <https://github.com/autowarefoundation/autoware.universe/issues/2417>`_)
  * first commit
  * ci(pre-commit): autofix
  * added module load in _node.cpp
  * ci(pre-commit): autofix
  * create pcd metadata dict when either of the flag is true
  * ci(pre-commit): autofix
  * fix readme
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit 9a3613bfcd3e36e522d0ea9130f6200ca7689e2b
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Tue Dec 13 08:49:23 2022 +0900
  docs(default_ad_api): add readme (`#2491 <https://github.com/autowarefoundation/autoware.universe/issues/2491>`_)
  * docs(default_ad_api): add readme
  * feat: update table
  commit 49aa10b04de61c36706f6151d11bf17257ca54d1
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Tue Dec 13 06:46:20 2022 +0900
  feat(default_ad_api): split parameters into file (`#2488 <https://github.com/autowarefoundation/autoware.universe/issues/2488>`_)
  * feat(default_ad_api): split parameters into file
  * feat: remove old parameter
  * fix: test
  * feat: add default config
  commit 7f0138c356c742b6e15e571e7a4683caa55969ac
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Mon Dec 12 22:16:54 2022 +0900
  feat(behavior_path_planner, obstacle_avoidance_planner): add new drivable area (`#2472 <https://github.com/autowarefoundation/autoware.universe/issues/2472>`_)
  * update
  * update
  * update
  * update obstacle avoidance planner
  * update
  * clean code
  * uddate
  * clean code
  * remove resample
  * update
  * add orientation
  * change color
  * update
  * remove drivable area
  * add flag
  * update
  * update color
  * fix some codes
  * change to makerker array
  * change avoidance utils
  commit c855e23cc17d1518ebce5dd15629d03acfe17da3
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Mon Dec 12 17:15:10 2022 +0900
  refactor(vehicle_cmd_gate): remove old emergency topics (`#2403 <https://github.com/autowarefoundation/autoware.universe/issues/2403>`_)
  commit fa04d540c9afdded016730c9978920a194d2d2b4
  Author: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Date:   Mon Dec 12 16:04:00 2022 +0900
  feat: replace python launch with xml launch for system monitor (`#2430 <https://github.com/autowarefoundation/autoware.universe/issues/2430>`_)
  * feat: replace python launch with xml launch for system monitor
  * ci(pre-commit): autofix
  * update figure
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit 4a6990c49d1f8c3bedfb345e7c94c3c6893b4099
  Author: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Date:   Mon Dec 12 15:01:39 2022 +0900
  feat(trajectory_follower): pub steer converged marker (`#2441 <https://github.com/autowarefoundation/autoware.universe/issues/2441>`_)
  * feat(trajectory_follower): pub steer converged marker
  * Revert "feat(trajectory_follower): pub steer converged marker"
  This reverts commit a6f6917bc542d5b533150f6abba086121e800974.
  * add steer converged debug marker in contoller_node
  commit 3c01f15125dfbc45e1050ee96ccc42618d6ee6fd
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Mon Dec 12 12:48:41 2022 +0900
  docs(tier4_state_rviz_plugin): update readme (`#2475 <https://github.com/autowarefoundation/autoware.universe/issues/2475>`_)
  commit d8ece0040354be5381a27403bcc757354735a77b
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Mon Dec 12 11:57:03 2022 +0900
  chore(simulator_compatibility_test): suppress setuptools warnings (`#2483 <https://github.com/autowarefoundation/autoware.universe/issues/2483>`_)
  commit 727586bfe86dc9cb21ce34d9cbe19c241e162b04
  Author: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  Date:   Mon Dec 12 10:00:35 2022 +0900
  fix(behavior_path_planner): lane change candidate resolution (`#2426 <https://github.com/autowarefoundation/autoware.universe/issues/2426>`_)
  * fix(behavior_path_planner): lane change candidate resolution
  * rework sampling based  on current speed
  * refactor code
  * use util's resampler
  * consider min_resampling_points and resampling dt
  * simplify code
  commit 284548ca7f38b1d83af11f2b9caaac116eb9b09c
  Author: Zulfaqar Azmi <93502286+zulfaqar-azmi-t4@users.noreply.github.com>
  Date:   Mon Dec 12 09:57:19 2022 +0900
  fix(behavior_path_planner): minimum distance for lane change (`#2413 <https://github.com/autowarefoundation/autoware.universe/issues/2413>`_)
  commit 469d8927bd7a0c98b9d491d347e111065973e13f
  Author: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Date:   Fri Dec 9 21:27:18 2022 +0900
  revert(behavior_path): revert removal of refineGoalFunction (`#2340 <https://github.com/autowarefoundation/autoware.universe/issues/2340>`_)" (`#2485 <https://github.com/autowarefoundation/autoware.universe/issues/2485>`_)
  This reverts commit 8e13ced6dfb6edfea77a589ef4cb93d82683bf51.
  commit d924f85b079dfe64feab017166685be40e977e62
  Author: NorahXiong <103234047+NorahXiong@users.noreply.github.com>
  Date:   Fri Dec 9 19:53:51 2022 +0800
  fix(freespace_planning_algorithms): fix rrtstar can't arrive goal error (`#2350 <https://github.com/autowarefoundation/autoware.universe/issues/2350>`_)
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  commit b2ded82324bce78d9db3ff01b0227b00709b1efe
  Author: badai nguyen <94814556+badai-nguyen@users.noreply.github.com>
  Date:   Fri Dec 9 17:12:13 2022 +0900
  fix(ground-segmentation): recheck gnd cluster pointcloud (`#2448 <https://github.com/autowarefoundation/autoware.universe/issues/2448>`_)
  * fix: reclassify ground cluster pcl
  * fix: add lowest-based recheck
  * chore: refactoring
  * chore: refactoring
  Co-authored-by: Shunsuke Miura <37187849+miursh@users.noreply.github.com>
  commit 8906a1e78bc5b7d6417683ecedc1efe3f48be31e
  Author: Takamasa Horibe <horibe.takamasa@gmail.com>
  Date:   Fri Dec 9 16:29:45 2022 +0900
  fix(trajectory_follower): fix mpc trajectory z pos (`#2482 <https://github.com/autowarefoundation/autoware.universe/issues/2482>`_)
  commit d4939058f05f9a1609f0ed22afbd0d4febfb212d
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Fri Dec 9 12:40:30 2022 +0900
  feat(behavior_velocity_planner): clean walkway module (`#2480 <https://github.com/autowarefoundation/autoware.universe/issues/2480>`_)
  commit d3b86a37ae7c3a0d59832caf56afa13b148d562c
  Author: Makoto Kurihara <mkuri8m@gmail.com>
  Date:   Thu Dec 8 22:59:32 2022 +0900
  fix(emergency_handler): fix mrm handling when mrm behavior is none (`#2476 <https://github.com/autowarefoundation/autoware.universe/issues/2476>`_)
  commit 2dde073a101e96757ef0cd189bb9ff06836934e9
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Thu Dec 8 17:16:13 2022 +0900
  feat(behavior_velocity_planner): add velocity factors (`#1985 <https://github.com/autowarefoundation/autoware.universe/issues/1985>`_)
  * (editting) add intersection_coordination to stop reason
  * (editting) add intersection coordination to stop reasons
  * (Editting) add v2x to stop reason
  * (editting) add stop reason2 publisher
  * (editting) add stop reason2 to  scene modules
  * add stop reason2 to obstacle stop planner and surround obstacle checker
  * Modify files including unintended change by rebase
  * ci(pre-commit): autofix
  * Modification 1:  not to publsh vacant stop reason, 2: change default status in obstacle stop and surround obstacle checker
  * fix error
  * ci(pre-commit): autofix
  * modification for renaming stop_reason2 to motion_factor
  * (Editting) rename variables
  * bug fix
  * (WIP) Add motion factor message. Modify scene modules due to new motion factor. Moving motion factor aggregator.
  * (WIP) Save current work. Modify aggregator, CMakeList. Add launcher
  * (WIP) Solved build error, but not launched
  * (WIP) fixing error in launch
  * (WIP) fixing error in launch
  * (WIP) fixing launch error
  * Fix error in launching motion factor aggregator
  * Delete unnecessary comment-out in CMakelists. Change remapping in launcher.
  * ci(pre-commit): autofix
  * pull the latest foundation/main
  * (fix for pre-commit.ci) Add <memory> to motion_factor_aggregator.hpp
  * ci(pre-commit): autofix
  * feat: add velocity factor interface
  * fix: fix build error
  * feat: stop sign
  * WIP
  * feat: update visualizer
  * feat: modify traffic light manager
  * feat: update velocity factors
  * feat: update api
  * feat: move adapi msgs
  * feat: remove old aggregator
  * feat: move api
  * feat: rename message
  * feat: add using
  * feat: add distance
  * feat: fix build error
  * feat: use nan as default distance
  * fix: set virtual traffic light detail
  * fix: remove debug code
  * fix: copyright
  Co-authored-by: TakumiKozaka-T4 <takumi.kozaka@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit 9a5057e4948ff5ac9165c14eb7112d79f2de76d5
  Author: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Date:   Thu Dec 8 13:42:50 2022 +0900
  fix(freespace_planning_algorithms): comment out failing tests (`#2440 <https://github.com/autowarefoundation/autoware.universe/issues/2440>`_)
  commit cddb8c74d0fbf49390b4d462c20c12bc257f4825
  Author: kminoda <44218668+kminoda@users.noreply.github.com>
  Date:   Thu Dec 8 11:57:04 2022 +0900
  feat(gyro_odometer): publish twist when both data arrives (`#2423 <https://github.com/autowarefoundation/autoware.universe/issues/2423>`_)
  * feat(gyro_odometer): publish when both data arrive
  * remove unnecessary commentouts
  * ci(pre-commit): autofix
  * use latest timestamp
  * small fix
  * debugged
  * update gyro_odometer
  * ci(pre-commit): autofix
  * add comments
  * add comments
  * ci(pre-commit): autofix
  * fix timestamp validation flow
  * ci(pre-commit): autofix
  * remove unnecessary commentouts
  * pre-commit
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit f0f513cf44532dfe8d51d27c4caef23fb694af16
  Author: kminoda <44218668+kminoda@users.noreply.github.com>
  Date:   Thu Dec 8 11:08:29 2022 +0900
  fix: remove unnecessary DEBUG_INFO declarations (`#2457 <https://github.com/autowarefoundation/autoware.universe/issues/2457>`_)
  commit 01daebf42937a05a2d83f3dee2c0778389492e50
  Author: Takayuki Murooka <takayuki5168@gmail.com>
  Date:   Thu Dec 8 00:28:35 2022 +0900
  fix(tier4_autoware_api_launch): add rosbridge_server dependency (`#2470 <https://github.com/autowarefoundation/autoware.universe/issues/2470>`_)
  commit 26ef8174b1c12b84070b36df2a7cd14bfa9c0363
  Author: Shumpei Wakabayashi <42209144+shmpwk@users.noreply.github.com>
  Date:   Wed Dec 7 19:32:09 2022 +0900
  fix: rename `use_external_emergency_stop` to  `check_external_emergency_heartbeat` (`#2455 <https://github.com/autowarefoundation/autoware.universe/issues/2455>`_)
  * fix: rename use_external_emergency_stop to check_external_emergency_heartbeat
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  commit 024b993a0db8c0d28db0f05f64990bed7069cbd8
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 7 18:00:32 2022 +0900
  fix(motion_utils): rename sampling function (`#2469 <https://github.com/autowarefoundation/autoware.universe/issues/2469>`_)
  commit c240ce2b6f4e79c435ed651b347a7d665a947862
  Author: Yukihiro Saito <yukky.saito@gmail.com>
  Date:   Wed Dec 7 16:33:44 2022 +0900
  feat: remove web controller (`#2405 <https://github.com/autowarefoundation/autoware.universe/issues/2405>`_)
  commit 2992b1cadae7e7ac86fd249998ce3c7ddbe476c9
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 7 15:39:28 2022 +0900
  feat(motion_utils): add points resample function (`#2465 <https://github.com/autowarefoundation/autoware.universe/issues/2465>`_)
  commit 4a75d7c0ddbd88f54afaf2bb05eb65138a53ea60
  Author: Mingyu1991 <115005477+Mingyu1991@users.noreply.github.com>
  Date:   Wed Dec 7 14:42:33 2022 +0900
  docs: update training data for traffic light (`#2464 <https://github.com/autowarefoundation/autoware.universe/issues/2464>`_)
  * update traffic light cnn classifier README.md
  * correct to upper case
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  commit a4287165be87fa7727f79c01dfb0bea6af54c333
  Author: Ryuta Kambe <veqcc.c@gmail.com>
  Date:   Wed Dec 7 12:21:49 2022 +0900
  perf(behavior_velocity_planner): remove unnecessary debug data (`#2462 <https://github.com/autowarefoundation/autoware.universe/issues/2462>`_)
  commit 0a5b2857d3b2c1c9370598013b25aeaebf2d654d
  Author: Yutaka Shimizu <43805014+purewater0901@users.noreply.github.com>
  Date:   Wed Dec 7 12:03:46 2022 +0900
  feat(behavior_path_planner): cut overlapped path (`#2451 <https://github.com/autowarefoundation/autoware.universe/issues/2451>`_)
  * feat(behavior_path_planner): cut overlapped path
  * clean code
  commit 65003dc99f2abe937afcc010514530fa666fbbfd
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Wed Dec 7 11:06:41 2022 +0900
  revert(default_ad_api): fix autoware state to add wait time (`#2407 <https://github.com/autowarefoundation/autoware.universe/issues/2407>`_) (`#2460 <https://github.com/autowarefoundation/autoware.universe/issues/2460>`_)
  Revert "fix(default_ad_api): fix autoware state to add wait time (`#2407 <https://github.com/autowarefoundation/autoware.universe/issues/2407>`_)"
  This reverts commit c4224854a7e57a9526dde998f742741fe383471c.
  commit fab18677ca4de378faff84a41db5147577e7448d
  Author: Makoto Kurihara <mkuri8m@gmail.com>
  Date:   Wed Dec 7 10:32:41 2022 +0900
  fix(raw_vehicle_cmd_converter): fix column index for map validation (`#2450 <https://github.com/autowarefoundation/autoware.universe/issues/2450>`_)
  commit a1d3c80a4f5e3a388887a5afb32d9bf7961301f1
  Author: Ambroise Vincent <ambroise.vincent@arm.com>
  Date:   Tue Dec 6 10:39:02 2022 +0100
  fix(tvm_utility): copy test result to CPU (`#2414 <https://github.com/autowarefoundation/autoware.universe/issues/2414>`_)
  Also remove dependency to autoware_auto_common.
  Issue-Id: SCM-5401
  Change-Id: I83b859742df2f2ff7df1d0bd2d287bfe0aa04c3d
  Co-authored-by: Xinyu Wang <93699235+angry-crab@users.noreply.github.com>
  commit eb9946832c7e42d5380fd71956165409d0b592c3
  Author: Mamoru Sobue <mamoru.sobue@tier4.jp>
  Date:   Tue Dec 6 18:11:41 2022 +0900
  chore(behaviror_velocity_planner): changed logging level for intersection (`#2459 <https://github.com/autowarefoundation/autoware.universe/issues/2459>`_)
  changed logging level
  commit c4224854a7e57a9526dde998f742741fe383471c
  Author: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Date:   Tue Dec 6 17:01:37 2022 +0900
  fix(default_ad_api): fix autoware state to add wait time (`#2407 <https://github.com/autowarefoundation/autoware.universe/issues/2407>`_)
  * fix(default_ad_api): fix autoware state to add wait time
  * Update system/default_ad_api/src/compatibility/autoware_state.cpp
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  commit f984fbb708cb02947ec2824ce041c739c35940f7
  Author: Takamasa Horibe <horibe.takamasa@gmail.com>
  Date:   Tue Dec 6 13:55:17 2022 +0900
  feat(transition_manager): add param to ignore autonomous transition condition (`#2453 <https://github.com/autowarefoundation/autoware.universe/issues/2453>`_)
  * feat(transition_manager): add param to ignore autonomous transition condition
  * same for modeChangeCompleted
  * remove debug print
  commit d3e640df270a0942c4639e11451faf26e099bbe1
  Author: Tomoya Kimura <tomoya.kimura@tier4.jp>
  Date:   Tue Dec 6 13:01:06 2022 +0900
  feat(operation_mode_transition_manager): transition to auto quickly when vehicle stops (`#2427 <https://github.com/autowarefoundation/autoware.universe/issues/2427>`_)
  * chore(cspell): interpolable => interpolatable
  * Revert "Merge branch 'destroy-typos-check-all' into destroy-typos"
  This reverts commit 6116ca02d9df59f815d772a271fed7b0b21ebaf7, reversing
  changes made to 1f7157a6b6d957dc0ddd2ac5ef7f8a36c94b96e4.
  * chore: fix duplication of parameter
  * chore: fix duplication of function
  * revert: system/system_monitor/launch/system_monitor.launch.xml
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Yukihiro Saito <yukky.saito@gmail.com>
* feat(dummy_perception_publisher): publish object with acc (`#1853 <https://github.com/autowarefoundation/autoware.universe/issues/1853>`_)
  * feat(dummy_perception_publisher): publish object with acc
  * fix
  * fix
* fix(dummy_perception_publisher): independent of pointcloud from detection_successful_rate (`#1166 <https://github.com/autowarefoundation/autoware.universe/issues/1166>`_)
  * fix(dummy_perception_publisher): independent of pointcloud from detection_success_rate
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* perf(dummy_perception_publisher): tune ego-centric pointcloud generation of dummy perception publisher (`#926 <https://github.com/autowarefoundation/autoware.universe/issues/926>`_)
  * Take advantage of visible range
  * Tune
  * Fix: typo
  * Use hypot
* fix(dummy_perception_publisher): publish multiple layers of pointcloud (`#882 <https://github.com/autowarefoundation/autoware.universe/issues/882>`_)
  * fix: single -> multiple layers pointcloud
  * refactor: share common among different pcloud creators
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: remove bad chars (`#845 <https://github.com/autowarefoundation/autoware.universe/issues/845>`_)
* fix: suppress compiler warnings (`#852 <https://github.com/autowarefoundation/autoware.universe/issues/852>`_)
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* feat(dummy_perception_publisher): publish realistic dummy pointcloud using raymarchig (`#527 <https://github.com/autowarefoundation/autoware.universe/issues/527>`_)
  * Create pointcloud by raycasting from vehicle
  * [after-review] Use vector of ObjectInfo
  * [after-review] Implemented by strategy pattern
  * [after-review] split files
  * Use pcl raytracing
  Tmp
  Tmp
  Tmp
  * Add signed distance function lib
  * Use sdf library
  * Remove no longer used functions
  * Refactor
  * Simplify getPoint
  * Raytracing considering inter object relation
  * Add random noise
  * Default is object centric
  * Return if no objects are detected
  * Change definition of tf_global_to_local (same as other autoware codes)
  * Remove create method
  * Reaname: VehicleCentric -> EgoCentric
  * Refactor a bit
  * Tune parameter
  * Fix: Even if selected_idices is zero, pointclouds must be published
  * Fix launch file
  * Fix typo
  * Fix: create merged pointcloud when no idx is selected
  * Use ray-maching by default
* fix(dummy_perception_publisher): modify build error in rolling (`#761 <https://github.com/autowarefoundation/autoware.universe/issues/761>`_)
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: move empty_objects_publisher (`#613 <https://github.com/autowarefoundation/autoware.universe/issues/613>`_)
  * feat: move empty_objects_publisher
  * fix group of empty_object_publisher
* fix(dummy_perception_publisher): modified objects also use baselink z-position (`#588 <https://github.com/autowarefoundation/autoware.universe/issues/588>`_)
* revert(dummy_perception): change leaf size and disable ray trace (`#468 <https://github.com/autowarefoundation/autoware.universe/issues/468>`_)
  * Revert "chore(dummy_perception_publisher): change raytrace param (`#414 <https://github.com/autowarefoundation/autoware.universe/issues/414>`_)"
  This reverts commit d29e0e1d6630ef53edea1dd66bebf1a657aa6e8b.
  * chore(dummy_perception): revert change leaf size and disable raytrace
* feat(tier4_simulator_launch, dummy_perception_publisher): launch perception modules from simulator.launch.xml (`#465 <https://github.com/autowarefoundation/autoware.universe/issues/465>`_)
  * feat(tier4_simulator_launch, dummy_perception_publisher): launch perception modules from simualtor.launch.xml
  * remove perception launching dummy_perception_publisher.launch.xml
  * remove unnecessary comment
* fix(dummy_perception): fix to use launch at perception launch (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * fix(dummy_perception): fix to use launch file in perception launch
  * fix(tier4_perception_launch): fix angle increment for occupancy grid
* chore(dummy_perception_publisher): change raytrace param (`#414 <https://github.com/autowarefoundation/autoware.universe/issues/414>`_)
* chore: replace legacy timer (`#329 <https://github.com/autowarefoundation/autoware.universe/issues/329>`_)
  * chore(goal_distance_calculator): replace legacy timer
  * chore(path_distance_calculator): replace legacy timer
  * chore(control_performance_analysis): replace legacy timer
  * chore(external_cmd_selector): replace legacy timer
  * chore(joy_controller): replace legacy timer
  * chore(lane_departure_checker): replace legacy timer
  * chore(obstacle_collision_checker): replace legacy timer
  * chore(pure_pursuit): replace legacy timer
  * chore(shift_decider): replace legacy timer
  * chore(trajectory_follower_nodes): replace legacy timer
  * chore(vehicle_cmd_gate): replace legacy timer
  * chore(ekf_localizer): replace legacy timer
  * chore(localization_error_monitor): replace legacy timer
  * chore(multi_object_tracker): replace legacy timer
  * chore(tensorrt_yolo): replace legacy timer
  * chore(traffic_light_classifier): replace legacy timer
  * chore(traffic_light_ssd_fine_detector): replace legacy timer
  * chore(traffic_light_visualization): replace legacy timer
  * chore(behavior_path_planner): replace legacy timer
  * chore(costmap_generator): replace legacy timer
  * chore(freespace_planner): replace legacy timer
  * chore(planning_error_monitor): replace legacy timer
  * chore(scenario_selector): replace legacy timer
  * chore(pointcloud_preprocessor): replace legacy timer
  * chore(dummy_perception_publisher): replace legacy timer
  * chore(ad_service_state_monitor): replace legacy timer
  * chore(dummy_diag_publisher): replace legacy timer
  * chore(emergency_handler): replace legacy timer
  * chore(system_error_monitor): replace legacy timer
  * chore(topic_state_monitor): replace legacy timer
  * chore(accel_brake_map_calibrator): replace legacy timer
  * chore(external_cmd_converter): replace legacy timer
  * chore(pacmod_interface): replace legacy timer
  * chore(lint): apply pre-commit
* fix(dummy_perception): fix ns and topic (`#242 <https://github.com/autowarefoundation/autoware.universe/issues/242>`_)
* feat: change launch package name (`#186 <https://github.com/autowarefoundation/autoware.universe/issues/186>`_)
  * rename launch folder
  * autoware_launch -> tier4_autoware_launch
  * integration_launch -> tier4_integration_launch
  * map_launch -> tier4_map_launch
  * fix
  * planning_launch -> tier4_planning_launch
  * simulator_launch -> tier4_simulator_launch
  * control_launch -> tier4_control_launch
  * localization_launch -> tier4_localization_launch
  * perception_launch -> tier4_perception_launch
  * sensing_launch -> tier4_sensing_launch
  * system_launch -> tier4_system_launch
  * ci(pre-commit): autofix
  * vehicle_launch -> tier4_vehicle_launch
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: tanaka3 <ttatcoder@outlook.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add dummy perception publisher (`#90 <https://github.com/autowarefoundation/autoware.universe/issues/90>`_)
  * release v0.4.0
  * add use_object_recognition flag in dummy_perception_publisher (`#696 <https://github.com/autowarefoundation/autoware.universe/issues/696>`_)
  * remove ROS1 packages temporarily
  * add sample ros2 packages
  * remove ROS1 packages
  * Revert "remove ROS1 packages temporarily"
  This reverts commit 2e9822586a3539a32653e6bcd378715674b519ca.
  * add COLCON_IGNORE to ros1 packages
  * Rename launch files to launch.xml (`#28 <https://github.com/autowarefoundation/autoware.universe/issues/28>`_)
  * Port dummy_perception_publisher to ROS2 (`#90 <https://github.com/autowarefoundation/autoware.universe/issues/90>`_)
  * Port dummy_perception_publisher to ROS2
  * Uncrustify
  * Lint
  * Copyright
  * Period
  * Further ament_cpplint fixes
  * Convert calls of Duration to Duration::from_seconds where appropriate (`#131 <https://github.com/autowarefoundation/autoware.universe/issues/131>`_)
  * Use quotes for includes where appropriate (`#144 <https://github.com/autowarefoundation/autoware.universe/issues/144>`_)
  * Use quotes for includes where appropriate
  * Fix lint tests
  * Make tests pass hopefully
  * adding linters to dummy_perception_publisher (`#220 <https://github.com/autowarefoundation/autoware.universe/issues/220>`_)
  * [dummy_perception_publisher] fix launch file and installation (`#215 <https://github.com/autowarefoundation/autoware.universe/issues/215>`_)
  * [dummy_perception_publisher] fix launch file and installation
  * Apply suggestions from code review
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * reduce terminal ouput for better error message visibility (`#200 <https://github.com/autowarefoundation/autoware.universe/issues/200>`_)
  * reduce terminal ouput for better error message visibility
  * [costmap_generator] fix waiting for first transform
  * fix tests
  * fix test
  * modify launch file for making psim work (`#238 <https://github.com/autowarefoundation/autoware.universe/issues/238>`_)
  * modify launch file for making psim work
  * remove unnecesary ns
  * Ros2 v0.8.0 dummy perception publisher (`#286 <https://github.com/autowarefoundation/autoware.universe/issues/286>`_)
  * Remove "/" in frame_id (`#406 <https://github.com/autowarefoundation/autoware.universe/issues/406>`_)
  * Fix transform (`#420 <https://github.com/autowarefoundation/autoware.universe/issues/420>`_)
  * Replace rclcpp::Time(0) by tf2::TimePointZero() in lookupTransform
  * Fix canTransform
  * Fix test
  * add use_sim-time option (`#454 <https://github.com/autowarefoundation/autoware.universe/issues/454>`_)
  * Remove use_sim_time for set_parameter (`#1260 <https://github.com/autowarefoundation/autoware.universe/issues/1260>`_)
  * Diable dummy_perception_publisher if argument 'scenario_simulation' i… (`#1275 <https://github.com/autowarefoundation/autoware.universe/issues/1275>`_)
  * Diable dummy_perception_publisher if argument 'scenario_simulation' is true
  * Rename argument to 'disable_dummy_perception_publisher_node' from 'scenario_simulation'
  * change theta step for obj point cloud (`#1280 <https://github.com/autowarefoundation/autoware.universe/issues/1280>`_)
  * Revert changes of PR `#1275 <https://github.com/autowarefoundation/autoware.universe/issues/1275>`_ (`#1377 <https://github.com/autowarefoundation/autoware.universe/issues/1377>`_)
  * Add pre-commit (`#1560 <https://github.com/autowarefoundation/autoware.universe/issues/1560>`_)
  * add pre-commit
  * add pre-commit-config
  * add additional settings for private repository
  * use default pre-commit-config
  * update pre-commit setting
  * Ignore whitespace for line breaks in markdown
  * Update .github/workflows/pre-commit.yml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  * exclude svg
  * remove pretty-format-json
  * add double-quote-string-fixer
  * consider COLCON_IGNORE file when seaching modified package
  * format file
  * pre-commit fixes
  * Update pre-commit.yml
  * Update .pre-commit-config.yaml
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * Fix dependency type of rosidl_default_generators (`#1801 <https://github.com/autowarefoundation/autoware.universe/issues/1801>`_)
  * Fix dependency type of rosidl_default_generators
  * Remove unnecessary depends
  * Use ament_cmake_auto
  * Fix -Wunused-parameter (`#1836 <https://github.com/autowarefoundation/autoware.universe/issues/1836>`_)
  * Fix -Wunused-parameter
  * Fix mistake
  * fix spell
  * Fix lint issues
  * Ignore flake8 warnings
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  * fix topic namespace (`#2054 <https://github.com/autowarefoundation/autoware.universe/issues/2054>`_)
  * add sort-package-xml hook in pre-commit (`#1881 <https://github.com/autowarefoundation/autoware.universe/issues/1881>`_)
  * add sort xml hook in pre-commit
  * change retval to exit_status
  * rename
  * add prettier plugin-xml
  * use early return
  * add license note
  * add tier4 license
  * restore prettier
  * change license order
  * move local hooks to public repo
  * move prettier-xml to pre-commit-hooks-ros
  * update version for bug-fix
  * apply pre-commit
  * Feature/porting occlusion spot (`#1740 <https://github.com/autowarefoundation/autoware.universe/issues/1740>`_)
  * Feature/occlusion_spot safety planner public road (`#1594 <https://github.com/autowarefoundation/autoware.universe/issues/1594>`_)
  * add blind spot safety planner public road
  * remove duplicated procesing
  * remove unused private param
  * renaming fix typo add comments
  * fix spell check
  * velocity -> relative velocity
  * calc2d, To param, simplify search, To original
  * add the num possible collision gurd
  * computational cost reduction
  * Cosmetic change for PossibleCollisionInfo
  * add interpolation to possible collision value
  * refactor codes
  * fix details
  * invalid point gurd
  * To Param
  * refacotor to occlusion spot util
  * cosmetic change
  * clean up blindspot
  * To Occlusion Spot
  * revise readme
  * refactor drawing
  * for better explanation
  * fix velocity profile
  * clean up details
  * cosmetic change for debug marker
  * use max velocity in obstacle info instead
  * add gtest for Too Many Possible Collision case
  * change case
  * refactor readme
  * minor fix
  * add more occlusion spot explanation
  * replace svg
  * add gtest build path lanelet
  * hotfix lateral distance and searching method
  * update g test for lateral distance
  * use faster search
  * set more realistic param
  * add lanelet subtype highway
  * cosmetic change of reviews
  * add occlusion spot module in private area (`#1640 <https://github.com/autowarefoundation/autoware.universe/issues/1640>`_)
  * add occlusion spot in private
  * For debugging change
  * add spline interpolation to path
  * add review changes
  * adding minor change
  * cosmetic change
  * Vector to retval
  * Blindspot To OcclusionSpot1
  * To Occlusion Spot 2
  * To Occlusions spot3
  * update gtest with unified anchor
  * remove anchor
  * add test slice
  * simplify interpolation
  * Too Occlusion spot4
  * add test buffer
  * To Occlusion spot
  * namespace gurd
  * correct slice and add interpolation first
  * handle self crossing with check for invation
  * to ros debug stream
  * removed unused interpolation
  * add readme to plant uml
  * cosmetic change
  * minor change
  * update readme
  * review change
  * change occlusion spot text color
  * To Offset no Throw
  * better debug marker
  * catch only inversion error
  * skip return in case of inversion
  * for better grid
  * simplify path lanelet at first
  * remove std::cout
  * for better path
  * limit ego path inside target lanelet location
  * remove last points
  * cosmetic change for markers
  * apply module for limited scene
  * fix interpolation gurd
  * for better params
  * do not includes path behind
  * remove dummy perception publisher
  * Revert "remove dummy perception publisher"
  This reverts commit 4acad985fe31dd9befaa21a16631495de6c3a117.
  * replace hard coded occupancy grid option in psim
  * remove respawn
  * add arg to params and remove redundunt launch
  * fix spell check
  * fix default value
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Feature/occlusion spot private slice size param (`#1703 <https://github.com/autowarefoundation/autoware.universe/issues/1703>`_)
  * add min slice size
  * for a bit narrow lateral distance
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_velocity_planner/config/occlusion_spot_param.yaml
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Rename files
  * Porting to ros2
  * pre-commit fixes
  * Fix typo
  * Fix launch namespace
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Fix parameter type
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  * Update planning/scenario_planning/lane_driving/behavior_planning/behavior_velocity_planner/src/scene_module/occlusion_spot/scene_occlusion_spot_in_private_road.cpp
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  * Change formatter to clang-format and black (`#2332 <https://github.com/autowarefoundation/autoware.universe/issues/2332>`_)
  * Revert "Temporarily comment out pre-commit hooks"
  This reverts commit 748e9cdb145ce12f8b520bcbd97f5ff899fc28a3.
  * Replace ament_lint_common with autoware_lint_common
  * Remove ament_cmake_uncrustify and ament_clang_format
  * Apply Black
  * Apply clang-format
  * Fix build errors
  * Fix for cpplint
  * Fix include double quotes to angle brackets
  * Apply clang-format
  * Fix build errors
  * Add COLCON_IGNORE (`#500 <https://github.com/autowarefoundation/autoware.universe/issues/500>`_)
  * port dummy perception publisher to auto (`#562 <https://github.com/autowarefoundation/autoware.universe/issues/562>`_)
  * port dummy perception publisher to auto
  * autoware_perception_msgs/DynamicObjectWithFeatureArray convert to autoware_perception_msgs/DetectedObjectsWithFeature
  * change tracked objects to PREDICTED objects
  * separate pub type with real
  * Add README.md to dummy perception publisher (`#641 <https://github.com/autowarefoundation/autoware.universe/issues/641>`_)
  * Added readme for dummy_perception_pub
  * README update
  * README update
  * Fix pre-commit
  * fix typo
  * Update README.md
  * Update README.md
  * Update README.md
  * Modified node.cpp
  * Modified README.md
  * change parameter name
  * Update README.md
  * [shape_estimation]change type (`#663 <https://github.com/autowarefoundation/autoware.universe/issues/663>`_)
  * change output type of shape_estimation
  * remove unused function
  * add dynamic_object_converter
  * rename
  * fix typo
  * fix dummy_perception_publisher
  * update readme
  * fix copyright
  * rename package
  * add readme
  * fix launch name
  * remove unused variable
  * fix readme
  * fix convert function
  * change topic name of DynamicObjectsWithFeature
  * Fix no ground pointcloud topic name (`#733 <https://github.com/autowarefoundation/autoware.universe/issues/733>`_)
  Co-authored-by: j4tfwm6z <proj-jpntaxi@tier4.jp>
  * auto/fix occupancy grid name space in dummy perception publisher (`#739 <https://github.com/autowarefoundation/autoware.universe/issues/739>`_)
  * fix name space
  * change namespace: object_segmentation -> obstacle_segmentation
  * feat: add use_traffic_light status
  Co-authored-by: mitsudome-r <ryohsuke.mitsudome@tier4.jp>
  Co-authored-by: Taichi Higashide <taichi.higashide@tier4.jp>
  Co-authored-by: Nikolai Morin <nnmmgit@gmail.com>
  Co-authored-by: nik-tier4 <71747268+nik-tier4@users.noreply.github.com>
  Co-authored-by: Ryohsuke Mitsudome <43976834+mitsudome-r@users.noreply.github.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Kosuke Murakami <kosuke.murakami@tier4.jp>
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: tkimura4 <tomoya.kimura@tier4.jp>
  Co-authored-by: Tatsuya Yamasaki <httperror@404-notfound.jp>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Keisuke Shima <19993104+KeisukeShima@users.noreply.github.com>
  Co-authored-by: Kazuki Miyahara <kmiya@outlook.com>
  Co-authored-by: pre-commit <pre-commit@example.com>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Azumi Suzuki <38061530+s-azumi@users.noreply.github.com>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
  Co-authored-by: Yohei Mishina <66298900+YoheiMishina@users.noreply.github.com>
  Co-authored-by: j4tfwm6z <proj-jpntaxi@tier4.jp>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
* Contributors: Ahmed Ebrahim, Berkay Karaman, Daisuke Nishimatsu, Guan  Muhua (Tier4), Hirokazu Ishida, Kenji Miyake, Kotaro Yoshimoto, Mamoru Sobue, Maxime CLEMENT, Satoshi OTA, Shumpei Wakabayashi, Takayuki Murooka, Takeshi Miura, Tomoya Kimura, Vincent Richard, Yoshi Ri, taikitanaka3
