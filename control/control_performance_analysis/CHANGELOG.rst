^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Changelog for package control_performance_analysis
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

0.38.0 (2024-11-08)
-------------------
* unify package.xml version to 0.37.0
* refactor(signal_processing): prefix package and namespace with autoware (`#8541 <https://github.com/autowarefoundation/autoware.universe/issues/8541>`_)
* fix(control_performance_analysis): fix unusedFunction (`#8557 <https://github.com/autowarefoundation/autoware.universe/issues/8557>`_)
  fix:unusedFunction
* refactor(universe_utils/motion_utils)!: add autoware namespace (`#7594 <https://github.com/autowarefoundation/autoware.universe/issues/7594>`_)
* refactor(motion_utils)!: add autoware prefix and include dir (`#7539 <https://github.com/autowarefoundation/autoware.universe/issues/7539>`_)
  refactor(motion_utils): add autoware prefix and include dir
* feat(autoware_universe_utils)!: rename from tier4_autoware_utils (`#7538 <https://github.com/autowarefoundation/autoware.universe/issues/7538>`_)
  Co-authored-by: kosuke55 <kosuke.tnp@gmail.com>
* refactor(vehicle_info_utils)!: prefix package and namespace with autoware (`#7353 <https://github.com/autowarefoundation/autoware.universe/issues/7353>`_)
  * chore(autoware_vehicle_info_utils): rename header
  * chore(bpp-common): vehicle info
  * chore(path_optimizer): vehicle info
  * chore(velocity_smoother): vehicle info
  * chore(bvp-common): vehicle info
  * chore(static_centerline_generator): vehicle info
  * chore(obstacle_cruise_planner): vehicle info
  * chore(obstacle_velocity_limiter): vehicle info
  * chore(mission_planner): vehicle info
  * chore(obstacle_stop_planner): vehicle info
  * chore(planning_validator): vehicle info
  * chore(surround_obstacle_checker): vehicle info
  * chore(goal_planner): vehicle info
  * chore(start_planner): vehicle info
  * chore(control_performance_analysis): vehicle info
  * chore(lane_departure_checker): vehicle info
  * chore(predicted_path_checker): vehicle info
  * chore(vehicle_cmd_gate): vehicle info
  * chore(obstacle_collision_checker): vehicle info
  * chore(operation_mode_transition_manager): vehicle info
  * chore(mpc): vehicle info
  * chore(control): vehicle info
  * chore(common): vehicle info
  * chore(perception): vehicle info
  * chore(evaluator): vehicle info
  * chore(freespace): vehicle info
  * chore(planning): vehicle info
  * chore(vehicle): vehicle info
  * chore(simulator): vehicle info
  * chore(launch): vehicle info
  * chore(system): vehicle info
  * chore(sensing): vehicle info
  * fix(autoware_joy_controller): remove unused deps
  ---------
* feat!: replace autoware_auto_msgs with autoware_msgs for control modules (`#7240 <https://github.com/autowarefoundation/autoware.universe/issues/7240>`_)
  Co-authored-by: Cynthia Liu <cynthia.liu@autocore.ai>
  Co-authored-by: NorahXiong <norah.xiong@autocore.ai>
  Co-authored-by: beginningfan <beginning.fan@autocore.ai>
* fix(control_performance_analysis): fix bug of ignoredReturnValue (`#6921 <https://github.com/autowarefoundation/autoware.universe/issues/6921>`_)
* Contributors: Esteve Fernandez, Kosuke Takeuchi, Ryohsuke Mitsudome, Ryuta Kambe, Satoshi OTA, Takayuki Murooka, Yutaka Kondo, kobayu858

0.26.0 (2024-04-03)
-------------------
* refactor(common, control, planning): replace boost::optional with std::optional (`#5717 <https://github.com/autowarefoundation/autoware.universe/issues/5717>`_)
  * refactor(behavior_path_planner): replace boost optional with std
  * do it on motion utils as well.
  * up  until now
  * finally
  * fix all issue
  ---------
