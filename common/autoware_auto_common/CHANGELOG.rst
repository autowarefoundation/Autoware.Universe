^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package autoware_auto_common
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.40.0 (2024-12-12)
-------------------
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
* refactor(qp_interface): prefix package and namespace with autoware (`#9236 <https://github.com/autowarefoundation/autoware.universe/issues/9236>`_)
* Contributors: Esteve Fernandez, Fumiya Watanabe, Ryohsuke Mitsudome, Yutaka Kondo

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
* refactor(qp_interface): prefix package and namespace with autoware (`#9236 <https://github.com/autowarefoundation/autoware.universe/issues/9236>`_)
* Contributors: Esteve Fernandez, Yutaka Kondo

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* fix(autoware_auto_common): fix cppcheck warnings of functionStatic (`#8265 <https://github.com/autowarefoundation/autoware.universe/issues/8265>`_)
  fix: deal with functionStatic warnings
* fix(autoware_auto_common): nullptr_t (`#7212 <https://github.com/autowarefoundation/autoware.universe/issues/7212>`_)
* fix: do not use c++20 char8_t keyword (`#3629 <https://github.com/autowarefoundation/autoware.universe/issues/3629>`_)
  fix: do not use c++20 keyword as a type alias
* Contributors: Shumpei Wakabayashi, Yutaka Kondo, ralwing, taisa1

0.26.0 (2024-04-03)
-------------------
* fix(autoware_auto_common): move headers to a separate directory (`#5919 <https://github.com/autowarefoundation/autoware.universe/issues/5919>`_)
  * fix(autoware_auto_common): move headers to a separate directory
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: add maintainer (`#4234 <https://github.com/autowarefoundation/autoware.universe/issues/4234>`_)
  * chore: add maintainer
  * Update evaluator/localization_evaluator/package.xml
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
  ---------
  Co-authored-by: kminoda <44218668+kminoda@users.noreply.github.com>
* build: mark autoware_cmake as <buildtool_depend> (`#3616 <https://github.com/autowarefoundation/autoware.universe/issues/3616>`_)
  * build: mark autoware_cmake as <buildtool_depend>
  with <build_depend>, autoware_cmake is automatically exported with ament_target_dependencies() (unecessary)
  * style(pre-commit): autofix
  * chore: fix pre-commit errors
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
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
* revert: "feat(pointcloud_preprocessor): use point_cloud_msg_wrapper" (`#2317 <https://github.com/autowarefoundation/autoware.universe/issues/2317>`_)
  Revert "feat(pointcloud_preprocessor): use point_cloud_msg_wrapper (`#1276 <https://github.com/autowarefoundation/autoware.universe/issues/1276>`_)"
  This reverts commit ef7dcda087beffaa0acf35e9647be1cb439007ba.