* refactor(control_performance_analysis): rework parameters (`#4730 <https://github.com/autowarefoundation/autoware.universe/issues/4730>`_)
  * refactor the configuration files of the node control_performance_analysis according to the new ROS node config guideline.
  Rename controller_performance_analysis.launch.xml->control_performance_analysis.launch.xml
  update the parameter information in the README.md
  * style(pre-commit): autofix
  * Update the type of a parameter in schema file.
  * revert copyright info
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* style: fix typos in PlotJuggler config files (`#3618 <https://github.com/autowarefoundation/autoware.universe/issues/3618>`_)
* refactor(control_performance_analysis): refactor structure (`#3786 <https://github.com/autowarefoundation/autoware.universe/issues/3786>`_)
  * fix(control_performance_analysis): data is not updated on failure
  * update
  * update all (tmp
  * update
  * update
  * fix pre-commit
  ---------
* fix(control_performance_analysis): fix interpolate method (`#3704 <https://github.com/autowarefoundation/autoware.universe/issues/3704>`_)
  * fix(control_performance_analysis): fix interpolate method
  * pre-commit
  * fix process die due to failure on the closest index search
  * minor refactor to use autoware_util
  ---------
* build: proper eigen deps and include (`#3615 <https://github.com/autowarefoundation/autoware.universe/issues/3615>`_)
  * build: proper eigen deps and include
  * style(pre-commit): autofix
  ---------
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* refactor(control_performance_analysis): delete default values (`#2944 <https://github.com/autowarefoundation/autoware.universe/issues/2944>`_)
  delete param
  Co-authored-by: yamazakiTasuku <tasuku.yamazaki@tier4.jp>
* chore(control, planning): add maintainers (`#2951 <https://github.com/autowarefoundation/autoware.universe/issues/2951>`_)
  * chore(control_performance_analysis): add maintainers
  * chore(external_cmd_selector): add maintainers
  * chore(joy_controller): add maintainers
  * chore(obstacle_collision_checker): add maintainers
  * chore(scenariio_selector): add maintainers
  * fix(control_performance_analysis): control/control_performance_analysis/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  ---------
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
* chore: remove motion_common dependency (`#2231 <https://github.com/autowarefoundation/autoware.universe/issues/2231>`_)
  * remove motion_common from smoother
  * remove motion_common from control_performance_analysis and simple_planning_simualtor
  * fix include
  * add include
* chore: missing topic info duration 1000 -> 5000 (`#2056 <https://github.com/autowarefoundation/autoware.universe/issues/2056>`_)
* chore(planning/control packages): organized authors and maintainers (`#1610 <https://github.com/autowarefoundation/autoware.universe/issues/1610>`_)
  * organized planning authors and maintainers
  * organized control authors and maintainers
  * fix typo
  * fix colcon test
  * fix
  Update control/external_cmd_selector/package.xml
  Update control/vehicle_cmd_gate/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/motion_velocity_smoother/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/freespace_planner/package.xml
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update planning/planning_debug_tools/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/shift_decider/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/pure_pursuit/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Update control/operation_mode_transition_manager/package.xml
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  * fix
  * fix
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
* fix(control_performance_analysis): fix choosing wrong closest trajectory point (`#1522 <https://github.com/autowarefoundation/autoware.universe/issues/1522>`_)
  * fix(control_performance_analysis): fix choosing wrong closest trajectory point
  * removed comments
  * ci(pre-commit): autofix
  * Single-parameter constructors marked explicit.
  * Removed unnecessary variable and change the struct name
  * optimized declaration of variables
  * update for parameter declaration
  * ci(pre-commit): autofix
  * changed desired steering angle calculation
  * ci(pre-commit): autofix
  * get reference of some variables
  * ci(pre-commit): autofix
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(control_performance_analysis): monitor desired and current steering tire angle in driving monitor (`#1133 <https://github.com/autowarefoundation/autoware.universe/issues/1133>`_)
  * feat(control_performance_analysis): monitor desired and current steering tire angle in driving monitor
  * ci(pre-commit): autofix
  * update readme
  * ci(pre-commit): autofix
  * update lpf for steering monitor
  * ci(pre-commit): autofix
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(control_performance_analysis): add low pass filter to control_performance_analysis tool (`#1099 <https://github.com/autowarefoundation/autoware.universe/issues/1099>`_)
  * feat(control_performance_analysis): add low pass filter to control_performance_analysis tool
  * ci(pre-commit): autofix
  * Member variables are suffixed by an underscore
  Co-authored-by: Berkay <berkay@leodrive.ai>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat(control_performance_analysis): add new functionalities to evaluate the control modules (`#659 <https://github.com/autowarefoundation/autoware.universe/issues/659>`_)
  Co-authored-by: M. Fatih Cırıt <mfc@leodrive.ai>
* fix: set Eigen include directory as SYSTEM for Humble arm64 (`#978 <https://github.com/autowarefoundation/autoware.universe/issues/978>`_)
* chore: upgrade cmake_minimum_required to 3.14 (`#856 <https://github.com/autowarefoundation/autoware.universe/issues/856>`_)
* refactor: simplify Rolling support (`#854 <https://github.com/autowarefoundation/autoware.universe/issues/854>`_)
* refactor: use autoware cmake (`#849 <https://github.com/autowarefoundation/autoware.universe/issues/849>`_)
  * remove autoware_auto_cmake
  * add build_depend of autoware_cmake
  * use autoware_cmake in CMakeLists.txt
  * fix bugs
  * fix cmake lint errors
* fix(control_performance_analysis): modify build error in rolling (`#757 <https://github.com/autowarefoundation/autoware.universe/issues/757>`_)
* fix(tier4_autoware_utils): modify build error in rolling (`#720 <https://github.com/autowarefoundation/autoware.universe/issues/720>`_)
  * fix(tier4_autoware_utils): modify build error in rolling
  * fix(lanelet2_extension): add target compile definition for geometry2
  * fix(ekf_localizer): add target compile definition for geometry2
  * fix(freespace_planning_algorithms): add target compile definition for geometry2
  * fix(interpolation): add target compile definition for geometry2
  * fix(freespace_planner): add target compile definition for geometry2
  * fix(lane_departure_checker): add target compile definition for geometry2
  * fix(map_based_prediction): add target compile definition for geometry2
  * fix(ground_segmentation): add target compile definition for geometry2
  * fix(motion_velocity_smoother): add target compile definition for geometry2
  * fix(multi_object_tracker): add target compile definition for geometry2
  * fix(trajectory_follower): add target compile definition for geometry2
  * fix(control_performance_analysis): add target compile definition for geometry2
  * fix(detected_object_validation): add target compile definition for geometry2
  * fix(goal_distance_calculator): add target compile definition for geometry2
  * fix(ndt_scan_matcher): add target compile definition for geometry2
  * fix(route_handler): add target compile definition for geometry2
  * fix(behavior_path_planner): add target compile definition for geometry2
  * fix(mission_planner): add target compile definition for geometry2
  * fix(obstacle_avoidance_planner): add target compile definition for geometry2
  * fix(obstacle_stop_planner): add target compile definition for geometry2
  * fix(obstacle_collision_checker): add target compile definition for geometry2
  * fix(shape_estimation): add target compile definition for geometry2
  * fix(behavior_velocity_planner): add target compile definition for geometry2
  * fix(path_distance_calculator): add target compile definition for geometry2
  * fix(detection_by_tracker): add target compile definition for geometry2
  * fix(surround_obstacle_checker): add target compile definition for geometry2
  * fix(probabilistic_occupancy_grid_map): add target compile definition for geometry2
  * fix(tier4_debug_tools): add target compile definition for geometry2
  * fix(tier4_vehicle_rviz_plugin): add target compile definition for geometry2
  * fix(pure_pursuit): add target compile definition for geometry2
  * fix(trajectory_follower_nodes): add target compile definition for geometry2
  * fix(occupancy_grid_map_outlier_filter): add target compile definition for geometry2
  * fix(traffic_light_map_based_detector): add target compile definition for geometry2
  * fix(planning_error_monitor): add target compile definition for geometry2
  * fix(planning_evaluator): add target compile definition for geometry2
  * fix(lidar_centerpoint): add target compile definition for geometry2
* ci(pre-commit): update pre-commit-hooks-ros (`#625 <https://github.com/autowarefoundation/autoware.universe/issues/625>`_)
  * ci(pre-commit): update pre-commit-hooks-ros
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
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
* fix(control performance analysis): suppress warnings (`#293 <https://github.com/autowarefoundation/autoware.universe/issues/293>`_)
  * fix: delete unused variable
  * feat: add Werror
  * fix: fix clang-diagnostic-unused-private-field
* feat: rename existing packages name starting with autoware to different names (`#180 <https://github.com/autowarefoundation/autoware.universe/issues/180>`_)
  * autoware_api_utils -> tier4_api_utils
  * autoware_debug_tools -> tier4_debug_tools
  * autoware_error_monitor -> system_error_monitor
  * autoware_utils -> tier4_autoware_utils
  * autoware_global_parameter_loader -> global_parameter_loader
  * autoware_iv_auto_msgs_converter -> tier4_auto_msgs_converter
  * autoware_joy_controller -> joy_controller
  * autoware_error_monitor -> system_error_monitor(launch)
  * autoware_state_monitor -> ad_service_state_monitor
  * autoware_web_controller -> web_controller
  * remove autoware_version
  * remove autoware_rosbag_recorder
  * autoware\_*_rviz_plugin -> tier4\_*_rviz_plugin
  * fix ad_service_state_monitor
  * ci(pre-commit): autofix
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: load vehicle info default param (`#148 <https://github.com/autowarefoundation/autoware.universe/issues/148>`_)
  * update global_parameter loader readme
  * remove unused dependency
  * add default vehicle_info_param to launch files
  * fix: import os
  * Update simulator/simple_planning_simulator/launch/simple_planning_simulator.launch.py
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  * Update perception/ground_segmentation/launch/scan_ground_filter.launch.py
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  * fix dependency
  * fix scan_ground_filter.launch
  * ci(pre-commit): autofix
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
* feat: change pachage name: autoware_msgs -> tier4_msgs (`#150 <https://github.com/autowarefoundation/autoware.universe/issues/150>`_)
  * change pkg name: autoware\_*_msgs -> tier\_*_msgs
  * ci(pre-commit): autofix
  * autoware_external_api_msgs -> tier4_external_api_msgs
  * ci(pre-commit): autofix
  * fix description
  Co-authored-by: pre-commit-ci[bot] <66853113+pre-commit-ci[bot]@users.noreply.github.com>
  Co-authored-by: Takeshi Miura <57553950+1222-takeshi@users.noreply.github.com>
* feat: add control_performance_analysis (`#95 <https://github.com/autowarefoundation/autoware.universe/issues/95>`_)
  * Feature/porting control performance analysis (`#1671 <https://github.com/autowarefoundation/autoware.universe/issues/1671>`_)
  * Feature/control performance analysis (`#1212 <https://github.com/autowarefoundation/autoware.universe/issues/1212>`_)
  * First commit of kinematic_controller
  * First commit.
  * second commit
  * Just setup updated Autoware.
  * changed package name.
  * Messages variables are created.
  * Writing subscribers and publishers.
  * Writing subscribers. Traj, pose and control_values are read into the node.
  * Computing control performance variables.
  * Computing control performance variables.
  * Current velocity subscribed.
  * Acceleration performance is computed.
  * Publishing completed. Will start rqt_multiplot
  * Publishing completed. Decay rate fixed. Will start rqt_multiplot
  * rqt_multiplot first configuration.
  * Update pure_pursuit.launch
  * Update pure_pursuit.launch
  * Update .gitignore
  * Update Error.msg
  * Update control_performance_utils.cpp
  * Update ErrorStamped.msg
  * Update package.xml
  * rqt_multiplot first configuration.
  * Update controller_performance_core.cpp
  * Update controller_performance_core.cpp
  * Update CMakeLists.txt
  * Update control_performance_analysis_param.yaml
  * EPS is added for value_decay_rate.
  * There is a  bug.
  * Bug removed.
  * Bug removed.
  * lateral_acceleration is published.
  * Interpolated pose is added.
  * Update controller_performance_node.cpp
  * find Curve index bug is removed.
  * dot product on projection is updated.
  * Vehicle measured steering is included in the node and rqt_graph.
  * Review will be requested.
  * After the test:
  Three point curvature module is added. Std:vector will be fixed.
  * After the test:
  Curvature plot is added.
  * After the test:
  Fine tuned.
  * rqt curvature is modified.
  * Pure pursuit curvature is implemented and tested. Results are fine.
  * addressed some code review issues. Will replace get_pose.
  * GetPose is removed.
  * All the core review issues have been addressed.
  * Rename files
  * Porting control performance analysis
  * Apply lint
  * Add boost dependency for optional
  * Remove confusing abbreviation
  * Fix dependency in packages.xml
  * Add missing new line
  * Add comment for eigen macro
  * pre-commit fixes
  Co-authored-by: Ali BOYALI <boyali@users.noreply.github.com>
  * Fix package.xml (`#2056 <https://github.com/autowarefoundation/autoware.universe/issues/2056>`_)
  * Fix typo for control_performance_analysis (`#2328 <https://github.com/autowarefoundation/autoware.universe/issues/2328>`_)
  * fix typo
  * fix Contro -> Control
  * fix for spellcheck
  * fix
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
  * adapt to actuation cmd/status as control msg (`#646 <https://github.com/autowarefoundation/autoware.universe/issues/646>`_)
  * adapt to actuation cmd/status as control msg
  * fix readme
  * fix topics
  * fix remaing topics
  * as to pacmod interface
  * fix vehicle status
  * add header to twist
  * revert gyro_odometer_change
  * revert twist topic change
  * revert unchanged package
  * port control_performance_analysis (`#698 <https://github.com/autowarefoundation/autoware.universe/issues/698>`_)
  * port control_performance_analysis
  * rename
  * fix topic name
  * remove unnecessary depedency
  * change name of odom topic
  * add readme in control_performance_analysis (`#716 <https://github.com/autowarefoundation/autoware.universe/issues/716>`_)
  * add readme
  * update readme
  * update readme
  * Update control/control_performance_analysis/README.md
  * Update twist topic name (`#736 <https://github.com/autowarefoundation/autoware.universe/issues/736>`_)
  * Apply suggestions from code review
  Co-authored-by: Daisuke Nishimatsu <42202095+wep21@users.noreply.github.com>
  Co-authored-by: Ali BOYALI <boyali@users.noreply.github.com>
  Co-authored-by: Kenji Miyake <kenji.miyake@tier4.jp>
  Co-authored-by: Hiroki OTA <hiroki.ota@tier4.jp>
  Co-authored-by: Kenji Miyake <31987104+kenji-miyake@users.noreply.github.com>
  Co-authored-by: taikitanaka3 <65527974+taikitanaka3@users.noreply.github.com>
  Co-authored-by: Kosuke Murakami <kosuke.murakami@tier4.jp>
  Co-authored-by: Tomoya Kimura <tomoya.kimura@tier4.jp>
* Contributors: Berkay Karaman, Daisuke Nishimatsu, Hiroki OTA, Kenji Miyake, Kotaro Yoshimoto, Satoshi OTA, Takamasa Horibe, Takayuki Murooka, Takeshi Miura, Tomoya Kimura, Vincent Richard, Yuntianyi Chen, Zulfaqar Azmi, yamazakiTasuku