* feat(pointcloud_preprocessor): use point_cloud_msg_wrapper (`#1276 <https://github.com/autowarefoundation/autoware.universe/issues/1276>`_)
* docs: update link style and fix typos (`#950 <https://github.com/autowarefoundation/autoware.universe/issues/950>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button (`#894 <https://github.com/autowarefoundation/autoware.universe/issues/894>`_)
  * feat(state_rviz_plugin): add GateMode and PathChangeApproval Button
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * docs: update link style
  * chore: fix link
  * feat(map_tf_generator): accelerate the 'viewer' coordinate calculation (`#890 <https://github.com/autowarefoundation/autoware.universe/issues/890>`_)
  * add random point sampling function to quickly calculate the 'viewer' coordinate
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * docs(obstacle_stop_planner): update documentation (`#880 <https://github.com/autowarefoundation/autoware.universe/issues/880>`_)
  * docs(tier4_traffic_light_rviz_plugin): update documentation (`#905 <https://github.com/autowarefoundation/autoware.universe/issues/905>`_)
  * fix(accel_brake_map_calibrator): rviz panel type (`#895 <https://github.com/autowarefoundation/autoware.universe/issues/895>`_)
  * fixed panel type
  * modified instruction for rosbag replay case
  * modified update_map_dir service name
  * fix(behavior velocity planner): skipping emplace back stop reason if it is empty (`#898 <https://github.com/autowarefoundation/autoware.universe/issues/898>`_)
  * skipping emplace back stop reason if it is empty
  * add braces
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  * feat(behavior_path_planner): weakened noise filtering of drivable area (`#838 <https://github.com/autowarefoundation/autoware.universe/issues/838>`_)
  * feat(behavior_path_planner): Weakened noise filtering of drivable area
  * fix lanelet's longitudinal disconnection
  * add comments of erode/dilate process
  * refactor(vehicle-cmd-gate): using namespace for msgs (`#913 <https://github.com/autowarefoundation/autoware.universe/issues/913>`_)
  * refactor(vehicle-cmd-gate): using namespace for msgs
  * for clang
  * feat(pose_initializer): introduce an array copy function (`#900 <https://github.com/autowarefoundation/autoware.universe/issues/900>`_)
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat: add lidar point filter when debug (`#865 <https://github.com/autowarefoundation/autoware.universe/issues/865>`_)
  * feat: add lidar point filter when debug
  * ci(pre-commit): autofix
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * feat(component_interface_utils): add interface classes  (`#899 <https://github.com/autowarefoundation/autoware.universe/issues/899>`_)
  * feat(component_interface_utils): add interface classes
  * feat(default_ad_api): apply the changes of interface utils
  * fix(component_interface_utils): remove old comment
  * fix(component_interface_utils): add client log
  * fix(component_interface_utils): remove unimplemented message
  * docs(component_interface_utils): add design policy
  * docs(component_interface_utils): add comment
  * refactor(vehicle_cmd_gate): change namespace in launch file (`#927 <https://github.com/autowarefoundation/autoware.universe/issues/927>`_)
  Co-authored-by: Berkay <berkay@leodrive.ai>
  * feat: visualize lane boundaries (`#923 <https://github.com/autowarefoundation/autoware.universe/issues/923>`_)
  * feat: visualize lane boundaries
  * fix: start_bound
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * fix(system_monitor): fix truncation warning in strncpy (`#872 <https://github.com/autowarefoundation/autoware.universe/issues/872>`_)
  * fix(system_monitor): fix truncation warning in strncpy
  * Use std::string constructor to copy char array
  * Fixed typo
  * fix(behavior_velocity_planner.stopline): extend following and previous search range to avoid no collision (`#917 <https://github.com/autowarefoundation/autoware.universe/issues/917>`_)
  * fix: extend following and previous search range to avoid no collision
  * chore: add debug marker
  * fix: simplify logic
  * chore: update debug code
  * fix: delete space
  * fix: some fix
  * ci(pre-commit): autofix
  * fix: delete debug code
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * docs(surround obstacle checker): update documentation (`#878 <https://github.com/autowarefoundation/autoware.universe/issues/878>`_)
  * docs(surround_obstacle_checker): update pub/sub topics & params
  * docs(surround_obstacle_checker): remove unused files
  * docs(surround_obstacke_checker): update purpose
  * feat(tier4_autoware_utils): add vehicle state checker (`#896 <https://github.com/autowarefoundation/autoware.universe/issues/896>`_)
  * feat(tier4_autoware_utils): add vehicle state checker
  * fix(tier4_autoware_utils): use absolute value
  * feat(tier4_autoware_utils): divide into two classies
  * test(tier4_autoware_utils): add unit test for vehicle_state checker
  * fix(tier4_autoware_utils): impl class inheritance
  * docs(tier4_autoware_utils): add vehicle_state_checker document
  * fix(tier4_autoware_utils): into same loop
  * fix(tier4_autoware_utils): fix variables name
  * fix(tier4_autoware_utils): remove redundant codes
  * fix(motion_velocity_smoother): fix overwriteStopPoint using backward point (`#816 <https://github.com/autowarefoundation/autoware.universe/issues/816>`_)
  * fix(motion_velocity_smoother): fix overwriteStopPoint using backward point
  * Modify overwriteStopPoint input and output
  * feat(obstacle_avoidance_planner): explicitly insert zero velocity (`#906 <https://github.com/autowarefoundation/autoware.universe/issues/906>`_)
  * feat(obstacle_avoidance_planner) fix bug of stop line unalignment
  * fix bug of unsorted output points
  * move calcVelocity in node.cpp
  * fix build error
  * feat(behavior_velocity): find occlusion more efficiently (`#829 <https://github.com/autowarefoundation/autoware.universe/issues/829>`_)
  * fix(system_monitor): add some smart information to diagnostics (`#708 <https://github.com/autowarefoundation/autoware.universe/issues/708>`_)
  * feat(obstacle_avoidance_planner): dealt with close lane change (`#921 <https://github.com/autowarefoundation/autoware.universe/issues/921>`_)
  * feat(obstacle_avoidance_planner): dealt with close lane change
  * fix bug of right lane change
  * feat(obstacle_avoidance_planner): some fix for narrow driving (`#916 <https://github.com/autowarefoundation/autoware.universe/issues/916>`_)
  * use car like constraints in mpt
  * use not widest bounds for the first bounds
  * organized params
  * fix format
  * prepare rear_drive and uniform_circle constraints
  * fix param callback
  * update config
  * remove unnecessary files
  * update tier4_planning_launch params
  * chore(obstacle_avoidance_planner): removed obsolete obstacle_avoidance_planner doc in Japanese (`#919 <https://github.com/autowarefoundation/autoware.universe/issues/919>`_)
  * chore(behavior_velocity_planner.stopline): add debug marker for stopline collision check (`#932 <https://github.com/autowarefoundation/autoware.universe/issues/932>`_)
  * chore(behavior_velocity_planner.stopline): add debug marker for stopline collision check
  * feat: use marker helper
  * feat(map_loader): visualize center line by points (`#931 <https://github.com/autowarefoundation/autoware.universe/issues/931>`_)
  * feat: visualize center line points
  * fix: delete space
  * feat: visualize center line by arrow
  * revert insertMarkerArray
  * fix: delete space
  * feat: add RTC interface (`#765 <https://github.com/autowarefoundation/autoware.universe/issues/765>`_)
  * feature(rtc_interface): add files
  * feature(rtc_interface): implement functions
  * feature(rtc_interface): reimprement functions to use CooperateCommands and write README.md
  * feature(rtc_interface): fix README
  * feature(rtc_interface): add getModuleType()
  * feature(rtc_interface): fix definition of constructor
  * feature(rtc_interface): fix time stamp
  * feature(rtc_interface): fix README
  * feature(rtc_interface): add isRegistered and clearCooperateStatus
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  * chore: sync files (`#911 <https://github.com/autowarefoundation/autoware.universe/issues/911>`_)
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  * fix: replace boost::mutex::scoped_lock to std::scoped_lock (`#907 <https://github.com/autowarefoundation/autoware.universe/issues/907>`_)
  * fix: replace boost::mutex::scoped_lock to std::scoped_lock
  * fix: replace boost::mutex to std::mutex
  * feat(tensorrt_yolo): add multi gpu support to tensorrt_yolo node (`#885 <https://github.com/autowarefoundation/autoware.universe/issues/885>`_)
  * feat(tensorrt_yolo): add multi gpu support to tensorrt_yolo node
  * feat(tensorrt_yolo): update arg
  Co-authored-by: Kaan Colak <kcolak@leodrive.ai>
  * feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner (`#887 <https://github.com/autowarefoundation/autoware.universe/issues/887>`_)
  * feat(tier4_planning_launch): create parameter yaml for behavior_velocity_planner
  * Update launch/tier4_planning_launch/config/scenario_planning/lane_driving/behavior_planning/behavior_velocity_planner/behavior_velocity_planner.param.yaml
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * feat: add param.yaml in behavior_velocity_planner package
  * some fix
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  * fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader (`#942 <https://github.com/autowarefoundation/autoware.universe/issues/942>`_)
  * fix(map_loader): use std::filesystem to load pcd files in pointcloud_map_loader
  * fix(map_loader): remove c_str
  * fix(map_loader): replace c_str to string
  * fix: relative link
  * fix: relative links
  * fix: relative links
  * fix: relative links
  * fix: typo
  * fix relative links
  * docs: ignore rare unknown words
  * ci(pre-commit): autofix
  * docs: ignore unknown words one by one
  * ci(pre-commit): autofix
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Ishita <ishitah.takeshi@gmail.com>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Satoshi OTA <44889564+satoshi-ota@users.noreply.github.com>
  Co-authored-by: Mamoru Sobue <hilo.soblin@gmail.com>
  Co-authored-by: TakumiKozaka-T4 <70260442+TakumiKozaka-T4@users.noreply.github.com>
  Co-authored-by: Takagi, Isamu <43976882+isamu-takagi@users.noreply.github.com>
  Co-authored-by: Takayuki Murooka <takayuki5168@gmail.com>
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: storrrrrrrrm <103425473+storrrrrrrrm@users.noreply.github.com>
  Co-authored-by: suchang <chang.su@autocore.ai>
  Co-authored-by: Berkay <brkay54@gmail.com>
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: ito-san <57388357+ito-san@users.noreply.github.com>
  Co-authored-by: Kosuke Takeuchi <kosuke.tnp@gmail.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: kk-inoue-esol <76925382+kk-inoue-esol@users.noreply.github.com>
  Co-authored-by: Fumiya Watanabe <rej55.g@gmail.com>
  Co-authored-by: awf-autoware-bot[bot] <94889083+awf-autoware-bot[bot]@users.noreply.github.com>
  Co-authored-by: kenji-miyake <kenji-miyake@users.noreply.github.com>
  Co-authored-by: RyuYamamoto <ryu.yamamoto@tier4.jp>
  Co-authored-by: Kaan Çolak <kaancolak95@gmail.com>
  Co-authored-by: Kaan Colak <kcolak@leodrive.ai>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* feat: isolate gtests in all packages (`#693 <https://github.com/autowarefoundation/autoware.universe/issues/693>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* chore: remove license notations from CMakeLists.txt (`#846 <https://github.com/autowarefoundation/autoware.universe/issues/846>`_)
* chore: remove bad chars (`#845 <https://github.com/autowarefoundation/autoware.universe/issues/845>`_)
* style: fix format of package.xml (`#844 <https://github.com/autowarefoundation/autoware.universe/issues/844>`_)
* fix(autoware_auto_tf2): modify build error in rolling (`#718 <https://github.com/autowarefoundation/autoware.universe/issues/718>`_)
  * fix(autoware_auto_common): modify build error in rolling
  * fix(autoware_auto_tf2): modify build error in rolling
  * fix(autoware_auto_geometry): modify build error in rolling
  * fix(simple_planning_simulator): add compile definition for geometry2
  * fix(motion_common): add compile definition for geometry2
  * fix(motion_testing): add compile definition for geometry2
  * fix(simple_planning_simulator): modify build error in rolling
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: fix typos (`#751 <https://github.com/autowarefoundation/autoware.universe/issues/751>`_)
* ci(pre-commit): clear the exclude option (`#426 <https://github.com/autowarefoundation/autoware.universe/issues/426>`_)
  * ci(pre-commit): remove unnecessary excludes
  * ci(pre-commit): autofix
  * ci(pre-commit): autofix
  * address pre-commit for Markdown files
  * fix Python imports
  * address cpplint errors
  * fix broken package.xml
  * rename ROS parameter files
  * fix build
  * use autoware_lint_common
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* chore: remove filesystem.hpp (`#440 <https://github.com/autowarefoundation/autoware.universe/issues/440>`_)
* feat: add autoware auto dependencies (`#185 <https://github.com/autowarefoundation/autoware.universe/issues/185>`_)
  * Back port .auto control packages (`#571 <https://github.com/autowarefoundation/autoware.universe/issues/571>`_)
  * Implement Lateral and Longitudinal Control Muxer
  * [`#570 <https://github.com/autowarefoundation/autoware.universe/issues/570>`_] Porting wf_simulator
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Deactivate flaky test in 'trajectory_follower_nodes'
  * [`#1189 <https://github.com/autowarefoundation/autoware.universe/issues/1189>`_] Fix flacky test in 'trajectory_follower_nodes/latlon_muxer'
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add osqp_interface package
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Add library code for MPC-based lateral control
  * [`#1271 <https://github.com/autowarefoundation/autoware.universe/issues/1271>`_] Use std::abs instead of abs
  * [`#1057 <https://github.com/autowarefoundation/autoware.universe/issues/1057>`_] Implement Lateral Controller for Cargo ODD
  * [`#1246 <https://github.com/autowarefoundation/autoware.universe/issues/1246>`_] Resolve "Test case names currently use snake_case but should be CamelCase"
  * [`#1325 <https://github.com/autowarefoundation/autoware.universe/issues/1325>`_] Deactivate flaky smoke test in 'trajectory_follower_nodes'
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add library code of longitudinal controller
  * Fix build error for trajectory follower
  * Fix build error for trajectory follower nodes
  * [`#1272 <https://github.com/autowarefoundation/autoware.universe/issues/1272>`_] Add AckermannControlCommand support to simple_planning_simulator
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add Longitudinal Controller node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rename velocity_controller -> longitudinal_controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Update CMakeLists.txt for the longitudinal_controller_node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add smoke test python launch file
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use LowPassFilter1d from trajectory_follower
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use autoware_auto_msgs
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Changes for .auto (debug msg tmp fix, common func, tf listener)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove unused parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Rm default params from declare_parameters + use autoware types
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use default param file to setup NodeOptions in the ros test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix docstring
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Replace receiving a Twist with a VehicleKinematicState
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change class variables format to m\_ prefix
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix plugin name of LongitudinalController in CMakeLists.txt
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix copyright dates
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Reorder includes
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add some tests (~89% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add more tests (90+% coverage without disabling flaky tests)
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Use Float32MultiArrayDiagnostic message for debug and slope
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Calculate wheel_base value from vehicle parameters
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Cleanup redundant logger setting in tests
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Set ROS_DOMAIN_ID when running tests to prevent CI failures
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Remove TF listener and use published vehicle state instead
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Change smoke tests to use autoware_testing
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Add plotjuggler cfg for both lateral and longitudinal control
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Improve design documents
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Disable flaky test
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Properly transform vehicle state in longitudinal node
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix TF buffer of lateral controller
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Tuning of lateral controller for LGSVL
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix formating
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix /tf_static sub to be transient_local
  * [`#1058 <https://github.com/autowarefoundation/autoware.universe/issues/1058>`_] Fix yaw recalculation of reverse trajs in the lateral controller
  * modify trajectory_follower for galactic build
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update simple_planning_simulator
  * [`#1379 <https://github.com/autowarefoundation/autoware.universe/issues/1379>`_] Update trajectory_follower_nodes
  * apply trajectory msg modification in control
  * move directory
  * remote control/trajectory_follower level dorectpry
  * remove .iv trajectory follower
  * use .auto trajectory_follower
  * remove .iv simple_planning_simulator & osqp_interface
  * use .iv simple_planning_simulator & osqp_interface
  * add tmp_autoware_auto_dependencies
  * tmporally add autoware_auto_msgs
  * apply .auto message split
  * fix build depend
  * fix packages using osqp
  * fix autoware_auto_geometry
  * ignore lint of some packages
  * ignore ament_lint of some packages
  * ignore lint/pre-commit of trajectory_follower_nodes
  * disable unit tests of some packages
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  * Port parking planner packages from .Auto (`#600 <https://github.com/autowarefoundation/autoware.universe/issues/600>`_)
  * Copy code of 'vehicle_constants_manager'
  * Fix vehicle_constants_manager for ROS galactic
  * Rm .iv costmap_generator freespace_planner freespace_planning_aglorihtms
  * Add astar_search (from .Auto)
  * Copy freespace_planner from .Auto
  * Update freespace_planner for .IV
  * Copy costmap_generator from .Auto
  * Copy and update had_map_utils from .Auto
  * Update costmap_generator
  * Copy costmap_generator_nodes
  * Update costmap_generator_nodes
  * Comment out all tests
  * Move vehicle_constant_managers to tmp_autoware_auto_dependencies
  * ignore pre-commit for back-ported packages
  * ignore testing
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  * fix: fix pre-commit
  * fix: fix markdownlint
  * fix: fix cpplint
  * feat: remove autoware_auto_dependencies
  Co-authored-by: Takamasa Horibe <horibe.takamasa@gmail.com>
  Co-authored-by: Maxime CLEMENT <maxime.clement@tier4.jp>
  Co-authored-by: Joshua Whitley <josh.whitley@autoware.org>
  Co-authored-by: Igor Bogoslavskyi <igor.bogoslavskyi@gmail.com>
  Co-authored-by: MIURA Yasuyuki <kokosabu@gmail.com>
  Co-authored-by: wep21 <border_goldenmarket@yahoo.co.jp>
  Co-authored-by: tomoya.kimura <tomoya.kimura@tier4.jp>
  Co-authored-by: Maxime CLEMENT <78338830+maxime-clem@users.noreply.github.com>
* Contributors: Daisuke Nishimatsu, Esteve Fernandez, Kenji Miyake, Kotaro Yoshimoto, Maxime CLEMENT, Satoshi OTA, Shumpei Wakabayashi, Shunsuke Miura, Takeshi Miura, Vincent Richard
